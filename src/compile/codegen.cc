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
            (int)value[0], 
            false);

    } else if (ISTY(String)) {
        return gen.builder.CreateGlobalStringPtr(value);

    }

    return nullptr;

#undef ISTY

}

/**
 * new 堆对象创建
 */
Value* ASTNew::codegen(Gen & gen)
{
    return obj->codegen(gen);
}

/**
 * malloc 堆内存申请
 */
Value* ASTMalloc::codegen(Gen & gen)
{
    // 类型尺寸大小
    size_t size = gen.getTypeBitSize(type);
    Value *newsz(nullptr);
    // FATAL(type->str() + " size " + Str::l2s(size))
    if (auto*cons=dynamic_cast<ASTConstant*>(len)) { 
        size = size * Str::s2l(cons->value);
        newsz = ConstantInt::get(gen.builder.getInt32Ty(), size, true);
    } else {
        newsz = gen.builder.CreateMul(
            ConstantInt::get(gen.builder.getInt32Ty(), size, true),
            gen.createLoad(len)
        );
    }
    // 申请内存空间
    auto intty = def::core::Type::get("Int");
    auto ftype = TypeFunction("malloc", intty);
    ftype.add(intty);
    Function *func = gen.getCLibFunc(ftype);
    // 解析参数
    std::vector<Value*> argvs;
    argvs.push_back(gen.createLoad( newsz ));
    // 新分配的内存地址
    Value *vptr = gen.builder.CreateCall(func, argvs);
    // 类型转换
    // 引用值的类型
    llvm::Type* vty = gen.fixType(type);
    if (is_array) { // new array 堆数组空间
        vty = ArrayType::get(vty, 0);
    }
    // 转换成指针类型
    llvm::Type* vpty = PointerType::get(vty, 0);
    // 地址值
    Value* v = gen.createLoad(vptr);
    // int 转 pointer 
    return gen.builder.CreateIntToPtr(v, vpty);
}

/**
 * delete 堆内存释放
 */
Value* ASTDelete::codegen(Gen & gen)
{
    /*
    // 引用类型用 int 32 储存
    llvm::Type* vty = gen.fixType(vptr->getType());
    // 地址值
    Value* v = gen.varyPointer(vptr);

    // pointer 转 int
    Value* adr = gen.builder.CreatePtrToInt(v, nullptr);
    */

    auto intty = def::core::Type::get("Int");
    auto ftype = TypeFunction("free", Type::get("Nil"));
    ftype.add(intty);
    Function *func = gen.getCLibFunc(ftype);
    // 解析参数
    std::vector<Value*> argvs;
    Value* vp = vptr->codegen(gen);
    if (auto *ins=cast<IntToPtrInst>(vp)) {
        vp = ins->getOperand(0);
    } else {
        FATAL("codegen delete error !")
        // vp = gen.builder.CreatePtrToInt(vp, gen.fixType(vptr->getType()));
    }
    argvs.push_back(vp);
    // 新分配的内存地址
    /*vp->dump();
    func->dump();*/
    Value *freecall = gen.builder.CreateCall(func, argvs);

    return freecall;
}

/**
 * copy 拷贝对象
 */
Value* ASTCopy::codegen(Gen & gen)
{
    // 类型
    StructType *scty = (StructType*)gen.fixType( value->getType() );

    // 分配内存
    Value *structval = gen.builder.CreateAlloca(scty);

    // 拷贝内存
    Value *val = gen.createLoad( value );
    // Value *ptr = gen.builder.CreateGEP(scty, structval, gen.getGEPidxary(0));
    gen.builder.CreateStore(val, structval);

    // 返回新地址
    return structval;

}

/**
 * 获取 Quote 引用对象的 IR
 */
Value* ASTQuote::codegen(Gen & gen)
{
    // 引用类型用 int 32 储存
    llvm::Type* vty = gen.fixType(type);
    // 地址值
    Value* v = gen.varyPointer(value);

    // pointer 转 int
    return gen.builder.CreatePtrToInt(v, vty);
}

/**
 * 从引用载入对象
 */
Value* ASTLoad::codegen(Gen & gen)
{
    auto *qty = dynamic_cast<TypeRefer*>(type);
    if (!qty) { // 载入必须为引用类型
        FATAL("ASTLoad must be <TypeReference*> !")
    }
    // 引用值的类型
    llvm::Type* vty = gen.fixType(qty->type);
    // 转换成指针类型
    llvm::Type* vpty = PointerType::get(vty, 0);
    // 地址值
    Value* v = gen.createLoad(value);
    // int 转 pointer 
    return gen.builder.CreateIntToPtr(v, vpty);
}

/**
 * 数组对象构造
 */
Value* ASTArrayConstruct::codegen(Gen & gen)
{
    // 数组类型
    llvm::Type* arrty = gen.fixType(type);

    // 数组空间分配
    return gen.builder.CreateAlloca(arrty);
}

/**
 * 数组元素访问
 */
