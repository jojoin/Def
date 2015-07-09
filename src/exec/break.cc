/**
 * 循环退出
 */
DO* Exec::Break(Node*n)
{
    // cout<<"Break !!!"<<endl;
    // 抛出
    throw new Throw(ThrowType::Break);
}  