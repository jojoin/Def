/**
 * 循环继续
 */
DO* Exec::Continue(Node*n)
{
    // cout<<"Continue !!!"<<endl;
    // 抛出
    throw new Abnor(AbnorType::Continue);
}  