Value* ASTArrayVisit::codegen(Gen & gen)
{
    def::core::Type* tarty = instance->getType();
    if (auto*p=dynamic_cast<TypePointer*>(tarty)) {
        tarty = p->type;
    }
    ArrayType *scty = (ArrayType*)gen.fixType(tarty);
    
    // 数组值
    Value* arrval = instance->codegen(gen);

    // 索引值
    Value* inxval = gen.createLoad(index->codegen(gen));
      // 数组元素指针
    vector<Value*> idxlist = gen.getGEPidxary(0);
    idxlist.push_back(inxval);
    
    // 返回数组访问
    return gen.builder.CreateGEP(arrval, idxlist);


}

/**
 * 数组元素赋值
 */
Value* ASTArrayAssign::codegen(Gen & gen)
{
    def::core::Type* tarty = instance->getType();
    if (auto*p=dynamic_cast<TypePointer*>(tarty)) {
        tarty = p->type;
    }
    ArrayType *scty = (ArrayType*)gen.fixType(tarty);
    
    // 数组值
    Value* arrval = instance->codegen(gen);

    // 索引值
    Value* inxval = gen.createLoad(index->codegen(gen));

    // 数组元素指针
    vector<Value*> idxlist = gen.getGEPidxary(0);
    idxlist.push_back(inxval);
    Value* elmptr = gen.builder.CreateGEP(arrval, idxlist);
    
    // 赋值
    Value* putval = gen.createLoad(value->codegen(gen));
    
    //return putval;

    // 返回数组访问
    return gen.builder.CreateStore(putval, elmptr);

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
    // 内置操作 +
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
    // 内置操作 *
    if (idname=="mul" X "Int" X "Int") {
        Value* v1 = gen.createLoad(params[0]);
        Value* v2 = gen.createLoad(params[1]);
        Value* res = gen.builder.CreateMul(v1, v2);
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

    // 条件判断
    if (idname=="eq" X "Int" X "Int") {
        return gen.builder.CreateICmpEQ(
            gen.createLoad(params[0]),
            gen.createLoad(params[1])
        );
    }




    
#ifdef DEBUG
    string llvmvn("ctmp."+fname);
#else
    string llvmvn(""); // release 版本显示函数返回值变量名
#endif


    // 内置函数
    if (idname == "putchar" X "Int") {
        idname = "putchar" X "Char"; // 统一输出 int 和 char
    }
    if (
        idname == "putchar" X "Char" ||
        idname == "abs" X "Int"
        // ... 其它内置函数
    ) {
        
        Function *func = gen.getCLibFunc(*fndef->ftype);

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
    // 函数调用，捕获的变量，先查找真实名字，再查全局唯一名称
    for (auto &p : fndef->cptvar) {
        string real_name = p.first;
        string unique_name = get<1>(p.second);
        Value *ov = gen.values->get(real_name);
        if(!ov){
            ov = gen.unique_values->get(unique_name);
        }
        ASSERT(ov, "ASTFunctionCall::codegen: cptvar '"+real_name+"' not find !")
        Value *pv = gen.varyPointer(ov);
        argvs.push_back(pv);
        // pv->dump();
    }
    // 实际参数
    for (auto &p : params) {
        // 转换结构或数组值参数为指针 
        Value *pv;
        def::core::Type *pty = p->getType();
        auto *ptty = dynamic_cast<TypePointer*>(pty);
        auto *stty = dynamic_cast<TypeStruct*>(pty);
        auto *arty = dynamic_cast<TypeArray*>(pty);
        if(ptty){
            pv = gen.varyPointer(p);
        } else if (stty || arty) {
            pv = gen.varyPointer(p);
            if (arty) { // 如果实参为数组且长度大于形参，则转换实参类型
                size_t pmidx = argvs.size();
                auto *vsarty = dynamic_cast<TypeArray*>(
                    fndef->ftype->types[pmidx]);
                // 数组类型长度转换
                if (arty->len > vsarty->len) {
                    Value *vint = gen.builder.CreatePtrToInt(pv, gen.builder.getInt32Ty());
                    auto *pty = PointerType::get(
                        ArrayType::get(gen.fixType(arty->type), vsarty->len), 0);
                    pv = gen.builder.CreateIntToPtr(vint, pty);
                } else {
                    FATAL("Function argument is an array，The actual length should be looger than the declare !");
                }
                
            }
        } else {
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
        Value *pv = gen.varyPointer(instance);
        argvs.push_back(pv);
        // pv->dump();
    }

    // 实际参数
    for (auto &p : call->params) {
        // 转换结构值参数为结构指针
        Value *pv;
        if (p->getType()->isAtomType()) {
            pv = gen.createLoad(p);
        }else{
            pv = gen.varyPointer(p);
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

    // 普通成员函数
    Value *retptr = gen.builder.CreateCall(func, argvs);
    
    return retptr;
}

/**
 * 成员访问
 */
Value* ASTMemberVisit::codegen(Gen & gen)
{
    def::core::Type* tarty = instance->getType();
    if (auto*p=dynamic_cast<TypePointer*>(tarty)) {
        tarty = p->type;
    }
    StructType *scty = (StructType*)gen.fixType(tarty);

    Value* structval = gen.varyPointer(instance);
    return gen.builder.CreateStructGEP(scty, structval, index);
    
    /*
    Value* structval = instance->codegen(gen);

    // 结构值（如函数返回值）
    if ( isa<PointerType>(scty) ) {
        return gen.builder.CreateStructGEP( scty, structval, index);
    } else {
        return gen.builder.CreateExtractValue(structval, index);
    }
    */

}

/**
 * 成员赋值
 */
Value* ASTMemberAssign::codegen(Gen & gen)
{
    def::core::Type* tarty = instance->getType();
    if (auto*p=dynamic_cast<TypePointer*>(tarty)) {
        tarty = p->type;
    }
    StructType *scty = (StructType*)gen.fixType(tarty);
   
    // Value* structval = instance->codegen(gen);
    Value* putval = gen.createLoad(value); // argvs[1]->codegen(gen);
    
    Value* structval = gen.varyPointer(instance);
    Value* ptr = gen.builder.CreateStructGEP( scty, structval, index);
    return gen.builder.CreateStore(putval, ptr);
    
    /*
    // 结构值（如函数返回值）
    if ( ! isa<PointerType>(structval->getType())) {
        // Value *ptr = gen.builder.CreateExtractValue(structval, pos);
        return gen.builder.CreateInsertValue(putval, structval, index);
    } else { // 结构值指针
        Value* ptr = gen.builder.CreateStructGEP( scty, structval, index);
        return gen.builder.CreateStore(putval, ptr);
    }
    */
}

/**
 * 获取类型构造调用的 IR
 */
Value* ASTTypeConstruct::codegen(Gen & gen)
{
    // 创建函数
    StructType *scty = (StructType*)gen.fixType( type );

    Value *structval = instance // 已经分配了内存
        ? instance->codegen(gen)
        : gen.builder.CreateAlloca(scty);
    
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
    ASSERT(val, "Variable define null value : "+name)
    /*if (value->getType()->isAtomType()) {
        val = gen.createLoad(val); // 原子类型传值，类和数组传引用
    } else{
        val = gen.varyPointer(val);
    }*/
    val = gen.varyPointer(val);
    gen.values->put(name, val); // 放入真实名字
    gen.unique_values->put(unique_name, val); // 放入全局唯一名字
    return val;
}

/**
 * 变量赋值
 */
Value* ASTVariableAssign::codegen(Gen & gen)
{
    string vname = name; // 变量名

    Value *val = value->codegen(gen);
    ASSERT(val, "Variable assign null value : "+name)

    Value *old = gen.values->get(vname);
    if(!old){
        // 支持从名字空间导入不在当前作用域的变量
        old = gen.unique_values->get(unique_name);
        vname = unique_name;
    }
    ASSERT(old, "Variable assign not find the value : "+vname)
    old = gen.varyPointer(old);

    // 赋值
    Value * sto = gen.builder.CreateStore(val, old);
    ASSERT(sto, "Variable assign cannot CreateStore");

    // 放入（必须为 Store 节点！）
    gen.values->put(name, sto);
    if(unique_name!=""){ // 可能为参数赋值
        gen.unique_values->put(unique_name, sto);
    }

    return val;
}

/**
 * 变量值 IR
 */
Value* ASTVariable::codegen(Gen & gen)
{
    Value* v = gen.values->get(name);
    if (!v) {
        // 支持从名字空间导入不在当前作用域的变量
        v = gen.unique_values->get(unique_name);
    }
    ASSERT(v, "codegen: Cannot find variable: " + name)
    return v;
}

/**
 * 指令 IR
 */
Value* ASTRet::codegen(Gen & gen)
{
    if (!value) {
        return gen.builder.CreateRetVoid();
    }
    // 函数返回
    Value *retval(nullptr);
    AST *ret(nullptr);
    /*
    if (auto*astv=dynamic_cast<ASTVariable*>(value)) {
        ret = astv->origin;
    } else {
        ret = value;
    }
    // 返回堆上的指针对象
    if (dynamic_cast<ASTNew*>(ret)) {
    */
    if (dynamic_cast<TypePointer*>(value->getType())) {
        retval = value->codegen(gen);
    } else {
        retval = gen.createLoad(value);
    }
    // 返回
    ASSERT(retval, "ASTRet null return value")
    return gen.builder.CreateRet( retval );
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

/**
 * 子作用域
 */
Value* ASTChildScope::codegen(Gen & gen)
{
    // 栈重置
    auto ostk = gen.values;
    auto nstk = new Scope(ostk);
    gen.values = nstk;
    // 解析子句
    Value* last(nullptr);
    for(auto li : childs){
        last = li->codegen(gen);
    }
    // 栈复位
    delete nstk;
    gen.values = ostk;
    return last;
}