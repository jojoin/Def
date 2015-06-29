/**
 * list 列表结构建立
 */
DO* Exec::List(Node* n)
{
	LOCALIZE_gc

    NodeList* p = (NodeList*)n;

    ObjectList* list = _gc->AllotList();
    size_t i = 0
         , s = p->ChildSize();
    while( i < s ){
        // 添加数组项目
        list->Push( Evaluat( p->Child(i) ) );
        i++;
    }

    return list;
}