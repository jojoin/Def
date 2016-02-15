/**
 *
 */

#include "llvm/IR/Function.h"


#include "gen.h"

#include "../sys/debug.h"

#include "../core/splitfix.h"
#include "../core/ast.h"
#include "../core/error.h"
#include "../core/splitfix.h"
#include "../parse/analysis.h"

using namespace std;
using namespace llvm;
using namespace def::parse;
using namespace def::core;


#define T Object::Type


/**
 * 获取静态对象的 llvm IR
 */
Value* ASTConstant::codegen(Gen & gen)
{
    // cout<<"-Codegen::Gen Constant-"<<endl;
    
#define ISTY(T) Type##T* ty = dynamic_cast<Type##T*>(type)
   
    
    if (ISTY(Bool)) {
        return ConstantInt::get(
            gen.builder.getInt1Ty(), 
            value=="true" ? 1 : 0, 
            false);

    } else if (ISTY(Int)) {
        return ConstantInt::get(
            gen.builder.getInt32Ty(), 
            Str::s2l(value), 
            true);

    } else if (ISTY(Float)) {
        return ConstantFP::get(
            gen.builder.getFloatTy(),
            Str::s2d(value)
        );

    } else if (ISTY(Char)) {
        return ConstantInt::get(
            gen.builder.getInt32Ty(), 
            value, 
            false);

    } else if (ISTY(String)) {
        return gen.builder.CreateGlobalStringPtr(value);
    }

    return nullptr;

#undef ISTY

}

/**
 * Group
 */
Value* ASTGroup::codegen(Gen & gen)
{
    Value* last = nullptr;
    for (auto &one : childs) {
        // delete last;
        if (one) {
            auto *li = one->codegen(gen);
            if (li) {
                last = li;
            }
        }
    }
    return last;
}

/**
 * 获取静态对象的 llvm IR
 */
Value* ASTFunctionCall::codegen(Gen & gen)
{
    string fname = fndef->ftype->name;
    string idname = fndef->getIdentify();

#define X DEF_SPLITFIX_FUNCARGV
    // 内置操作
    if (idname=="bool" X "Int") {
        Value* v1 = gen.createLoad(params[0]);
        Value* res = gen.builder.CreateICmpNE(
            v1, ConstantInt::get( gen.builder.getInt32Ty(), 0, true));
        return res;
    }
    // 内置操作
    if (idname=="add" X "Int" X "Int") {
        Value* v1 = gen.createLoad(params[0]);
        Value* v2 = gen.createLoad(params[1]);
        // v1->dump();
        // v2->dump();
        Value* res = gen.builder.CreateAdd(v1, v2);
        return res;
    }
    // 内置操作 -
    if (idname=="sub" X "Int" X "Int") {
        Value* v1 = gen.createLoad(params[0]);
        Value* v2 = gen.createLoad(params[1]);
        Value* res = gen.builder.CreateSub(v1, v2);
        return res;
    }
    // 内置操作
    if (idname=="add" X "Float" X "Int") {
        Value *pv1 = gen.builder.CreateFPToSI(
            gen.createLoad(params[0]),
            gen.builder.getInt32Ty());
        Value* res = gen.builder.CreateAdd(
            pv1,
            gen.createLoad(params[1])
        );
        return res;
    }
    // 内置操作
    if (idname=="add" X "Int" X "Float") {
        Value *pv2 = gen.builder.CreateFPToSI(
            gen.createLoad(params[1]),
            gen.builder.getInt32Ty());
        Value* res = gen.builder.CreateAdd(
            gen.createLoad(params[0]),
            pv2
        );
        return res;
    }
    // 内置操作
    if (idname=="add" X "Float" X "Float") {
        Value* res = gen.builder.CreateFAdd(
            gen.createLoad(params[0]),
            gen.createLoad(params[1])
        );
        return gen.builder.CreateFPToSI(res, gen.builder.getInt32Ty());
    }

    
#ifdef DEBUG
    string llvmvn("ctmp."+fname);
#else
    string llvmvn(""); // release 版本显示函数返回值变量名
#endif


    // 内置函数
    if (
        idname == "putchar" X "Int" ||
        idname == "abs" X "Int"
    ) {
        
        Function *func = gen.module.getFunction(fname);
        // Function *func = gen.getFunction(fname);
        if (!func) {
            FunctionType *fty = (FunctionType*)gen.fixBuiltinFunctionType( fndef->ftype );
            func = Function::Create(fty, Function::ExternalLinkage, fname, &gen.module);
            // gen.functions[fname] = func; // 缓存
        }


        // 解析参数
        std::vector<Value*> argvs;
        for (auto &p : params) {
            Value *val = gen.createLoad( p );
            argvs.push_back(val);
        }
        return gen.builder.CreateCall(func, argvs, llvmvn);
    }

#undef X


    // 获得函数
    Function *func = gen.createFunction(this);


    // 解析参数
    std::vector<Value*> argvs;
    // 捕获的变量
    for (auto &p : fndef->cptvar) {
        string name = p.first;
        Value *pv = gen.varyPointer(
            gen.getValue(name)
            );
        argvs.push_back(pv);
        // pv->dump();
    }
    // 实际参数
    for (auto &p : params) {
        // 转换结构值参数为结构指针
        Value *pv;
        if (dynamic_cast<TypeStruct*>(p->getType())) {
            pv = gen.varyPointer(p);
        }else{
            pv = gen.createLoad(p);
        }
        argvs.push_back(pv);
        // pv->dump();
    }


    // 创建调用
    /*
    if (0==argvs.size()) {
        // 无参函数
        return gen.builder.CreateCall(func, None, "noargvcalltmp");
    }
    */

    // 创建调用，得到返回数据的地址
    /*
    func->dump();
    cout << "gen.builder.CreateCall: " << idname 
        << " param num: " << func->arg_size()
        << " argv num: " << argvs.size() << endl;
    */
    Value *retptr = gen.builder.CreateCall(func, argvs, llvmvn);

    // 如果返回值是对象 则取其地址
    // （临时做法：分配内存，再拷贝函数返回值进入）
    /*
    auto rty = fndef->ftype->ret;
    if (dynamic_cast<TypeStruct*>(rty)) {
        Value *retaoc = gen.builder.CreateAlloca(
            gen.fixType(rty));
        gen.builder.CreateStore(retptr, retaoc); // 复制返回值
        return retaoc; // 返回内存地址
        // return gen.builder.CreateLoad(callret);
        // callret = gen.builder.CreateGEP(gen.fixType(frty), callret, 0);
    }
    */

 

    return retptr;
}

