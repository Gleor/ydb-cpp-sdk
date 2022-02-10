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
    contrib/libs/llvm12/lib/BinaryFormat 
    contrib/libs/llvm12/lib/MC 
    contrib/libs/llvm12/lib/MC/MCDisassembler 
    contrib/libs/llvm12/lib/Support 
    contrib/libs/llvm12/lib/Target/X86/TargetInfo 
)

ADDINCL(
    ${ARCADIA_BUILD_ROOT}/contrib/libs/llvm12/lib/Target/X86 
    contrib/libs/llvm12/lib/Target/X86 
    contrib/libs/llvm12/lib/Target/X86/MCTargetDesc 
)

NO_COMPILER_WARNINGS()

NO_UTIL()

SRCS(
    X86ATTInstPrinter.cpp
    X86AsmBackend.cpp
    X86ELFObjectWriter.cpp
    X86InstComments.cpp
    X86InstPrinterCommon.cpp
    X86IntelInstPrinter.cpp
    X86MCAsmInfo.cpp
    X86MCCodeEmitter.cpp
    X86MCTargetDesc.cpp
    X86MachObjectWriter.cpp
    X86ShuffleDecode.cpp
    X86WinCOFFObjectWriter.cpp
    X86WinCOFFStreamer.cpp
    X86WinCOFFTargetStreamer.cpp
)

END()
