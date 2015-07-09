/**
 * 组合条件判断
 */
DO* Exec::Judge(Node *nl, Node *nr, NT t)
{
	LOCALIZE_gc;
	LOCALIZE_stack;

    // 左变量
    DO *ol = Evaluat(nl);

    // 或运算
    if(t==NT::Or){
        if( ol && Conversion::Bool(ol) ){
            return ol;
        }else{
            return Evaluat(nr);
        }

    // 与运算
    }else if(t==NT::And){
        DO *objr = Evaluat(nr);


        if(
            ol && Conversion::Bool(ol) &&
            objr && Conversion::Bool(objr)
        ){
            return ObjTrue();
        }else{
            return ObjFalse();
        }
    }


    return ObjFalse();

}
