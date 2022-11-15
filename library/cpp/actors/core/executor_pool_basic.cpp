#include "executor_pool_basic.h"
#include "probes.h"
#include "mailbox.h"
#include <library/cpp/actors/util/affinity.h>
#include <library/cpp/actors/util/datetime.h>

#ifdef _linux_
#include <pthread.h>
#endif

namespace NActors {
    LWTRACE_USING(ACTORLIB_PROVIDER);

    constexpr TDuration TBasicExecutorPool::DEFAULT_TIME_PER_MAILBOX;

    TBasicExecutorPool::TBasicExecutorPool(
        ui32 poolId,
        ui32 threads,
        ui64 spinThreshold,
        const TString& poolName,
        TAffinity* affinity,
        TDuration timePerMailbox,
        ui32 eventsPerMailbox,
        int realtimePriority,
        ui32 maxActivityType)
        : TExecutorPoolBase(poolId, threads, affinity, maxActivityType)
        , SpinThreshold(spinThreshold)
        , SpinThresholdCycles(spinThreshold * NHPTimer::GetCyclesPerSecond() * 0.000001) // convert microseconds to cycles
        , Threads(new TThreadCtx[threads])
        , PoolName(poolName)
        , TimePerMailbox(timePerMailbox)
        , EventsPerMailbox(eventsPerMailbox)
        , RealtimePriority(realtimePriority)
        , ThreadUtilization(0)
        , MaxUtilizationCounter(0)
        , MaxUtilizationAccumulator(0)
        , ThreadCount(threads)
    {
        auto semaphore = TSemaphore();
        Semaphore = semaphore.ConverToI64();
    }

    TBasicExecutorPool::TBasicExecutorPool(const TBasicExecutorPoolConfig& cfg)
        : TBasicExecutorPool(
            cfg.PoolId,
            cfg.Threads,
            cfg.SpinThreshold,
            cfg.PoolName,
            new TAffinity(cfg.Affinity),
            cfg.TimePerMailbox,
            cfg.EventsPerMailbox,
            cfg.RealtimePriority,
            cfg.MaxActivityType
        )
    {}

    TBasicExecutorPool::~TBasicExecutorPool() {
        Threads.Destroy();
    }

    bool TBasicExecutorPool::GoToBeBlocked(TThreadCtx& threadCtx, TTimers &timers) {
        do {
            if (AtomicCas(&threadCtx.BlockedFlag, TThreadCtx::BS_BLOCKED, TThreadCtx::BS_BLOCKING)) {
                timers.HPNow = GetCycleCountFast();
                timers.Elapsed += timers.HPNow - timers.HPStart;
                if (threadCtx.BlockedPad.Park()) // interrupted
                    return true;
                timers.HPStart = GetCycleCountFast();
                timers.Blocked += timers.HPStart - timers.HPNow;
            }
        } while (AtomicGet(threadCtx.BlockedFlag) != TThreadCtx::BS_NONE && !RelaxedLoad(&StopFlag));
        return false;
    }

    bool TBasicExecutorPool::GoToSleep(TThreadCtx& threadCtx, TTimers &timers) {
        do {
            timers.HPNow = GetCycleCountFast();
            timers.Elapsed += timers.HPNow - timers.HPStart;
            if (threadCtx.Pad.Park()) // interrupted
                return true;
            timers.HPStart = GetCycleCountFast();
            timers.Parked += timers.HPStart - timers.HPNow;
        } while (AtomicLoad(&threadCtx.WaitingFlag) == TThreadCtx::WS_BLOCKED && !RelaxedLoad(&StopFlag));
        return false;
    }

    void TBasicExecutorPool::GoToSpin(TThreadCtx& threadCtx) {
        ui64 start = GetCycleCountFast();
        bool doSpin = true;
        while (true) {
            for (ui32 j = 0; doSpin && j < 12; ++j) {
                if (GetCycleCountFast() >= (start + SpinThresholdCycles)) {
                    doSpin = false;
                    break;
                }
                for (ui32 i = 0; i < 12; ++i) {
                    if (AtomicLoad(&threadCtx.WaitingFlag) == TThreadCtx::WS_ACTIVE) {
                        SpinLockPause();
                    } else {
                        doSpin = false;
                        break;
                    }
                }
            }
            if (!doSpin) {
                break;
            }
            if (RelaxedLoad(&StopFlag)) {
                break;
            }
        }
    }

