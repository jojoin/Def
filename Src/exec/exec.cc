

/**
 * 完全构造
 */
Exec::Exec(void)
{
    _envir = Envir(); // 新建执行环境
}

/**
 * 拷贝构造执行环境
 */
Exec::Exec(Envir e){
	_envir = Envir(e);
}


// 执行栈变量初始化
void Exec::StackPush(string name, DO* obj)
{
	_envir._stack->VarPut(name, obj);
}



// 指定&获取父栈
Stack* Exec::StackParent(Stack*p)
{
	return _envir._stack->parent = p;
}       




/**
 * 从入口文件开始执行
 */
bool Exec::Main(string fl)
{
	string file = Path::join(Path::cwd(), fl);

	if(!Fs::Exist(file)){
		ERR("File \""+file+"\" is not find !");
	}

	string text = Fs::ReadFile(file);
	// cout<<file<<endl;
	_envir.SetFile(file); // 入口文件

	Node *nd = Parse(text, file); // 解析语法
	// nd->Print();
    // return false; 
	_envir.Set(nd);       // 设置环境

    // 解释执行
    Run();

    // 清除环境数据
    _envir.Clear();

    return true;

}


/**
 * 解析得到抽象语法树
 */
Node* Exec::Parse(string &text, string file)
{

    // 词法分析结果
    Tokenizer T(text); // 初始化词法分析器
    T.SetFile(file);
    vector<Word>* words = T.Scan(); // 执行词法分析

    // cout << words->at(0).value << endl;
    // cout << words->at(1).value << endl;
    // cout << words->at(2).value << endl;

    // 语法分析
    Nodezer N(words); // 初始化语法分析器
    N.SetFile(file);
    Node* node = N.BuildAST(); // 解析得到语法树（表达式）

    // cout << node->Child(0)->GetName() << endl;
    // cout << node->Right()->Child(1)->Left()->GetName() << endl;

    // node->Print();
    // cout<<endl;
    // cout << "node->ChildSize() = " << node->ChildSize() << endl;

    words->clear();
    delete words; // 析构words数组

    return node; // 返回语法树
}



/**
 * 执行代码
 */
DO* Exec::Eval(string code)
{
    code += "      ";
    string file = _envir._file;
    // 词法+语法分析
    Node* nd = Parse(code, file);
    // nd->Print();
    // return NULL;
    // 执行语句
    _envir.SetFile(file); // 入口文件
    _envir.Set(nd);       // 设置环境
    // 解释执行
    // Run();
    NodeGroup *gr = (NodeGroup*)nd;
    DO* res = Evaluat( gr->Child(0) );
    // 清除环境数据
    // _envir.Clear();

    // 返回
    return res;
}


/**
 * 执行 Def 调用帧
 * @return 调用的返回对象
 */
DO* Exec::Run()
{
    LOCALIZE_node;
    LOCALIZE_gc;

    DO *ret = ObjNone();

    if(!_node){
    	return ret;
    }


    // 组合表达式 NodeType::Group
    size_t i = 0
         , s = _node->ChildSize();
    while(i<s){
        ret = Evaluat( _node->Child(i) );
        i++;
    }

    _gc->Quote(ret); //返回值加引用
    return ret;
}





/**
 * 执行当前栈帧的垃圾回收
 * 通常在一句表达式执行完毕后调用
 */
inline void Exec::VarPut(string name, DO*obj)
{
    LOCALIZE_gc;
    LOCALIZE_stack;

    // 入栈
    DO *exi = _stack->VarPut(name, obj); // 查找变量是否存在
    if(exi){
        // cout<<"_gc->Free()"<<endl;
        _gc->Free(exi);       // 解引用
    }
    // cout<<"_gc->Quote()"<<name<<endl;
    _gc->Quote(obj); // 加引用
}


//返回对象
inline ObjectNone* Exec::ObjNone()
{
    return _envir._gc->prep_none;
}
inline ObjectBool* Exec::ObjTrue()
{
    return _envir._gc->prep_true;
}
inline ObjectBool* Exec::ObjFalse()
{
    return _envir._gc->prep_false;
}

