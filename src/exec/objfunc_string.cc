/**
 * 对象函数 string
 */
DO* Exec::ObjfuncString(string base, string func, Node* para)
{
    // cout<<"-Exec::ObjfuncString-"<<endl;
    LOCALIZE_gc
    NodeGroup* argv = (NodeGroup*) para;
    size_t len = argv->ChildSize();

    // 当前文件所在文件
    string path = Path::getDir(_envir._file);
    // cout<<"Path::getDir(_envir._file) = "<<path<<endl;
    DefObject* o1 = NULL;
    DefObject* o2 = NULL;
    if(len>0){
        o1 = Evaluat( para->Child(0) );
    }
    if(len>1){
        o2 = Evaluat( para->Child(1) );
    }

    // 取单个字符
    if(func=="at"){
        // cout<<"string obj func <at>"<<endl;
        if(o1 && o1->type==OT::Int){
            int idx = ((ObjectInt*)o1)->value - 1; // 索引从1开始
            if( idx>=0 && idx < base.size() ){
                return _gc->AllotString( base.substr(idx,1) );
            }
        }
        return ObjNone();


    // 截取
    }else if(func=="substr"){

    // 分割
    }else if(func=="split"){

        // cout<<"string obj func <split>"<<endl;
        if(o1 && o1->type==OT::String){
            string st = ((ObjectString*)o1)->value;
            size_t stsz = st.size();
            int sta = 0;
            ObjectList *res = _gc->AllotList();
            if(st==""){
                size_t bsz = base.size();
                for(int i=0; i<bsz; ++i){
                    res->Push( _gc->AllotString( base.substr(i, 1) ) );
                }
            }else{
                while(1){
                    size_t found = base.find(st, sta);  
                    if(found==string::npos){
                        res->Push( _gc->AllotString( base.substr(sta) ) );
                        break;
                    }
                    res->Push( _gc->AllotString( base.substr(sta, found-sta) ) );
                    sta = found + stsz; // 更新查找位置
                }
            }
            return res; //返回分割的字符串
        }
        return ObjNone();
        
    // 替换
    }else if(func=="replace"){
        string nstr = base;
        if( o1 && o2 && o1->type==OT::String && o2->type==OT::String ){
            Str::replace_all(nstr, ((ObjectString*)o1)->value, ((ObjectString*)o2)->value);
        }
        return _gc->AllotString( nstr );
    }

}

