
/**
 * 赋值操作
 */
DO* Exec::Assign(Node*n)
{

    Node* nl = n->Left();
    DO* rv = Evaluat( n->Right() );   // 等号右值

    return Assign(nl, rv);
}
// 赋值
DO* Exec::Assign(Node*nl, DO*rv)
{
    LOCALIZE_gc;
    LOCALIZE_stack;

    NT nt = nl->type;
    // 普通变量赋值
    if(nt==NT::Variable){
        // cout<<"Assign name="<<name<<endl;
        string name = nl->GetName();     // 名字
        DO *exi = _stack->VarGet(name);   // 查找变量是否存在
        if(exi){
            // cout<<"_gc->Free()"<<endl;
            _gc->Free(exi);       // 解引用
        }
        // cout<<"_stack->VarPut()"<<name<<endl;
        _stack->VarPut(name, rv);   // 入栈

    // 成员访问赋值
    }else if(nt==NT::MemberAccess){
        // cout<<"MemberAccess Assign name="<<nl->Right()->GetName()<<endl;
        ObjectModule *mod = (ObjectModule*)Evaluat( nl->Left() );
        string member = nl->Right()->GetName();
        DO *exi = mod->Visit(member);
        if(exi){ //已存在，解引用
            _gc->Free(exi);
            mod->Set(member, rv);
        }else{
            mod->Insert(member, rv); // 设置成员
        }

    // 容器访问赋值
    }else if(nt==NT::ContainerAccess){
        // cout<<"ContainerAccess Assign name="<<nl->Right()->GetName()<<endl;
        DO *con = Evaluat( nl->Left() ); // 得到容器
        OT ct = con->type; // 容器类型
        Node *idx = nl->Right(); // 索引
        size_t idx_sz = idx ? idx->ChildSize() : 0;
        // cout<<"idx_sz = "<<idx_sz<<endl;
        // 字典
        if(ct==OT::Dict){
            // cout<<"ct==OT::Dict"<<endl;
            ObjectDict *dict = (ObjectDict*)con;
            if(!idx_sz){
                ERR("Dict Need <string> type key on Assign !");
            }
            // cout<<"*ik = Evaluat("<<endl;
            DO *ik = Evaluat( idx->Child(0) );
            if(ik->type!=OT::String){ // 验证 key 类型
                ERR("Dict key only <string> type on Assign !");
            }
            // cout<<"Conversion::String("<<endl;
            string key = Conversion::String( ik );
            DO *exi = dict->Visit(key);
            // cout<<"exi = "<<exi<<endl;
            if(exi){ //已存在，解引用
                _gc->Free(exi);
                dict->Set(key, rv);
            }else{
                dict->Insert(key, rv); // 设置成员
            }
            // cout<<"dict Assign"<<endl;

        // 列表
        }else if(ct==OT::List){
            ObjectList *list = (ObjectList*)con;
            if(!idx_sz){ // 添加到末尾
                list->Push(rv);
            }else{ // 替换制定位置
                DO *oi = Evaluat( idx->Child(0) );
                if(oi->type!=OT::Int){
                    ERR("List index only <int> type on Assign !");
                    return rv; // do nothing
                }
                size_t i = Conversion::Long( oi );
                if(!i){
                    ERR("List index begin from <int> 1 !");
                    return rv; // do nothing
                }
                DO *exi = list->Visit(i-1); //索引从1开始
                if(exi){ //已存在，解引用
                    _gc->Free(exi);
                }
                list->Push(i-1, rv); // 添加到指定位置

            }
        }
    }

    _gc->Quote(rv); // 加引用
    return rv; // 返回右值

}

/**
 * 赋值操作
 */
DO* Exec::AssignUp(Node*n)
{
    // cout<<"AssignUp "<<endl;
    LOCALIZE_gc;
    LOCALIZE_stack;

    DO *rv = Evaluat( n->Right() );   // 等号右值
    Node* nl = n->Left();
    NT nt = nl->type;

    // 普通变量赋值
    if(nt!=NT::Variable){
        ERR("AssignUp only to <Variable> !")
    }

    string name = nl->GetName();     // 名字

    // false 向上查找 忽略当前栈帧 
    DO* old = _stack->VarPutUp(name, rv, false);
    if(!old){
        ERR("AssignUp can't find variable : \""+name+"\" !")
    }

    // 释放旧变量
    _gc->Free(old);

	return rv;
}
