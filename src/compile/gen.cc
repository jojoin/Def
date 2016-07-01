/**
 *
 */

#include "gen.h"

#include "../sys/debug.h"
#include "../core/error.h"
#include "../core/ast.h"
#include "../parse/analysis.h"

using namespace std;
using namespace llvm;
using namespace def::core;
using namespace def::compile;
using namespace def::parse;


// ��ȡ��Ԫ��ָ��ѡ����
vector<Value*> Gen::getGEPidxary(int i1, int i2)
{
    vector<Value*> idxlist;
    if(i1>=0) idxlist.push_back(ConstantInt::get( builder.getInt32Ty(), i1, true));
    if(i2>=0) idxlist.push_back(ConstantInt::get( builder.getInt32Ty(), i2, true));
    return idxlist;
}

/**
 * ��ȡ����
 *
Value* Gen::getValue(const string & name)
{
    auto rel = values.find(name);
    if (rel != values.end()) {
        return rel->second;
    }
    // FATAL("codegen: Cannot find variable '"+name+"' !")
    return nullptr;
}
*/

/**
 * ���ñ���
 * ���ؾɱ���
 *
Value* Gen::putValue(const string & name, Value* v)
{
    Value* old = getValue(name);
    values[name] = v;
    return old;
}
*/

/**
 * �� AST ����ɿ�����Ϊ��������ʹ��
 */
Value* Gen::varyPointer(AST* ast)
{
    // AST* ast = (AST*)p;
    Value *v = ast->codegen(*this);

    Value *val = varyPointer(v);

    // ����Ǳ��������ظ�ֵ
    if (auto *vari = dynamic_cast<ASTVariable*>(ast)) {
        values->put(vari->name, val);
    }

    return val;
}
Value* Gen::varyPointer(Value* val)
{
    // Value *val = (Value *)v;
    llvm::Type* ty = val->getType();
    
    // �ṹ��������ȡ�õ�ַ
    if (isa<PointerType>(ty) && 
        (isa<StructType>(ty) || isa<ArrayType>(ty) )
    ) {
        return builder.CreateGEP(ty, val, getGEPidxary(0));
    }

    // int ת ptr
    if(isa<IntToPtrInst>(val)
        ){
        return val;
    }
    
    // Alloca �����ڴ�
    if( isa<AllocaInst>(val) ){
        return val;
    }

    // Load �����ڴ�
    if( isa<LoadInst>(val) ){
        auto * ldis = (LoadInst*)val;
        return ldis->getPointerOperand();
    }

    // Store д���ڴ�
    if ( isa<StoreInst>(val) ){
        auto * stis = (StoreInst*)val;
        return stis->getPointerOperand();
    }

    // ָ��
    if ( isa<PointerType>(ty) ){ 
        return val;
    }

    // �������͵�ֵ�����·����ڴ�
    Value *aoc = builder.CreateAlloca(ty);
    builder.CreateStore(val, aoc); // ����
    return aoc;
    


}




/**
 * ��������
 */
Value* Gen::createLoad(AST* p)
{
    AST* ast = (AST*)p;
    // def::parse::Type *ty = Analysis::getType(ast);
    Value *val = ast->codegen(*this);

    return createLoad(val);

}
Value* Gen::createLoad(Value* val)
{
    // Value *val = (Value*)v;
    llvm::Type *ty = val->getType();

    // Store ����������ӵ�ַ����������������֧�������������򣡣�
    if ( isa<StoreInst>(val) ) {
        auto * stis = (StoreInst*)val;
        // return stis->getValueOperand();
        val = stis->getPointerOperand();
        return builder.CreateLoad(val);
    }

    // ָ��ڵ�
    if( isa<PointerType>(ty) ) {
        // ��Ҫ�ӵ�ַ��������
        return builder.CreateLoad(val);
    }
    
    return val;

}



/**
 * ��������
 */
