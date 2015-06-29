/**
 * For 遍历
 */
DO* Exec::For(Node* n)
{
    LOCALIZE_gc;
    // cout<<"For !!!"<<endl;
    NodeFor* p = (NodeFor*)n;
    DO* res = ObjNone(); // 结果
    size_t len = p->ChildSize();
    // cout<<"len="<<len<<endl;
    if(len<3){
        return res;
    }
    DefObject* con = Evaluat( p->Child(0) );
    OT ct = con->type;
    Node* k = p->Child(1);
    Node* v = p->Child(2);

    // 列表 块
    if(ct==OT::List || ct==OT::Block){
        // cout<<"for: List Block"<<endl;
        ObjectExArr *arr = (ObjectExArr*)con;
        size_t sz = arr->Size();
        // cout<<"sz="<<sz<<endl;
        for(int i=0; i<sz; ++i)
        {
            Assign(k, _gc->AllotInt(i+1)); // 下标从1开始
            Assign(v, arr->Visit(i));
            for(int j=3; j<len; ++j)
            {   // 执行遍历体
                res = Evaluat( p->Child(j) ); 
            }
        }

    // 字典 模块
    }else if(ct==OT::Dict || ct==OT::Module){
        // cout<<"for: Dict Module"<<endl;
        ObjectExPkg *pkg = (ObjectExPkg*)con;
        map<string, DO*>::iterator itr = pkg->value.begin();
        for(; itr != pkg->value.end(); ++itr){
            // om->Insert( itr->first, itr->second );
            Assign(k, _gc->AllotString(itr->first)); // 下标从1开始
            Assign(v, itr->second);
            for(int j=3; j<len; ++j)
            {   // 执行遍历体
                res = Evaluat( p->Child(j) ); 
            }
        }

    }

    return res; 
}
