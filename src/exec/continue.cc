/**
 * 循环继续
 */
DO* Exec::Continue(Node*n)
{
    // cout<<"Continue !!!"<<endl;
    // 抛出
    throw new Throw(ThrowType::Continue);
}  