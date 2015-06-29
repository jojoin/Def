/**
 * 取变量值
 */
DO* Exec::Variable(string name)
{
    // 通过名字取得变量值（支持沿作用域向上查找）
    //cout<<"Variable !!!"<<endl;
    DO* val = _envir._stack->VarGetUp( name );
    return val; // 变量不存在返回 NULL
}
