

/**
 * 打印对象
 */
DO* Exec::Print(Node *n)
{
    LOCALIZE_gc;

    DO* obj = Evaluat( n->Child() );
    DO::Print( obj ); // 求值并打印
    cout << endl;

    // 临时变量释放
    if( n->IsValue() || n->IsOperate() ){
        //cout<<"Free Literals or Algorithm Value"<<endl;
        _gc->Free(obj);
    }

    return obj;
}
