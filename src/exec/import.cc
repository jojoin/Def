/**
 * Import 模块加载
 */
DO* Exec::Import(Node* n)
{
    LOCALIZE_module
    LOCALIZE_stack
    LOCALIZE_gc

    // cout<<"-Exec::Import-"<<endl;
    DO* str = Evaluat( n->Child() );
    string name = Conversion::String( str );


    DO* mod; // 模块

    if(Module::IsSysmodName(name)){
        // 新建系统模块
        mod = new ObjectSysmod(name);
    }else{
        // 解析路径加载用户模块
        mod = Import( name );
    }

#ifdef WINDOWS
    // 替换字符
    // cout<<"ifdef WINDOWS : "<<name<<endl;
    Str::replace_all(name,"/","\\");
    // cout<<"endif WINDOWS : "<<name<<endl;
#endif

    if(mod){ //自动入栈
        string vn = Path::getName(name);
        if(Token::IsVariable(vn)){
            _stack->VarPut(vn, mod);
            _gc->Quote(mod);
        }
    }

    return mod;
}



/**
 * Import 模块加载
 */
DO* Exec::Import(string mdname)
{
	LOCALIZE_module

	// 加载模块
	ObjectModule* md;

    // 获得模块绝对路径
    string tarfile = Path::join( 
        Path::getDir(_envir._file),
        mdname
    );
    // cout<<"module path: "<<tarfile<<endl;

	string mdfile = Module::MatchFile(tarfile);
	if(mdfile==""){
		ERR("Can't find module\""+mdname+"\" !");
	}

	md = _module->GetCache( mdfile );
	if(md) return md; // 返回已经加载过的缓存的模块
	
	// 创建
	md = CreateModule(mdfile);

	if(!md){
		ERR("Can't create module\""+mdname+"\" !");
	}

	// 设置缓存
	_module->SetCache(mdfile, md);

	return md;
}

/**
 * 模块创建
 */
ObjectModule* Exec::CreateModule(string file)
{
    // cout<<"-Exec::CreateModule-"<<file<<endl;
	// 拷贝环境
	Envir le = Envir(_envir);

	le.SetFile( file );
	le.Set( EnvirType::Module );

	string text = Fs::ReadFile(file);
	Node *nd = Parse(text, file);
	// nd->Print();
    // return NULL; 
	le.Set( nd ); // 解析语法

	Stack *stack = new Stack(); // 新栈帧
	le.Set( stack );

    // cout<<"Exec nex = Exec(le);"<<endl;
	// 新建调用
	Exec nex = Exec(le);

    // cout<<"bool done = nex.Run();"<<endl;
    // 执行模块调用
    bool done = nex.Run();

    if(!done){
    	return NULL; // 执行失败
    }

    // cout<<"stack->Print();"<<endl;
    // stack->Print();

    // cout<<"ObjectModule *om = new ObjectModule();"<<endl;
    // 生成模块对象
    ObjectModule *om = new ObjectModule();
    map<string, DO*>::iterator itr = stack->v_local.begin();
    for(; itr != stack->v_local.end(); ++itr){
        om->Insert( itr->first, itr->second );
    }

    // cout<<"CreateModule!!!  = "<<om<<endl;
    return om;
}
