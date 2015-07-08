/**
 * 调用变量自带函数
 * @name 函数名
 * @argv 参数列表
 */
DO* Exec::Objfunc(Node* left, Node* para)
{
    // 检测是否为变量自带函数调用
    if(NT::MemberAccess==left->type){

        DO *base = Evaluat( left->Left() );
        string name = left->Right()->GetName();
        return Objfunc(base, name, para);
    }

    return NULL;
}
// 分支路由
DO* Exec::Objfunc(DO* base, string name, Node* para)
{
    // cout<<"-Exec::Objfunc-"<<endl;
    // LOCALIZE_gc
    OT bt = base->type;

    // 系统模块
    if(bt==OT::Sysmod){
        ObjectSysmod *mod = (ObjectSysmod*)base;
        string modname = mod->GetName();

// 系统模块函数
#define MOD(n, f) }else if(modname==#n){ return Sysmod##f(name, para);
        if(modname==""){
        MOD(fs, Fs)                 // 文件系统
        MOD(path, Path)             // 路径处理
        }
#undef MOD


// 原生对象函数
#define ELSE(N) }else if(bt==OT::N){ return Objfunc##N(((Object##N*)base)->value, name, para);
    ELSE(String)  // 字符串
    ELSE(Node)    // 节点
#undef ELSE

    }

    return NULL;
}

