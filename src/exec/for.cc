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

    // 整数
    if(ct==OT::Int){
        // cout<<"for: Int"<<endl;
        ObjectInt *num = (ObjectInt*)con;
        ObjectInt *stp = (ObjectInt*)Evaluat( k ); // 步进
        size_t ii = 1;
        while(ii<=num->value){
            Assign( v, _gc->AllotInt(ii) );
            for(int j=3; j<len; ++j)
            {   // 执行遍历体
                res = Evaluat( p->Child(j) ); 
            }
            ii += stp->value; //一步
        }


    // 字符串
    }else if(ct==OT::String){
        // cout<<"for: String"<<endl;
        ObjectString *str = (ObjectString*)con;
        ObjectString *spl = (ObjectString*)Evaluat( k ); // 分割
        if(spl->value==""){ // 遍历字符串
            size_t sz = str->value.size();
            for(int i=0; i<sz; ++i){
                Assign( v, _gc->AllotString( str->value.substr(i,1) ) );
                for(int j=3; j<len; ++j)
                {   // 执行遍历体
                    res = Evaluat( p->Child(j) ); 
                }
            }
        }else{ // 分割字符串
            vector<string> strs;
            Str::split(str->value, spl->value, strs);
            size_t sz = strs.size();
            for(int i=0; i<sz; ++i){
                Assign( v, _gc->AllotString(strs[i]) );
                for(int j=3; j<len; ++j)
                {   // 执行遍历体
                    res = Evaluat( p->Child(j) ); 
                }
            }
        }
        
    // 列表 块
    }else if(ct==OT::List || ct==OT::Block){
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
