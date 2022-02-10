# Generated by devtools/yamaker.

LIBRARY()

OWNER(
    orivej
    g:cpp-contrib
)

LICENSE(
    Apache-2.0 WITH LLVM-exception AND
    NCSA
)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

PEERDIR(
    contrib/libs/llvm12 
    contrib/libs/llvm12/include 
    contrib/libs/llvm12/lib/Analysis 
    contrib/libs/llvm12/lib/CodeGen 
    contrib/libs/llvm12/lib/CodeGen/AsmPrinter 
    contrib/libs/llvm12/lib/CodeGen/GlobalISel 
    contrib/libs/llvm12/lib/CodeGen/SelectionDAG 
    contrib/libs/llvm12/lib/IR 
    contrib/libs/llvm12/lib/MC 
    contrib/libs/llvm12/lib/Support 
    contrib/libs/llvm12/lib/Target 
    contrib/libs/llvm12/lib/Target/AArch64/MCTargetDesc 
    contrib/libs/llvm12/lib/Target/AArch64/TargetInfo 
    contrib/libs/llvm12/lib/Target/AArch64/Utils 
    contrib/libs/llvm12/lib/Transforms/CFGuard 
    contrib/libs/llvm12/lib/Transforms/Scalar 
    contrib/libs/llvm12/lib/Transforms/Utils 
)

ADDINCL(
    ${ARCADIA_BUILD_ROOT}/contrib/libs/llvm12/lib/Target/AArch64 
    contrib/libs/llvm12/lib/Target/AArch64 
)

NO_COMPILER_WARNINGS()

NO_UTIL()

SRCS(
    AArch64A53Fix835769.cpp
    AArch64A57FPLoadBalancing.cpp
    AArch64AdvSIMDScalarPass.cpp
    AArch64AsmPrinter.cpp
    AArch64BranchTargets.cpp
    AArch64CallingConvention.cpp
    AArch64CleanupLocalDynamicTLSPass.cpp
    AArch64CollectLOH.cpp
    AArch64CompressJumpTables.cpp
    AArch64CondBrTuning.cpp
    AArch64ConditionOptimizer.cpp
    AArch64ConditionalCompares.cpp
    AArch64DeadRegisterDefinitionsPass.cpp
    AArch64ExpandImm.cpp
    AArch64ExpandPseudoInsts.cpp
    AArch64FalkorHWPFFix.cpp
    AArch64FastISel.cpp
    AArch64FrameLowering.cpp
    AArch64ISelDAGToDAG.cpp
    AArch64ISelLowering.cpp
    AArch64InstrInfo.cpp
    AArch64LoadStoreOptimizer.cpp
    AArch64MCInstLower.cpp
    AArch64MachineFunctionInfo.cpp
    AArch64MacroFusion.cpp
    AArch64PBQPRegAlloc.cpp
    AArch64PromoteConstant.cpp
    AArch64RedundantCopyElimination.cpp
    AArch64RegisterInfo.cpp
    AArch64SIMDInstrOpt.cpp
    AArch64SLSHardening.cpp
    AArch64SelectionDAGInfo.cpp
    AArch64SpeculationHardening.cpp
    AArch64StackTagging.cpp
    AArch64StackTaggingPreRA.cpp
    AArch64StorePairSuppress.cpp
    AArch64Subtarget.cpp
    AArch64TargetMachine.cpp
    AArch64TargetObjectFile.cpp
    AArch64TargetTransformInfo.cpp
    GISel/AArch64CallLowering.cpp
    GISel/AArch64InstructionSelector.cpp
    GISel/AArch64LegalizerInfo.cpp
    GISel/AArch64PostLegalizerCombiner.cpp
    GISel/AArch64PostLegalizerLowering.cpp 
    GISel/AArch64PostSelectOptimize.cpp 
    GISel/AArch64PreLegalizerCombiner.cpp
    GISel/AArch64RegisterBankInfo.cpp
    SVEIntrinsicOpts.cpp
)

END()