Function* Gen::createFunction(AST* p)
{
    if(auto call = dynamic_cast<ASTFunctionCall*>(p)){
        return createFunctionByCall(call);
    } else if(auto fndef = dynamic_cast<ASTFunctionDefine*>(p)){
        return createFunctionByDefine(fndef);
    }
    FATAL("Gen::createFunction can't create !");
}
Function* Gen::createFunctionByCall(AST* call)
{
    return createFunctionByDefine(((ASTFunctionCall*)call)->fndef);
}
Function* Gen::createFunctionByDefine(AST* fdf)
{
    auto fndef = (ASTFunctionDefine*)fdf;
    string fname = fndef->ftype->name;
    string idname = fndef->getIdentify();
    // ���һ���
    Function *func = module.getFunction(idname);
    if(func){
        // cout << "module.getFunction " << fname << endl;
        return func; // ���ػ���
    }
    
    // ����ɵı���
    auto old_values = values;
    auto old_unique_values = unique_values;
    values = new Scope(old_values);
    unique_values = values;

    // ����ջ
    vector<string> cpt_name;
    vector<def::core::Type*> cpt_type;
    
    // ��ʵ���������Ǿ�̬��
    if (fndef->belong && ! fndef->is_static_member) {
        cpt_name.push_back(DEF_MEMFUNC_ISTC_PARAM_NAME);
        cpt_type.push_back(fndef->belong->type);
    }

    // ����ı���
    for (auto &p : fndef->cptvar) { // ���ò�������
        cpt_name.push_back(p.first);
        cpt_type.push_back(get<0>(p.second));
    }

    // ������������
    FunctionType *fty = (FunctionType*)fixType( 
        fndef->ftype, 
        & cpt_type
    );
    
    // ��������
    func = Function::Create(fty, Function::ExternalLinkage, idname, &module);

    // ���ú�������
    int idx = 0;
    int idx2 = 0;
    int cptnum = cpt_name.size();
    // cout << "values  = = = = "+call->fndef->ftype->name+" = = = = : " << endl;
    for (auto &Arg : func->args()) { // ���ò���
        string name;
        if (idx<cptnum) {
            // ��ʽ�������������������ʹ����ʵ���ֶ���ȫ��Ψһ����
            name = cpt_name[idx];
        } else {
            // ʵ�ʲ���
            name = fndef->ftype->tabs[idx2];
            idx2++;
        }
        values->put(name, &Arg);
        Arg.setName(name);
        idx++;
        // cout << "values[name]: " << name << endl;
    }
    
    // ����������
    ASSERT(fndef->body, "codegen: Cannot find function '"+fname+"' body !")

    // ����ɵĲ����
    BasicBlock *old_block = builder.GetInsertBlock();
    BasicBlock *new_block = BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(new_block);
    Value* last(nullptr);
    AST* tail(nullptr);
    size_t len = fndef->body->childs.size();
    for (auto &li : fndef->body->childs) {
        if (Value* v = li->codegen(*this)) {
            last = v;
            tail = li;
        }
    }

    // �Ƿ�Ϊ���캯��
    if (! last || fndef->is_construct) {
        builder.CreateRetVoid();
    }else if ( ! isa<ReturnInst>(last)) {
        // ���������һ���Զ���Ϊ����ֵ������ָ�룬����ֵ���� Load
        FATAL("function need a return value !")
        // createRet(last);
    }else {
    }

    // ����� ��λ
    builder.SetInsertPoint(old_block);
    
    // ��λ��������ջ
    delete values;
    values = old_values;
    unique_values = old_unique_values;

    return func;
}



/**
 * ����ת��
 */
