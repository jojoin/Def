/**
 * While 控制结构
 */
DO* Exec::While(Node* n)
{
    NodeWhile* p = (NodeWhile*)n;
    DO* res = ObjNil(); // 结果
    size_t len = p->ChildSize();
    if(!len){
        return res;
    }
    Node* cond = p->Child(0);
    while(1){
        if(!Conversion::Bool( Evaluat( cond ) )){
            break; // 条件假，跳出循环
        }
        // 捕获 content 或 break
        try{
            for(int i=1; i<len; i++){
                res = Evaluat( p->Child(i) ); //执行 while 块
            }
        }
        catch(Abnor* tr) // 函数返回
        {
            AbnorType t = tr->GetType();
            if(t==AbnorType::Continue){ // 循环继续
                // do nothing
                delete tr;
            }else if(t==AbnorType::Break){ // 循环退出
                break;
                delete tr;
            }else{ // 其它异常
                throw tr;
            }
        }
    }
    return res; 
}
