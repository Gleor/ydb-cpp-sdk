# Generated by devtools/yamaker.

PROGRAM()

LICENSE(Apache-2.0 WITH LLVM-exception)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

PEERDIR(
    contrib/libs/llvm14
    contrib/libs/llvm14/include
    contrib/libs/llvm14/lib/Analysis
    contrib/libs/llvm14/lib/AsmParser
    contrib/libs/llvm14/lib/BinaryFormat
    contrib/libs/llvm14/lib/Bitcode/Reader
    contrib/libs/llvm14/lib/Bitcode/Writer
    contrib/libs/llvm14/lib/Bitstream/Reader
    contrib/libs/llvm14/lib/CodeGen
    contrib/libs/llvm14/lib/CodeGen/AsmPrinter
    contrib/libs/llvm14/lib/CodeGen/GlobalISel
    contrib/libs/llvm14/lib/CodeGen/SelectionDAG
    contrib/libs/llvm14/lib/DebugInfo/CodeView
    contrib/libs/llvm14/lib/DebugInfo/DWARF
    contrib/libs/llvm14/lib/Demangle
    contrib/libs/llvm14/lib/Extensions
    contrib/libs/llvm14/lib/Frontend/OpenMP
    contrib/libs/llvm14/lib/IR
    contrib/libs/llvm14/lib/IRReader
    contrib/libs/llvm14/lib/Linker
    contrib/libs/llvm14/lib/MC
    contrib/libs/llvm14/lib/MC/MCDisassembler
    contrib/libs/llvm14/lib/MC/MCParser
    contrib/libs/llvm14/lib/Object
    contrib/libs/llvm14/lib/Passes
    contrib/libs/llvm14/lib/ProfileData
    contrib/libs/llvm14/lib/Remarks
    contrib/libs/llvm14/lib/Support
    contrib/libs/llvm14/lib/Target
    contrib/libs/llvm14/lib/Target/AArch64
    contrib/libs/llvm14/lib/Target/AArch64/AsmParser
    contrib/libs/llvm14/lib/Target/AArch64/MCTargetDesc
    contrib/libs/llvm14/lib/Target/AArch64/TargetInfo
    contrib/libs/llvm14/lib/Target/AArch64/Utils
    contrib/libs/llvm14/lib/Target/ARM
    contrib/libs/llvm14/lib/Target/ARM/AsmParser
    contrib/libs/llvm14/lib/Target/ARM/MCTargetDesc
    contrib/libs/llvm14/lib/Target/ARM/TargetInfo
    contrib/libs/llvm14/lib/Target/ARM/Utils
    contrib/libs/llvm14/lib/Target/BPF
    contrib/libs/llvm14/lib/Target/BPF/AsmParser
    contrib/libs/llvm14/lib/Target/BPF/MCTargetDesc
    contrib/libs/llvm14/lib/Target/BPF/TargetInfo
    contrib/libs/llvm14/lib/Target/NVPTX
    contrib/libs/llvm14/lib/Target/NVPTX/MCTargetDesc
    contrib/libs/llvm14/lib/Target/NVPTX/TargetInfo
    contrib/libs/llvm14/lib/Target/PowerPC
    contrib/libs/llvm14/lib/Target/PowerPC/AsmParser
    contrib/libs/llvm14/lib/Target/PowerPC/MCTargetDesc
    contrib/libs/llvm14/lib/Target/PowerPC/TargetInfo
    contrib/libs/llvm14/lib/Target/X86
    contrib/libs/llvm14/lib/Target/X86/AsmParser
    contrib/libs/llvm14/lib/Target/X86/MCTargetDesc
    contrib/libs/llvm14/lib/Target/X86/TargetInfo
    contrib/libs/llvm14/lib/TextAPI
    contrib/libs/llvm14/lib/Transforms/AggressiveInstCombine
    contrib/libs/llvm14/lib/Transforms/CFGuard
    contrib/libs/llvm14/lib/Transforms/Coroutines
    contrib/libs/llvm14/lib/Transforms/IPO
    contrib/libs/llvm14/lib/Transforms/InstCombine
    contrib/libs/llvm14/lib/Transforms/Instrumentation
    contrib/libs/llvm14/lib/Transforms/ObjCARC
    contrib/libs/llvm14/lib/Transforms/Scalar
    contrib/libs/llvm14/lib/Transforms/Utils
    contrib/libs/llvm14/lib/Transforms/Vectorize
    contrib/libs/llvm14/tools/polly/lib
    contrib/libs/llvm14/tools/polly/lib/External/isl
    contrib/libs/llvm14/tools/polly/lib/External/ppcg
)

ADDINCL(
    contrib/libs/llvm14/tools/bugpoint
)

NO_COMPILER_WARNINGS()

NO_UTIL()

SRCS(
    BugDriver.cpp
    CrashDebugger.cpp
    ExecutionDriver.cpp
    ExtractFunction.cpp
    FindBugs.cpp
    Miscompilation.cpp
    OptimizerDriver.cpp
    ToolRunner.cpp
    bugpoint.cpp
)

END()