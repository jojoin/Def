/**
 * block 块数据结构建立
 */
DO* Exec::Block(Node*n)
{
    LOCALIZE_gc
    NodeBlock* p = (NodeBlock*)n;

    ObjectBlock* block = _gc->AllotBlock();
    size_t i = 0
         , s = p->ChildSize();
    while( i < s ){
        // 添加数组项目
        block->Push( _gc->AllotNode( p->Child(i) ) );
        i++;
    }

    return block;

}