    bool TBasicExecutorPool::GoToWaiting(TThreadCtx& threadCtx, TTimers &timers, bool needToBlock) {
#if defined ACTORSLIB_COLLECT_EXEC_STATS
        if (AtomicGetAndIncrement(ThreadUtilization) == 0) {
            // Initially counter contains -t0, the pool start timestamp
            // When the first thread goes to sleep we add t1, so the counter
            // becomes t1-t0 >= 0, or the duration of max utilization so far.
            // If the counter was negative and becomes positive, that means
            // counter just turned into a duration and we should store that
            // duration. Otherwise another thread raced with us and
            // subtracted some other timestamp t2.
            const i64 t = GetCycleCountFast();
            const i64 x = AtomicGetAndAdd(MaxUtilizationCounter, t);
            if (x < 0 && x + t > 0)
                AtomicStore(&MaxUtilizationAccumulator, x + t);
        }
#endif

        Y_VERIFY(AtomicLoad(&threadCtx.WaitingFlag) == TThreadCtx::WS_NONE);

        if (SpinThreshold > 0 && !needToBlock) {
            // spin configured period
            AtomicSet(threadCtx.WaitingFlag, TThreadCtx::WS_ACTIVE);
            GoToSpin(threadCtx);
            // then - sleep
            if (AtomicLoad(&threadCtx.WaitingFlag) == TThreadCtx::WS_ACTIVE) {
                if (AtomicCas(&threadCtx.WaitingFlag, TThreadCtx::WS_BLOCKED, TThreadCtx::WS_ACTIVE)) {
                    if (GoToSleep(threadCtx, timers)) {  // interrupted
                        return true;
                    }
                }
            }
        } else {
            AtomicSet(threadCtx.WaitingFlag, TThreadCtx::WS_BLOCKED);
            if (GoToSleep(threadCtx, timers)) {  // interrupted
                return true;
            }
        }

        Y_VERIFY_DEBUG(AtomicLoad(&StopFlag) || AtomicLoad(&threadCtx.WaitingFlag) == TThreadCtx::WS_RUNNING);

#if defined ACTORSLIB_COLLECT_EXEC_STATS
        if (AtomicDecrement(ThreadUtilization) == 0) {
            // When we started sleeping counter contained t1-t0, or the
            // last duration of max utilization. Now we subtract t2 >= t1,
            // which turns counter negative again, and the next sleep cycle
            // at timestamp t3 would be adding some new duration t3-t2.
            // If the counter was positive and becomes negative that means
            // there are no current races with other threads and we should
            // store the last positive duration we observed. Multiple
            // threads may be adding and subtracting values in potentially
            // arbitrary order, which would cause counter to oscillate
            // around zero. When it crosses zero is a good indication of a
            // correct value.
            const i64 t = GetCycleCountFast();
            const i64 x = AtomicGetAndAdd(MaxUtilizationCounter, -t);
            if (x > 0 && x - t < 0)
                AtomicStore(&MaxUtilizationAccumulator, x);
        }
#endif
        return false;
    }

