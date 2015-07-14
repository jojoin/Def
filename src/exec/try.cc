/**
 * 组合条件判断
 */
DO* Exec::Try(Node *n)
{
    LOCALIZE_gc
    // cout<<"Try !!!"<<endl;
    NodeTry *p = (NodeTry*)n;
    DO* res = ObjNone(); // 结果
    bool iscatch = false;
    // 试运行 try
    if(p->run){
        try{
            res = Evaluat( p->run ); //执行 try 块
        }
        catch(Abnor* tr) // 捕获
        {
            AbnorType t = tr->GetType();
            if(t==AbnorType::Throw){
                iscatch = true;
                if(p->cat){
                    NodeGroup *cat = (NodeGroup*)p->cat;
                    size_t len = cat->ChildSize();
                    if(len>0){
                        Assign(cat->Child(0), tr->GetObject()); // 赋值
                        for(int i=1; i<len; ++i){ // 执行 catch 块
                            res = Evaluat( cat->Child(i) );
                        }
                    }
                }

            }else{ // 其它异常
                throw tr;
            }
        }
    }
    // 执行 else 块
    if(!iscatch && p->els){
        res = Evaluat( p->els );
    }

    // 返回结果
    return res;
}