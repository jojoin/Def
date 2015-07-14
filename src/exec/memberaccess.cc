/**
 * MumberAccess 成员访问
 */
DO* Exec::MemberAccess(Node* n)
{
    // cout<<"-Exec::MemberAccess-"<<endl;
    NodeMemberAccess *p = (NodeMemberAccess*) n;

    Node* left = p->Left();
    DO* base = Evaluat( left );
    Node* right = p->Right();
    if(right->type!=NT::Variable){
        ERR("Mumber name must be a Variable !");
    }
    // 访问名字
    string rname = right->GetName();

    return MemberAccess(base, rname);
}



/**
 * MumberAccess 成员访问
 */
DO* Exec::MemberAccess(DO* base, string name)
{
    DO* result = NULL; //成员访问结果
    OT bt = base->type;
    // 模块访问
    if( bt==OT::Module ){
        // cout<<"Module::key = "<<right->GetName()<<endl;
        result = ((ObjectModule*)base)->Visit( name ); 
    }

    if(!result){
        ERR("Can't find member '"+name+" !")
    }

    // cout<<"return result="<<(int)result<<endl;
    // return result ? result : ObjNil(); // 无效访问 返回 Nil
    return result;

}
