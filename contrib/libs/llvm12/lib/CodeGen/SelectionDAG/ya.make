# Generated by devtools/yamaker.

LIBRARY()

OWNER(
    orivej
    g:cpp-contrib
)

LICENSE(Apache-2.0 WITH LLVM-exception)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

PEERDIR(
    contrib/libs/llvm12 
    contrib/libs/llvm12/include 
    contrib/libs/llvm12/lib/Analysis 
    contrib/libs/llvm12/lib/CodeGen 
    contrib/libs/llvm12/lib/IR 
    contrib/libs/llvm12/lib/MC 
    contrib/libs/llvm12/lib/Support 
    contrib/libs/llvm12/lib/Target 
    contrib/libs/llvm12/lib/Transforms/Utils 
)

ADDINCL(
    contrib/libs/llvm12/lib/CodeGen/SelectionDAG
)

NO_COMPILER_WARNINGS()

NO_UTIL()

SRCS(
    DAGCombiner.cpp
    FastISel.cpp
    FunctionLoweringInfo.cpp
    InstrEmitter.cpp
    LegalizeDAG.cpp
    LegalizeFloatTypes.cpp
    LegalizeIntegerTypes.cpp
    LegalizeTypes.cpp
    LegalizeTypesGeneric.cpp
    LegalizeVectorOps.cpp
    LegalizeVectorTypes.cpp
    ResourcePriorityQueue.cpp
    ScheduleDAGFast.cpp
    ScheduleDAGRRList.cpp
    ScheduleDAGSDNodes.cpp
    ScheduleDAGVLIW.cpp
    SelectionDAG.cpp
    SelectionDAGAddressAnalysis.cpp
    SelectionDAGBuilder.cpp
    SelectionDAGDumper.cpp
    SelectionDAGISel.cpp
    SelectionDAGPrinter.cpp
    SelectionDAGTargetInfo.cpp
    StatepointLowering.cpp
    TargetLowering.cpp
)

END()
