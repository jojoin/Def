
/**
 * 算法操作
 * @param t 比较种类 = > < >= <= ~= ~
 */
DO* Exec::Compare(Node *nl, Node *nr, NT t)
{
    // 正式运算
    DO *l = Evaluat(nl),
              *r = Evaluat(nr);

    OT lt = l->type,
       rt = r->type;

    DO *_t = ObjTrue(),
              *_f = ObjFalse();

// 数值比较
#define COMPARE(T1,O1,T2,O2)                             \
    if( lt==OT::T1 && rt==OT::T2 ){                      \
    O1 vl = ((Object##T1*)l)->value;                     \
    O2 vr = ((Object##T2*)r)->value;                     \
    switch(t){                                           \
    case NT::Equal:     return vl==vr?_t:_f;             \
    case NT::More:      return vl> vr?_t:_f;             \
    case NT::Less:      return vl< vr?_t:_f;             \
    case NT::MoreEqual: return vl>=vr?_t:_f;             \
    case NT::LessEqual: return vl<=vr?_t:_f;             \
    case NT::NotEqual:  return vl!=vr?_t:_f;             \
    }}

    COMPARE(Int,long,Int,long)
    COMPARE(Int,long,Float,double)
    COMPARE(Float,double,Int,long)
    COMPARE(Float,double,Float,double)

#undef COMPARE

    // 字符串等于、不等于比较
    if( lt==OT::String && rt==OT::String )
    {
        string vl = ((ObjectString*)l)->value;
        string vr = ((ObjectString*)r)->value;
        bool ret = false;
        if(t==NT::Equal) return vl==vr?_t:_f;
        if(t==NT::NotEqual) return vl!=vr?_t:_f;
    }

    // 没有匹配的比较算法
    ERR("Can't compare !");
}


