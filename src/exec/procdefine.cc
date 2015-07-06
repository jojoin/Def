

/**
 * def 处理器定义
 */
DO* Exec::ProcDefine(Node* n)
{
    // cout<<"ProcDefine !!!"<<endl;
    NodeProcDefine* p = (NodeProcDefine*)n;

    ObjectProc* proc = new ObjectProc(p); //新建对象
    proc->envir = (void*) &_envir; // 定义所在栈帧环境

    string name = p->GetName();
    if(name!=""){ // 变量入栈
        // cout<<"VarPut(name, proc);"<<endl;
        VarPut(name, proc);
    }
    return proc;
}

