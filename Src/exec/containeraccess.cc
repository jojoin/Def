/**
 * ContainerAccess 容器访问
 */
DO* Exec::ContainerAccess(Node* n)
{
    // cout<<"-Exec::ContainerAccess-"<<endl;
    NodeContainerAccess* p = (NodeContainerAccess*)n;

    Node* con = p->Left();
    Node* idx = p->Right();
    size_t idxlen = idx->ChildSize();

    DO* result = NULL; //容器访问结果

    // cout<<"idxlen="<<idxlen<<endl;
    if(idxlen){ //索引个数

        DO* obj = Evaluat( con );
        OT ot = obj->type;
        DO* i1 = Evaluat( idx->Child(0) );
        OT i1t = i1->type;

        // 字典访问
        if(ot==OT::Dict){
            if(i1t!=OT::String){
                ERR("Dict key only <string> type !");
            }
            string key = Conversion::String( i1 );
            // cout<<"key = "<<key<<endl;
            result = ((ObjectDict*)obj)->Visit(key); 

        // 列表访问
        }else if(ot==OT::List){
            if(i1t!=OT::Int){
                ERR("List index only <int> type !");
            }
            if(idxlen==1){ // 单个获取
                size_t i = Conversion::Long( i1 );
                if(!i){
                    ERR("List index begin from <int> 1 !");
                }
                result = ((ObjectList*)obj)->Visit(i-1);
            }
        // 块访问
        }else if(ot==OT::Block){
            if(i1t!=OT::Int){
                ERR("Block index only <int> type !");
            }
            if(idxlen==1){ // 单个获取
                size_t i = Conversion::Long( i1 );
                if(!i){
                    ERR("Block index begin from <int> 1 !");
                }
                result = ((ObjectBlock*)obj)->Visit(i-1);
            }
        }
    }

    // cout<<"return result="<<(int)result<<endl;
    return result ? result : ObjNone(); // 无效访问 返回 none
}