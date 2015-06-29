
/**
 * def 函数调用
 */
DO* Exec::FuncCall(Node* n)
{
    LOCALIZE_gc
    // cout<<"FuncCall !!!"<<endl;
    NodeFuncCall *p = (NodeFuncCall*)n;
    Node *lf = n->Left();
    Node *rt = p->Right();

    ObjectFunc *of;

    //调用变量自带函数
    DO *ofres = Objfunc(lf, rt);
    if(ofres){
        return ofres; // 调用成功
    }

    if(NT::Variable==lf->type){

        string name = lf->GetName();
        DO *obj = Variable(name);
        if(!obj){ // 未找到自定义变量，尝试调用系统函数
            DO *res = Sysfunc( name, rt );
            if(res){
                return res; //系统函数调用成功
            }
            ERR("Can't find the function \""+name+"\" !")
        }
        of = (ObjectFunc*)obj;

    }else{
        of = (ObjectFunc*)Evaluat( lf );
    }
    // 获得函数对象
    NodeFuncDefine *func = (NodeFuncDefine*)of->GetNode();
    NodeGroup *fbody = (NodeGroup*)func->GetBody();
    if(!fbody || !fbody->ChildSize()){
        return ObjNone(); //函数体为空
    }
    // fbody->Print();
    // 拷贝环境
    Envir env = Envir(_envir);
    // 新栈帧（父级栈帧）
    Stack *stack = new Stack();
    stack->SetParent( (Stack*)of->GetStack() );
    // 处理默认参数
    ObjectDict* para = of->argv;
    // cout<<"*para="<<(int)para<<endl;
    map<string, DO*>::iterator itr_p = para->value.begin();
    for(; itr_p != para->value.end(); ++itr_p){
        stack->VarPut( itr_p->first, itr_p->second );
        _gc->Quote(itr_p->second); // 加引用
    }
    // 混合生成处理器参数
    // cout<<"func->GetArgv();"<<endl;
    Node* ppp = func->GetArgv();
    // cout<<"Node* ppp="<<(int)ppp<<endl;
    BuildFuncArgv(func->GetArgv(), p->Right(), stack);
    // cout<<"BuildFuncArgv stack"<<endl;
    // stack->Print();
    // 环境更新
    env.Set(EnvirType::Func);
    env.Set(stack);
    env.Set(fbody);
    // stack->Print();

    // 环境准备完毕，开始函数调用执行
    Exec exec = Exec(env);
    // 执行调用
    DO * retval = NULL;
    try
    {
        retval = exec.Run();
    }
    catch(Throw* tr) // 函数返回
    {
        if(tr->GetType()!=ThrowType::Return){
            ERR("Function run excepction not <Return> !");
        }
        retval = tr->GetObject(); // 返回值
        delete tr;
    }
    // cout<<"bool done = exec.Run();"<<endl;
    // stack->Print();
    // 函数调用完成，清理执行栈
    map<string, DO*>::iterator itr_s = stack->v_local.begin();
    for(; itr_s != stack->v_local.end(); ++itr_s){
        _gc->Free( itr_s->second );
    }
    delete stack; //清除栈
    
    // 返回函数执行结果
    return retval;
}

/**
 * 匹配函数参数列表
 */
void Exec::BuildFuncArgv(Node*form, Node*real, Stack*stack)
{
    LOCALIZE_gc

    // cout<<"BuildFuncArgv !!!"<<endl;
    size_t num_f = form ? form->ChildSize() : 0;
    size_t num_r = real ? real->ChildSize() : 0;
    size_t num_max = num_f > num_r ? num_f : num_r;
    // cout<<"num max="<<num_max<<", f="<<num_f<<", r="<<num_r<<endl;
    // 参数列表
    ObjectList *argv = _gc->AllotList();
    map<string, DO*> keypara; // 关键字参数
    //循环匹配参数
    for(int i=0; i<num_max; i++)
    {
        string name = ""; // 形式参数名
        DO* v = NULL; // 参数值
        bool iskp = false; // 是否为关键字参数
        // 取实参
        if(i<num_r){
            Node * li = real->Child(i);
            // 判断是否关键字参数
            if(li->type==NT::Assign){ 
                Node *n = li->Left();
                if(n->type==NT::Variable){
                    v = Evaluat( li->Right() );
                    keypara[n->GetName()] = v; //缓存关键字参数
                    iskp = true;
                }
            }
            if(!v){
                v = Evaluat( li );
            }
            argv->Push( v ); // 实参列表
            _gc->Quote(v);
        }
        // 取得形式参数名称
        if(i<num_f){ 
            Node *f = form->Child(i);
            if(f->type==NT::Assign){ //默认参数
                Node *nl = f->Left();
                if(nl->type==NT::Variable){
                    name = nl->GetName();
                } 
            }
            if(name==""){
                if(f->type!=NT::Variable){
                    ERR("Func form argv must be <Variable> type !")
                }
                name = f->GetName();
            }
        }
        // 形式参数匹配
        if(name!=""){
            DO *dft = stack->VarGet(name);
            if( iskp && !dft){ 
                //关键字参数项 没有默认值
                stack->VarPut(name, ObjNone());
            }
            // 正常匹配项
            if(!iskp && v){
                _gc->Quote(v); // 加引用
                stack->VarPut(name, v);
                if(dft) _gc->Free(dft);
            }
        }
    }
    // 关键字参数覆盖
    map<string, DO*>::iterator itr = keypara.begin();
    for(; itr != keypara.end(); ++itr){
        DO*old = stack->VarPut(itr->first, itr->second);
        if(old){
            _gc->Free(old); // 存在值则覆盖
        }
    }
    // 所有实参列表！
    stack->VarPut("_argv_", argv);

}
