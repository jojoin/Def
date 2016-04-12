

#include <memory>
#include <iostream>

#include "./target.h"

#include "llvm/ADT/STLExtras.h"

#include "llvm/IR/LegacyPassManager.h"

#include "llvm/CodeGen/CommandFlags.h"

#include "llvm/ADT/Triple.h"

#include "llvm/Analysis/TargetLibraryInfo.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetSubtargetInfo.h"


using namespace std;
using namespace llvm;
using namespace def::compile;




// ����
def::compile::Target::Target(LLVMContext & ct, Module & m)
    : context(ct)
    , module(m)
{
    // �������� Target
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmPrinters();
    InitializeAllAsmParsers();

    // Pass
    PassRegistry *Registry = PassRegistry::getPassRegistry();
    initializeCore(*Registry);
    initializeCodeGen(*Registry);
    initializeLoopStrengthReducePass(*Registry);
    initializeLowerIntrinsicsPass(*Registry);
    initializeUnreachableBlockElimPass(*Registry);

    // Register the target printer for --version.
    cl::AddExtraVersionPrinter(TargetRegistry::printRegisteredTargetsForVersion);
}



/**
 * �Ƿ�Ϊ���������
 */
bool def::compile::Target::isBinaryOutput(TargetMachine::CodeGenFileType ty)
{
    switch (ty) {
        case TargetMachine::CGFT_AssemblyFile:
            return false;
        case TargetMachine::CGFT_ObjectFile:
        case TargetMachine::CGFT_Null:
            return true;
    }
}
   


/**
 * ��� s ��� �� obj �ļ�
 */
void def::compile::Target::output(const string & file, TargetMachine::CodeGenFileType ety)
{
    Module *M = &module;

    Triple TheTriple;
    string deftri = sys::getDefaultTargetTriple();
    TheTriple.setTriple(deftri);

    std::string Error;
    const llvm::Target *TheTarget =
        TargetRegistry::lookupTarget(MArch, TheTriple, Error);

    TargetOptions Options = InitTargetOptionsFromCodeGenFlags();

    std::string CPUStr = getCPUStr(), FeaturesStr = getFeaturesStr();

    CodeGenOpt::Level OLvl = CodeGenOpt::Default;

    std::unique_ptr<TargetMachine> Target(
        TheTarget->createTargetMachine(
            TheTriple.getTriple(),
            CPUStr,
            FeaturesStr,
            Options,
            RelocModel,
            CMModel,
            OLvl));


    TargetLibraryInfoImpl TLII(Triple(M->getTargetTriple()));

    legacy::PassManager PM;

    PM.add(new TargetLibraryInfoWrapperPass(TLII));

    if (const DataLayout *DL = Target->getDataLayout())
        M->setDataLayout(*DL);

    setFunctionAttributes(CPUStr, FeaturesStr, *M);
    
    bool isbin = isBinaryOutput(ety);
    std::error_code cd;
    sys::fs::OpenFlags OpenFlags = sys::fs::F_Text;
    if (isbin) {
        // cout << "isbin F_None" << endl;
        OpenFlags = sys::fs::F_None;
    }

    // �½��ļ������
    auto raw = new raw_fd_ostream(file, cd, OpenFlags);
    
    // ������Ŀ������ Pass
    bool fail = Target->addPassesToEmitFile(PM, *raw, ety);

    if (fail) {
        errs() << "target does not support generation of this"
            << " file type!\n";
        return;
    }

    // ִ�� Pass ���
    PM.run(*M);

    if (isbin) { // F_None ģʽʱ�����ֶ��رգ�F_Text ģʽ�Զ��ر�
        raw->close();
    }
    
}

