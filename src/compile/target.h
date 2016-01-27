#pragma once


#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/raw_ostream.h"

#include "../global.h"
#include "../core/type.h"



namespace def {
namespace compile {

using namespace std;
using namespace llvm;


/**
 * 目标代码生成器
 */
class Target {

public:
    
    LLVMContext & context;
    Module& module;

    // 构造
    Target(LLVMContext &, Module &);
    
    // 输出 s 汇编 或 obj 文件
    void output(const string &, TargetMachine::CodeGenFileType);

private:
    bool isBinaryOutput(TargetMachine::CodeGenFileType);


};

}
}