llvm::Type* Gen::fixType(def::core::Type* ty, vector<def::core::Type*>* append)
{

#define ISTY(T) def::core::Type##T* obj = dynamic_cast<def::core::Type##T*>(ty)

    // ԭ������
    if (ISTY(Nil)) {
        return builder.getVoidTy();
    } else if (ISTY(Bool)) {
        return builder.getInt1Ty();
    } else if (ISTY(Int)) {
        return builder.getInt32Ty();
    } else if (ISTY(Float)) {
        return builder.getFloatTy();
    } else if (ISTY(Char)) {
        return builder.getInt32Ty();
    } else if (ISTY(String)) {
        // return PointerType::get(builder.getInt8Ty(), 0);
        return builder.getInt8PtrTy();
    }
    
    // ָ������
    if (ISTY(Pointer)) {
        return PointerType::get(fixType(obj->type), 0);
    }
    
    // ��������
    if (ISTY(Refer)) {
        return builder.getInt32Ty();
    }
    
    // ��������
    if (ISTY(Array)) {
        // ����ֵ������
        llvm::Type* vty = fixType(obj->type);
        // ת������������
        return ArrayType::get(vty, obj->len);
    }

    // ��������
    if (ISTY(Function)) {
        std::vector<llvm::Type*> ptys;
        // �����׷�ӵ�
        if (append) {
            for (auto &p : *append) {
                auto *pty = fixType(p);
                // ׷�ӵ�����ʼ����ָ�뷽ʽ����
                if ( ! isa<PointerType>(pty)) {
                    pty = PointerType::get(pty, 0);
                }
                ptys.push_back( pty ); // ��������
            }
        }
        for (auto &p : obj->types) {
            auto *pty = fixType(p);
            // ���������ָ�봫�ݣ���������ֵ����
            if (dynamic_cast<TypeStruct*>(p) ||
                dynamic_cast<TypeArray*>(p) 
                ) {
                if (!isa<PointerType>(pty)) {
                    pty = PointerType::get(pty, 0);
                }
            }
            ptys.push_back( pty ); // ��������
        }
        // ���ؽṹ����ֵ��Ҳ���ص�ַ
        auto rty = fixType(obj->ret);
        // rty = rty ? rty : builder.getVoidTy();
        /*
        if (dynamic_cast<TypeClass*>(obj->ret)) {
            rty = PointerType::get(rty, 0);
        }*/
        return FunctionType::get(rty, ptys, false);
    }

    // Struct ������
    if (ISTY(Struct)) {

        // �ÿ�ͷ�ĵ�ű�ʶ����
        string name = obj->getIdentify();

        // �����Ƿ��Ѿ�����
        auto* scty = module.getTypeByName(name);
        if(scty){
            return scty;
        }

        // ����������
        scty = StructType::create(context, name);
        // ������
        std::vector<llvm::Type*> ptys;
        for (auto &p : obj->types) {
            ptys.push_back( fixType(p) ); // ��������
        }
        scty->setBody(ptys);
        // auto* llty = StructType::get(context, ptys, false);
        // scty->setName(obj->name);

        return scty;

        //return llty;
    }


    ASSERT(0, "Cannot llvm::Type* Gen::fixType");
    


#undef ISTY

}


// �ڽ���������ת��
llvm::Type* Gen::fixBuiltinFunctionType(def::core::TypeFunction* fty)
{
    std::vector<llvm::Type*> ptys;
    for (auto &p : fty->types) {
        auto *pty = fixType(p);
        // if (1 || dynamic_cast<TypeStruct*>(p)) {
        // if ( ! isa<PointerType>(pty)) {
            // ͨ��ָ�봫�ݽṹ���Ͳ���
            // pty = PointerType::get(pty, 0);
        // }
        ptys.push_back( pty ); // ��������
    }
    // ���ؽṹ����ֵ��Ҳ���ص�ַ
    auto rty = fixType(fty->ret);
    // rty = rty ? rty : builder.getVoidTy();
    /*
    if (dynamic_cast<TypeClass*>(obj->ret)) {
        rty = PointerType::get(rty, 0);
    }*/
    return FunctionType::get(rty, ptys, false);
}



// ��ȡ�����ڴ���
int Gen::getTypeBitSize(def::core::Type* type)
{
    if (auto* scty = dynamic_cast<TypeStruct*>(type)) {
        size_t sz = 0;
        for (auto ty : scty->types) {
            sz += getTypeBitSize(ty);
        }
        return sz;
    }

    // ����ԭʼ����
    auto* vty = fixType(type);
    // ���ͳߴ��С
    return vty->getScalarSizeInBits();
}



// ��ȡ C �⺯��
Function* Gen::getCLibFunc(def::core::TypeFunction ftype)
{
    string fname = ftype.name;
    Function *func = module.getFunction(fname);
    if (!func) {
        FunctionType *fty = (FunctionType*)fixBuiltinFunctionType( &ftype );
        func = Function::Create(fty, Function::ExternalLinkage, fname, &module);
        // gen.functions[fname] = func; // ����
    }

    return func;
}