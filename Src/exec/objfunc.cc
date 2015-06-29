




/**
 * 调用变量自带函数
 * @name 函数名
 * @argv 参数列表
 */
DO* Exec::Objfunc(Node* left, Node* para)
{
    // 检测是否为变量自带函数调用
    if(NT::MemberAccess==left->type){

        DO *base = Evaluat( left->Left() );
        string name = left->Right()->GetName();
        return Objfunc(base, name, para);
    }

    return NULL;
}
DO* Exec::Objfunc(DO* base, string name, Node* para)
{
    // cout<<"-Exec::Objfunc-"<<endl;
    LOCALIZE_gc

    NodeGroup* argv = (NodeGroup*) para;
    size_t len = argv->ChildSize();

    OT bt = base->type;

    // 打印
    if(name=="print"){

        DO::Print( base );
        cout<<endl;
        return base;

    // 求类型
    }else if(name=="type"){


    }

    // 系统模块
    if(bt==OT::Sysmod){
        ObjectSysmod *mod = (ObjectSysmod*)base;
        string modname = mod->GetName();
        // 文件模块
        if(modname=="fs"){
            // 当前文件所在文件
            string path = Path::getDir(_envir._file);
            cout<<"Path::getDir(_envir._file) = "<<path<<endl;
            string p1 = ""; // 第一个参数
            if(len>0){
                DefObject* o1 = Evaluat( para->Child(0) );
                if(o1->type==OT::String){
                    p1 = ((ObjectString*)o1)->value;
                }
            }
            // 读取文件
            if(name=="read"){
                string tarfile = Path::join(path, p1);
                if(!Fs::Exist(tarfile)){
                    ERR("\""+tarfile+"\" is not find !")
                }
                // 读取文件
                return new ObjectString( Fs::ReadFile(tarfile) );
            }
        }


    }



    return NULL;
}