/**
 * 成员函数调用
 */
Value* ASTMemberFunctionCall::codegen(Gen & gen)
{
    string tyname = call->fndef->ftype->name;
    string idname = call->fndef->getIdentify();
    
    // 创建获取函数
    Function *func = gen.createFunction(call);
    
    // 解析参数
    std::vector<Value*> argvs;

    // 类实例（非静态）
    if( ! call->fndef->is_static_member ){
        Value *pv = gen.varyPointer(value);
        argvs.push_back(pv);
        // pv->dump();
    }

    // 实际参数
    for (auto &p : call->params) {
        // 转换结构值参数为结构指针
        Value *pv;
        if (dynamic_cast<TypeStruct*>(p->getType())) {
            pv = gen.varyPointer(p);
        }else{
            pv = gen.createLoad(p);
        }
        argvs.push_back(pv);
        // pv->dump();
    } 

    // 创建调用
    /*
    func->dump();
    cout << "gen.builder.CreateCall: " << idname 
        << " param num: " << func->arg_size()
        << " argv num: " << argvs.size() << endl;
    cout << " ========================== " << endl;
    */

    // 如果是构造函数
    if (call->fndef->is_construct)
    {
        gen.builder.CreateCall(func, argvs);
        return argvs[0]; // 返回类实例指针
    }

    Value *retptr = gen.builder.CreateCall(func, argvs, "mfc.tmp");
    
    return retptr;
}

/**
 * 成员访问
 */
Value* ASTMemberVisit::codegen(Gen & gen)
{
    StructType *scty = (StructType*)gen.fixType(
        instance->getType()
        );

    Value* structval = instance->codegen(gen);

    // 结构值（如函数返回值）
    if (isa<CallInst>(structval)) {
        return gen.builder.CreateExtractValue(structval, index);
        // 结构值指针
    } else {
        return gen.builder.CreateStructGEP( scty, structval, index);
    }
}

/**
 * 成员赋值
 */
Value* ASTMemberAssign::codegen(Gen & gen)
{
    StructType *scty = (StructType*)gen.fixType(
        instance->getType()
        );
        
    Value* structval = instance->codegen(gen);
    Value* putval = gen.createLoad(value); // argvs[1]->codegen(gen);

    // 结构值（如函数返回值）
    if (isa<CallInst>(structval)) {
        // Value *ptr = gen.builder.CreateExtractValue(structval, pos);
        return gen.builder.CreateInsertValue(putval, structval, index);
    } else { // 结构值指针
        Value* ptr = gen.builder.CreateStructGEP( scty, structval, index);
        return gen.builder.CreateStore(putval, ptr);
    }
}