    ui32 TBasicExecutorPool::GetReadyActivation(TWorkerContext& wctx, ui64 revolvingCounter) {
        ui32 workerId = wctx.WorkerId;
        Y_VERIFY_DEBUG(workerId < PoolThreads);

        TTimers timers;

        TThreadCtx& threadCtx = Threads[workerId];
        AtomicSet(threadCtx.WaitingFlag, TThreadCtx::WS_NONE);

        if (Y_UNLIKELY(AtomicGet(threadCtx.BlockedFlag) != TThreadCtx::BS_NONE)) {
            if (GoToBeBlocked(threadCtx, timers)) { // interrupted
                return 0;
            }
        }

        bool needToWait = false;
        bool needToBlock = false;

        TAtomic x = AtomicGet(Semaphore);
        do {
            i64 oldX = x;
            TSemaphore semaphore = TSemaphore::GetSemaphore(x);
            needToBlock = semaphore.CurrentSleepThreadCount < 0;
            needToWait = needToBlock || semaphore.OldSemaphore <= -semaphore.CurrentSleepThreadCount;

            semaphore.OldSemaphore--;
            if (needToWait) {
                semaphore.CurrentSleepThreadCount++;
            }

            x = AtomicGetAndCas(&Semaphore, semaphore.ConverToI64(), x);
            if (x == oldX) {
                break;
            }
        } while (!StopFlag);

        if (needToWait) {
            if (GoToWaiting(threadCtx, timers, needToBlock)) { // interrupted
                return 0;
            }
        } else {
            AtomicSet(threadCtx.WaitingFlag, TThreadCtx::WS_RUNNING);
        }

        // ok, has work suggested, must dequeue
        while (!RelaxedLoad(&StopFlag)) {
            if (const ui32 activation = Activations.Pop(++revolvingCounter)) {
                timers.HPNow = GetCycleCountFast();
                timers.Elapsed += timers.HPNow - timers.HPStart;
                wctx.AddElapsedCycles(IActor::ACTOR_SYSTEM, timers.Elapsed);
                if (timers.Parked > 0) {
                    wctx.AddParkedCycles(timers.Parked);
                }
                if (timers.Blocked > 0) {
                    wctx.AddBlockedCycles(timers.Blocked);
                }
                return activation;
            }
            SpinLockPause();
        }

        // stopping, die!
        return 0;
    }

    inline void TBasicExecutorPool::WakeUpLoop() {
        for (ui32 i = 0;;) {
            TThreadCtx& threadCtx = Threads[i % PoolThreads];
            switch (AtomicLoad(&threadCtx.WaitingFlag)) {
                case TThreadCtx::WS_NONE:
                case TThreadCtx::WS_RUNNING:
                    ++i;
                    break;
                case TThreadCtx::WS_ACTIVE: // in active spin-lock, just set flag
                    if (AtomicCas(&threadCtx.WaitingFlag, TThreadCtx::WS_RUNNING, TThreadCtx::WS_ACTIVE)) {
                        return;
                    }
                    break;
                case TThreadCtx::WS_BLOCKED:
                    if (AtomicCas(&threadCtx.WaitingFlag, TThreadCtx::WS_RUNNING, TThreadCtx::WS_BLOCKED)) {
                        threadCtx.Pad.Unpark();
                        return;
                    }
                    break;
                default:
                    Y_FAIL();
            }
        }
    }

    void TBasicExecutorPool::ScheduleActivationEx(ui32 activation, ui64 revolvingCounter) {
        Activations.Push(activation, revolvingCounter);
        bool needToWakeUp = false;

        TAtomic x = AtomicGet(Semaphore);
        TSemaphore semaphore = TSemaphore::GetSemaphore(x);
        do {
            needToWakeUp = semaphore.CurrentSleepThreadCount > 0;
            i64 oldX = semaphore.ConverToI64();
            semaphore.OldSemaphore++;
            if (needToWakeUp) {
                semaphore.CurrentSleepThreadCount--;
            }
            x = AtomicGetAndCas(&Semaphore, semaphore.ConverToI64(), oldX);
            if (x == oldX) {
                break;
            }
            semaphore = TSemaphore::GetSemaphore(x);
        } while (true);

        if (needToWakeUp) { // we must find someone to wake-up
            WakeUpLoop();
        }
    }

    void TBasicExecutorPool::GetCurrentStats(TExecutorPoolStats& poolStats, TVector<TExecutorThreadStats>& statsCopy) const {
        poolStats.MaxUtilizationTime = RelaxedLoad(&MaxUtilizationAccumulator) / (i64)(NHPTimer::GetCyclesPerSecond() / 1000);

        statsCopy.resize(PoolThreads + 1);
        // Save counters from the pool object
        statsCopy[0] = TExecutorThreadStats();
        statsCopy[0].Aggregate(Stats);
        // Per-thread stats
        for (size_t i = 0; i < PoolThreads; ++i) {
            Threads[i].Thread->GetCurrentStats(statsCopy[i + 1]);
        }
    }

