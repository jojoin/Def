/**
 * def 处理器调用
 */
DO* Exec::ProcCall(Node* n)
{
    LOCALIZE_gc
    // cout<<"ProcCall !!!"<<endl;
    NodeProcCall *p = (NodeProcCall*)n;
    // 获得处理器对象
    ObjectProc *op = (ObjectProc*)Evaluat( n->Left() );
    if(!op){
        ERR("Can't get the proc object !")
    }
    NodeProcDefine *proc = (NodeProcDefine*)op->GetNode();
    NodeGroup *pbody = (NodeGroup*)proc->GetBody();
    if(!pbody || !pbody->ChildSize()){
        return ObjNil(); //处理器体为空
    }
    // fbody->Print();
    // 拷贝环境
    Envir proc_envir = *(Envir*)op->GetEnvir();
    Envir env = Envir(proc_envir);
    // 新栈帧（父级栈帧）
    Stack *stack = new Stack();
    stack->SetParent( proc_envir._stack );
    // 混合生成处理器参数
    BuildProcArgv(proc->GetArgv(), p->Right(), stack);
    // 调用执行环境对象入栈
    ObjectExec *oe = new ObjectExec(this);
    stack->VarPut("_call_", oe);
    // 环境更新
    env.Set(EnvirType::Proc);
    env.Set(stack);
    env.Set(pbody);
    // stack->Print();

    // 环境准备完毕，开始函数调用执行
    Exec exec = Exec(env);
    // 执行调用
    DO * retval = NULL;
    try
    {
        retval = exec.Run();
    }
    catch(Abnor* tr) // 处理器返回
    {
        if(tr->GetType()==AbnorType::Return){
            retval = tr->GetObject(); // 返回值
            delete tr;
            // ERR("Function run excepction not <Return> !");
        }else{
            throw tr;
        }
    }
    // 处理器调用完成，清理执行栈
    map<string, DO*>::iterator itr_s = stack->v_local.begin();
    for(; itr_s != stack->v_local.end(); ++itr_s){
        _gc->Free( itr_s->second );
    }
    delete stack; //清除栈

    // 返回结果
    return retval;
}



/**
 * 匹配处理器参数列表
 */
void Exec::BuildProcArgv(Node*form, Node*real, Stack*stack)
{
    LOCALIZE_gc

    size_t num_f = form ? form->ChildSize() : 0;
    size_t num_r = real ? real->ChildSize() : 0;
    size_t num_max = num_f > num_r ? num_f : num_r;
    // 参数列表
    ObjectList *argv = new ObjectList();
    //循环匹配参数
    for(int i=0; i<num_max; i++){
        DO* v = NULL;
        // 取值得到实参
        if(i<num_r){
            v = new ObjectNode( real->Child(i) );
            _gc->Quote( v ); // 加引用
            argv->Push( v );
        }
        if(!v){
            v = ObjNil(); // 无匹配 Nil
        }
        // 匹配形参并入栈
        if(i<num_f){
            Node *f = form->Child(i);
            if(f->type!=NT::Variable){
                ERR("Proc form parameter must be <Variable> type !")
            }
            string name = f->GetName();
            stack->VarPut(name, v);
            _gc->Quote( v ); // 加引用
        }
    }
    // 所有实参列表！
    stack->VarPut("_argv_", argv);

}