/**
 * 获取类型构造调用的 IR
 */
Value* ASTTypeConstruct::codegen(Gen & gen)
{
    // 创建函数
    StructType *scty = (StructType*)gen.fixType( type );

    AllocaInst *structval = gen.builder.CreateAlloca(scty);
    
    if (! bare ) { // 不是空构造
        int len = childs.size();
        for (int i = 0; i < len; i++) {
            AST *child = childs[i];
            Value *val = gen.createLoad( child );
            Value *ptr = gen.builder.CreateStructGEP(
                scty, structval, i);
            gen.builder.CreateStore(val, ptr);
        }
    }

    return structval; // gen.builder.CreateLoad(structval);
}

/**
 * 变量初始化 IR
 */
Value* ASTVariableDefine::codegen(Gen & gen)
{
    Value *val = value->codegen(gen);
    val = gen.varyPointer(val);
    gen.putValue(name, val); // 放入
    return val;
}

/**
 * 变量赋值
 */
Value* ASTVariableAssign::codegen(Gen & gen)
{
    // Value *val = value->codegen(gen);
    
    Value *val = value->codegen(gen);


    Value *old = gen.getValue(name);
    old = gen.varyPointer(old);

    // 赋值
    gen.builder.CreateStore(val, old);

    // 放入
    gen.putValue(name, old); 

    // gen.putValue(name, val); 

    return val;
}

/**
 * 变量值 IR
 */
Value* ASTVariable::codegen(Gen & gen)
{
    Value* v = gen.getValue(name);
    if (!v) {
        FATAL("codegen: Cannot find variable '"+name+"' !")
    }
    return v;
}

/**
 * 指令 IR
 */
Value* ASTRet::codegen(Gen & gen)
{
    // 函数返回
    return gen.builder.CreateRet(
        gen.createLoad(value)
        // value->codegen(gen)
    );
}

/**
 * if 条件分支
 */
Value* ASTIf::codegen(Gen & gen)
{
    // if 节点返回值
    Value *v_ret(nullptr);
    auto *thefunc = gen.builder.GetInsertBlock()->getParent();

    Value *v_cond = cond->codegen(gen);
    auto *b_then = BasicBlock::Create(gen.context, "then", thefunc);
    auto *b_else = BasicBlock::Create(gen.context, "else", thefunc);
    auto *b_merge = BasicBlock::Create(gen.context, "ifcont", thefunc);
    
    // 跳转分支
    gen.builder.CreateCondBr(v_cond, b_then, b_else);
    
    // then block
    gen.builder.SetInsertPoint(b_then);
    Value *v_then = pthen->codegen(gen);
    gen.builder.CreateBr(b_merge);
    b_then = gen.builder.GetInsertBlock();
    
    // else block
    gen.builder.SetInsertPoint(b_else);
    Value *v_else = pelse ? pelse->codegen(gen) : nullptr;
    gen.builder.CreateBr(b_merge);
    b_else = gen.builder.GetInsertBlock();
    
    // merge block
    gen.builder.SetInsertPoint(b_merge);
    // if 分支类型一致 HasHungOffUses
    if (canphi) {
        PHINode *phi = gen.builder.CreatePHI(
            v_then->getType(), 2, "iftmp");
        phi->addIncoming(v_then, b_then);
        phi->addIncoming(v_else, b_else);
        v_ret = phi;
    }
    
    return v_ret;
}

/**
 * while 循环
 */
Value* ASTWhile::codegen(Gen & gen)
{
    

    // while 节点返回值
    Value *v_ret(nullptr);
    auto *thefunc = gen.builder.GetInsertBlock()->getParent();
    // BasicBlock *b_current = gen.builder.GetInsertBlock();
    auto *b_cond = BasicBlock::Create(gen.context, "loopcond", thefunc);
    auto *b_loop = BasicBlock::Create(gen.context, "loopbody", thefunc);
    auto *b_end = BasicBlock::Create(gen.context, "loopend", thefunc);
    // 循环条件判断
    gen.builder.CreateBr(b_cond);
    gen.builder.SetInsertPoint(b_cond);
    Value *v_cond = cond->codegen(gen);
    gen.builder.CreateCondBr(v_cond, b_loop, b_end);
    // 执行循环体
    gen.builder.SetInsertPoint(b_loop);
    Value *v_body = body->codegen(gen);
    gen.builder.CreateBr(b_cond);
    
    /*
    PHINode *phi = gen.builder.CreatePHI(
        gen.builder.getInt1Ty(), 2, "whilecond");
    phi->addIncoming(v_start, b_current);
    */

    gen.builder.SetInsertPoint(b_end);

    return nullptr;




}