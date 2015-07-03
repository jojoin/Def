
/**
 * Not 取反
 */
DO* Exec::Not(Node* n)
{
    NodeNot* p = (NodeNot*)n;
    if( Conversion::Bool( Evaluat( p->Child() ) ) ){
        return ObjFalse();
    }

    return ObjTrue();
}

