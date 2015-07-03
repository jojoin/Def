

/**
 * 算法操作
 * @param t 算法种类 + - * /
 */
DO* Exec::Operate(Node *nl, Node *nr, NT t)
{
	LOCALIZE_gc;
	LOCALIZE_stack;

    // 取负运算
    if( !nl && t==NT::Sub){
        DO *r = Evaluat(nr);
        if(r->type==OT::Int){
            return _gc->AllotInt( 0 - ((ObjectInt*)r)->value );
        }else if(r->type==OT::Float){
            return _gc->AllotInt( 0.0 - ((ObjectFloat*)r)->value );
        }
        ERR("Err:  only <Int> and <Float> type can get negative value !");
    }

    // 正式运算
    DO *l = Evaluat(nl);
    DO *r = Evaluat(nr);
    DO *result = NULL;

    OT lt = l->type;
    OT rt = r->type;

    // 整数算法
    if( lt==OT::Int && lt==OT::Int ){

        long vl = ((ObjectInt*)l)->value
           , vr = ((ObjectInt*)r)->value
           , res= 0;
        switch(t){
        case NT::Add: res = vl + vr; break;
        case NT::Sub: res = vl - vr; break;
        case NT::Mul: res = vl * vr; break;
        case NT::Div: res = vl / vr; break;
        }
        result = _gc->AllotInt(res);

    }else if( lt==OT::String && lt==OT::String ){
        string str = ((ObjectString*)l)->value 
                   + ((ObjectString*)r)->value;
        if(t==NT::Add){ //字符串 +
            result = _gc->AllotString(str);
        }

    }else{



    }

    // 参与计算的临时变量的释放
    if( nl->IsValue() || nl->IsOperate() ){
        //cout<<"nl->type==NT::Int"<<endl;
        _gc->Free(l);
    }
    if( nr->IsValue() || nr->IsOperate() ){
        //cout<<"nr->type==NT::Int"<<endl;
        _gc->Free(r);
    }


    return result;
}


