/**
 * While 控制结构
 */
DO* Exec::While(Node* n)
{
    NodeWhile* p = (NodeWhile*)n;
    DO* res = ObjNone(); // 结果
    size_t len = p->ChildSize();
    if(!len){
        return res;
    }
    Node* cond = p->Child(0);
    while(1){
        if(!Conversion::Bool( Evaluat( cond ) )){
            break; // 条件假，跳出循环
        }
        for(int i=1; i<len; i++){
            res = Evaluat( p->Child(i) ); //执行 while 块
        }
    }
    return res; 
}
