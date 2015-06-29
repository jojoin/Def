
/**
 * If 控制结构
 */
DO* Exec::If(Node* n)
{
    NodeIf* p = (NodeIf*)n;
    DO* ret = ObjNone();
    size_t i = 0
         , s = p->ChildSize();
    while(i<s){
        NodeGroup *li = (NodeGroup*)p->Child(i);
        size_t n = li->ChildSize();
        if(n){
            Node *cnd = li->Child(0); //条件（值NULL则为else块）
            if(!cnd || Conversion::Bool( Evaluat( cnd ) )){
                for(int j=1; j<n; j++)
                {
                    ret = Evaluat( li->Child(j) ); //执行 if 或 else 块
                }
                break; // if 完成
            }
        }
        i++;
    }
    return ret;
}

