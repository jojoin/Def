/**
 * 对语法节点进行求值操作
 */
DO* Exec::Evaluat(Node* n)
{
	LOCALIZE_module;
	LOCALIZE_gc;
	LOCALIZE_stack;

    if(n==NULL) return NULL;

#define T NodeType

    T t = n->type; //当前节点类型

    if(t==T::Group)
    { // 语句组（if或while的body）
        NodeGroup *gr = (NodeGroup*)n;
    	DO* last = NULL;
	    size_t i = 0
	         , s = gr->ChildSize();
	    while(i<s){
	        last = Evaluat( gr->Child(i) );
	        i++;
	    }
	    return last; //返回最后一条语句的值

    }
    else if(t==T::Variable)
    {
        //cout<<"Variable !!!"<<endl;
        string name = n->GetName();
        DO* val = Variable(name);
        if(!val){ // 变量不存在
        	ERR("Can't find variable : "+name+" !");
        }
        return val;

    }
    else if(t==T::Priority)
    { // 优先级
        // cout<<"priority!!!"<<endl;
        return Evaluat( n->Child() ); //递归求值

#define IF(kind) }else if(t==T::kind){ return kind(n);
        IF(Assign)
        IF(AssignUp)
        IF(ProcDefine)
        IF(FuncDefine)
        IF(ProcCall)
        IF(FuncCall)
        IF(Return)
        IF(Continue)
        IF(Break)
        IF(List)
        IF(Dict)
        IF(Block)
        IF(Print)
        IF(If)
        IF(While)
        IF(For)
        IF(Not)
        IF(ContainerAccess)
        IF(MemberAccess)
        IF(Import)
        IF(Try)
        IF(Throw)
#undef IF

    }else if(t==T::Add||t==T::Sub||t==T::Mul||t==T::Div){ // + - * / 算法操作
        //cout<<"operate !!!"<<endl;
        return Operate( n->Left(), n->Right(), t);

    }else if(t==T::Equal||t==T::More||t==T::Less||t==T::MoreEqual||t==T::LessEqual||t==T::NotEqual){ // 比较操作
        //cout<<"compare !!!"<<endl;
        return Compare( n->Left(), n->Right(), t);

    }else if(t==T::And||t==T::Or){ // 组合判断
        //cout<<"judge !!!"<<endl;
        return Judge( n->Left(), n->Right(), t);

    }else if(t==T::None||t==T::Bool||t==T::Int||t==T::Float||t==T::String){ // none bool int 字面量求值
        // cout<<"Allot !!!"<<endl;
        return _gc->Allot(n);
        
    }else{

    	// 未定义的操作
    	ERR("No Evaluat Match !");

    }

#undef T   // NodeType

}

