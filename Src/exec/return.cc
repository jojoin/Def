/**
 * 函数或处理器返回
 */
DO* Exec::Return(Node*n)
{
    LOCALIZE_gc
    // cout<<"Return !!!"<<endl;
    NodeReturn *p = (NodeReturn*)n;
    // 求返回值
    DO* obj;
    Node* chd = p->Child();
    if(chd){
        obj = Evaluat( chd );
        _gc->Quote( obj ); // 加引用
    }else{
        obj = ObjNone(); // 默认返回 none
    }
    
    // 抛出返回
    throw new Throw(ThrowType::Return, "", obj);
}  