    void TBasicExecutorPool::Prepare(TActorSystem* actorSystem, NSchedulerQueue::TReader** scheduleReaders, ui32* scheduleSz) {
        TAffinityGuard affinityGuard(Affinity());

        ActorSystem = actorSystem;

        ScheduleReaders.Reset(new NSchedulerQueue::TReader[PoolThreads]);
        ScheduleWriters.Reset(new NSchedulerQueue::TWriter[PoolThreads]);

        for (ui32 i = 0; i != PoolThreads; ++i) {
            Threads[i].Thread.Reset(
                new TExecutorThread(
                    i,
                    0, // CpuId is not used in BASIC pool
                    actorSystem,
                    this,
                    MailboxTable.Get(),
                    PoolName,
                    TimePerMailbox,
                    EventsPerMailbox));
            ScheduleWriters[i].Init(ScheduleReaders[i]);
        }

        *scheduleReaders = ScheduleReaders.Get();
        *scheduleSz = PoolThreads;
    }

    void TBasicExecutorPool::Start() {
        TAffinityGuard affinityGuard(Affinity());

        ThreadUtilization = 0;
        AtomicAdd(MaxUtilizationCounter, -(i64)GetCycleCountFast());

        for (ui32 i = 0; i != PoolThreads; ++i) {
            Threads[i].Thread->Start();
        }
    }

    void TBasicExecutorPool::PrepareStop() {
        AtomicStore(&StopFlag, true);
        for (ui32 i = 0; i != PoolThreads; ++i) {
            Threads[i].Pad.Interrupt();
            Threads[i].BlockedPad.Interrupt();
        }
    }

    void TBasicExecutorPool::Shutdown() {
        for (ui32 i = 0; i != PoolThreads; ++i)
            Threads[i].Thread->Join();
    }

    void TBasicExecutorPool::Schedule(TInstant deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Y_VERIFY_DEBUG(workerId < PoolThreads);

        Schedule(deadline - ActorSystem->Timestamp(), ev, cookie, workerId);
    }

    void TBasicExecutorPool::Schedule(TMonotonic deadline, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Y_VERIFY_DEBUG(workerId < PoolThreads);

        const auto current = ActorSystem->Monotonic();
        if (deadline < current)
            deadline = current;

        ScheduleWriters[workerId].Push(deadline.MicroSeconds(), ev.Release(), cookie);
    }

    void TBasicExecutorPool::Schedule(TDuration delta, TAutoPtr<IEventHandle> ev, ISchedulerCookie* cookie, TWorkerId workerId) {
        Y_VERIFY_DEBUG(workerId < PoolThreads);

        const auto deadline = ActorSystem->Monotonic() + delta;
        ScheduleWriters[workerId].Push(deadline.MicroSeconds(), ev.Release(), cookie);
    }

    void TBasicExecutorPool::SetRealTimeMode() const {
// TODO: musl-libc version of `sched_param` struct is for some reason different from pthread
// version in Ubuntu 12.04
#if defined(_linux_) && !defined(_musl_)
        if (RealtimePriority != 0) {
            pthread_t threadSelf = pthread_self();
            sched_param param = {RealtimePriority};
            if (pthread_setschedparam(threadSelf, SCHED_FIFO, &param)) {
                Y_FAIL("Cannot set realtime priority");
            }
        }
#else
        Y_UNUSED(RealtimePriority);
#endif
    }

    ui32 TBasicExecutorPool::GetThreadCount() const {
        return AtomicGet(ThreadCount);
    }

    void TBasicExecutorPool::SetThreadCount(ui32 threads) {
        threads = Max(1u, Min(PoolThreads, threads));
        with_lock (ChangeThreadsLock) {
            size_t prevCount = GetThreadCount();
            AtomicSet(ThreadCount, threads);

            TSemaphore semaphore = TSemaphore::GetSemaphore(AtomicGet(Semaphore));
            i64 oldX = semaphore.ConverToI64();
            semaphore.CurrentSleepThreadCount += threads - prevCount;
            semaphore.OldSemaphore -= threads - prevCount;
            AtomicAdd(Semaphore, semaphore.ConverToI64() - oldX);
        }
    }
}
