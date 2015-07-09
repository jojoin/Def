/**
 * 对象函数 node
 */
DO* Exec::ObjfuncNode(Node* base, string func, Node* para)
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

    // 获取节点类型名称
    if(func=="type"){
        // cout<<"node obj func <type>"<<endl;
        string tp = Node::GetTypeName(base);
        transform(tp.begin(), tp.end(), tp.begin(), ::tolower); // ::toupper 转大写
        return _gc->AllotString( tp );
        
    }else if(func=="name"){
        string nn = "";
        NT t = base->type;
        if(t==NT::Variable){
            nn = ((NodeVariable*)base)->GetName();
        }
        return _gc->AllotString( nn );
    }

}

