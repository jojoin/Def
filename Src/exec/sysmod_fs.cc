/**
 * 系统模块 fs
 */
DO* Exec::SysmodFs(string func, Node* para)
{
    // cout<<"-Exec::SysmodFs-"<<endl;
    LOCALIZE_gc
    NodeGroup* argv = (NodeGroup*) para;
    size_t len = argv->ChildSize();

    // 当前文件所在文件
    string path = Path::getDir(_envir._file);
    // cout<<"Path::getDir(_envir._file) = "<<path<<endl;
    string p1 = ""; // 第一个参数
    if(len>0){
        DefObject* o1 = Evaluat( para->Child(0) );
        if(o1->type==OT::String){
            p1 = ((ObjectString*)o1)->value;
        }
    }
    string tarfile = Path::join(path, p1);





    // 判断文件或目录是否存在
    if(func=="exist"){
        // cout<<tarfile<<endl;
        return Fs::Exist(tarfile) || Fs::ExistDir(tarfile)
            ? ObjTrue() 
            : ObjFalse();




    // 读取文件
    }else if(func=="read"){
        if(!Fs::Exist(tarfile)){
            ERR("\""+tarfile+"\" is not find !")
        }
        // 读取文件
        return _gc->AllotString( Fs::ReadFile(tarfile) );




    // 写入文件
    }else if(func=="write"){
        if(len>=2){
            DefObject* ostr = Evaluat( para->Child(1) );
            if(ostr->type==OT::String){
                // 写入文件
                if( 
                    Fs::WriteFile(tarfile, ((ObjectString*)ostr)->value)
                ){
                    return ObjTrue(); // 写入成功
                }
            }
        }
        return ObjFalse(); // 失败




    // 获取目录下的所有文件名称列表
    }else if(func=="getfiles"||func=="getdirs"||func=="getchilds"){
        // cout<<"-getfiles-"<<endl;
        vector<string> files;
        int tp = 0; // 获取内容
        if(func=="getdirs") tp = 1;
        else if(func=="getchilds") tp = 2;
        // 获取
        Fs::GetChilds(tarfile, files, tp);
        ObjectList *li = _gc->AllotList();
        vector<string>::iterator it;
        for(it=files.begin(); it!=files.end(); ++it)
            li->Push( _gc->AllotString(*it) ); // 遍历加入
        return li;


    // 获取目录下的所有目录名称列表
    }else if(func=="getdirs"){




    }

}

