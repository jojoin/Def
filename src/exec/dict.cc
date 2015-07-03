/**
 * dict 数据结构建立
 */
DO* Exec::Dict(Node* n)
{
	LOCALIZE_gc

    NodeDict* p = (NodeDict*)n;

    ObjectDict* dict = _gc->AllotDict();
    size_t i = 0
         , s = p->ChildSize();
    while( i < s ){
        // 添加数组项目
        string key = Conversion::String( Evaluat( p->Child(i) ) );
        if(key!=""){
            dict->Set( 
                key, 
                Evaluat( p->Child(i+1) )
            );
        }
        i += 2;
    }

    return dict;
}
