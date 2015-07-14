/**
 * 组合条件判断
 */
DO* Exec::Throw(Node *n)
{
    LOCALIZE_gc
    // cout<<"Throw !!!"<<endl;
    NodeThrow *p = (NodeThrow*)n;
    // 求返回值
    DO* obj;
    Node* chd = p->Child();
    if(chd){
        obj = Evaluat( chd );
        _gc->Quote( obj ); // 加引用
    }else{
        obj = ObjNone(); // 默认抛出 none
    }
    
    // 抛出返回
    throw new Abnor(AbnorType::Throw, "", obj);

}