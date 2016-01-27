
@echo off
echo - - - - - - - - - - - - - - - - - - - - - -
echo - Compile def vm c++ source code to def.exe
echo - - - - - - - - - - - - - - - - - - - - - -

cd ..\src

:: cpp文件目录
set "cpp=*.cc *\*.cc"
set "llvm=-I..\llvm\include -L..\llvm\Release\lib -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -lLLVMIRReader -lgtest_main -lgtest -lLLVMMCJIT -lLLVMOrcJIT -lLLVMDebugInfoDWARF -lLLVMTableGen -lLLVMLTO -lLLVMObjCARCOpts -lLLVMLinker -lLLVMBitWriter -lLLVMPasses -lLLVMLineEditor -lLLVMInterpreter -lLLVMExecutionEngine -lLLVMRuntimeDyld -lLLVMLibDriver -lLLVMOption -lLLVMMIRParser -lLLVMAsmParser -lLLVMDebugInfoPDB -lLLVMXCoreDisassembler -lLLVMXCoreCodeGen -lLLVMXCoreDesc -lLLVMXCoreInfo -lLLVMXCoreAsmPrinter -lLLVMX86Disassembler -lLLVMX86AsmParser -lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils -lLLVMSystemZDisassembler -lLLVMSystemZCodeGen -lLLVMSystemZAsmParser -lLLVMSystemZDesc -lLLVMSystemZInfo -lLLVMSystemZAsmPrinter -lLLVMSparcDisassembler -lLLVMSparcCodeGen -lLLVMSparcAsmParser -lLLVMSparcDesc -lLLVMSparcInfo -lLLVMSparcAsmPrinter -lLLVMPowerPCDisassembler -lLLVMPowerPCCodeGen -lLLVMPowerPCAsmParser -lLLVMPowerPCDesc -lLLVMPowerPCInfo -lLLVMPowerPCAsmPrinter -lLLVMNVPTXCodeGen -lLLVMNVPTXDesc -lLLVMNVPTXInfo -lLLVMNVPTXAsmPrinter -lLLVMMSP430CodeGen -lLLVMMSP430Desc -lLLVMMSP430Info -lLLVMMSP430AsmPrinter -lLLVMMipsDisassembler -lLLVMMipsCodeGen -lLLVMMipsAsmParser -lLLVMMipsDesc -lLLVMMipsInfo -lLLVMMipsAsmPrinter -lLLVMHexagonDisassembler -lLLVMHexagonCodeGen -lLLVMHexagonDesc -lLLVMHexagonInfo -lLLVMCppBackendCodeGen -lLLVMCppBackendInfo -lLLVMBPFCodeGen -lLLVMBPFDesc -lLLVMBPFInfo -lLLVMBPFAsmPrinter -lLLVMARMDisassembler -lLLVMARMCodeGen -lLLVMARMAsmParser -lLLVMARMDesc -lLLVMARMInfo -lLLVMARMAsmPrinter -lLLVMAMDGPUCodeGen -lLLVMipo -lLLVMVectorize -lLLVMAMDGPUAsmParser -lLLVMAMDGPUUtils -lLLVMAMDGPUDesc -lLLVMAMDGPUInfo -lLLVMAMDGPUAsmPrinter -lLLVMAArch64Disassembler -lLLVMMCDisassembler -lLLVMAArch64CodeGen -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMCodeGen -lLLVMTarget -lLLVMScalarOpts -lLLVMProfileData -lLLVMObject -lLLVMBitReader -lLLVMInstCombine -lLLVMInstrumentation -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMCore -lLLVMAArch64AsmParser -lLLVMMCParser -lLLVMAArch64Desc -lLLVMAArch64Info -lLLVMAArch64AsmPrinter -lLLVMMC -lLLVMAArch64Utils -lLLVMSupport"

:: 开始编译 -static
g++ %cpp% %llvm% -std=c++11 -m32 -s -DNDEBUG -static -o ..\build\def.exe




:: 合并文件 exec.cc
:: set "target=.\vm\exec.cc"
:: type .\exec\_header.c > %target%
:: for %%f in (.\exec\*.cc) do type %%f >> %target%
:: type .\exec\_footer.c >> %target%

:: cpp文件目录
:: set "cpp=.\*.cc .\util\*.cc .\parse\*.cc .\object\*.cc .\vm\*.cc"

:: 开始编译 -static
:: g++ %cpp% -std=c++11 -w -static -o ..\build\def.exe

cd ..\build

:: pause


