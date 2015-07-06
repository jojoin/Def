
/**
 * def 函数定义
 */
DO* Exec::FuncDefine(Node* n)
{
    LOCALIZE_gc
    // cout<<"FuncDefine !!!"<<endl;
    NodeFuncDefine* p = (NodeFuncDefine*)n;
    // 新建函数对象
    ObjectFunc* func = _gc->AllotFunc( p ); 
    // 处理默认参数
    ObjectDict* para = _gc->AllotDict(); 
    NodeGroup* pg =  (NodeGroup*)p->GetArgv();
    size_t len = pg->ChildSize();
    for(int i=0; i<len; i++){
        Node* li = pg->Child(i);
        if(li->type==NT::Assign){
            Node *nl = li->Left();
            if(nl->type==NT::Variable){
                DO* pv = Evaluat( li->Right() );
                para->Set( // 添加默认参数
                    nl->GetName(),
                    pv
                );
                _gc->Quote(pv); // 加引用
                // cout<<"default func parameter : "<<nl->GetName()<<endl;
                continue;
            } 
        }
        if(li->type!=NT::Variable){ // 参数格式错误
            ERR("Function parameter list format error !")
        }
        // cout<<"func parameter"<<endl;
    }
    func->argv = para; // 默认参数赋值
    func->envir = (void*)& _envir; // 定义所在栈帧环境
    // 变量入栈
    // cout<<"string name = p->GetName();"<<endl;
    string name = p->GetName();
    if(name!=""){
        VarPut(name, func);
    }
    return func;
}


