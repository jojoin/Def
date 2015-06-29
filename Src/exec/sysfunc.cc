
/**
 * 调用系统函数
 * @name 函数名
 * @argv 参数列表
 */
DO* Exec::Sysfunc(string name, Node* para)
{
    // cout<<"-Exec::Sysfunc-"<<endl;
    LOCALIZE_gc
    LOCALIZE_stack

    NodeGroup* argv = (NodeGroup*) para;
    size_t len = argv->ChildSize();


    // 打印
    if(name=="print"){

        if(!len>0) return ObjNone();
        DO *obj = Evaluat( argv->Child(0) );
        DO::Print( obj );
        cout<<endl;
        return obj;

    // 求类型
    }else if(name=="type"){

        if(!len>0){
            ERR("System function \"call\" parameter size must 1 !")
        }
        DO *obj = Evaluat( argv->Child(0) );
        return _gc->AllotString( DO::GetTypeName(obj) );

    }else if(name=="int"){


    // 求容器大小
    }else if(name=="size"){
        size_t res_sz = 0;
        if(len>0){
            DO *obj = Evaluat( argv->Child(0) );
            OT t = obj->type;
            if(t==OT::String){ // 字符串长度
                res_sz = ((ObjectString*)obj)->value.size();
            }else if(t==OT::List){ // 列表大小
                 res_sz = ((ObjectList*)obj)->Size();
            }else if(t==OT::Dict){

            }
        }

        return _gc->AllotInt(res_sz);

    // 节点赋值 or 节点执行取值
    }else if(name=="assign" || name=="evaluat"){

        if(len>0){
            // 获得调用环境
            ObjectExec *oe = (ObjectExec*)_stack->VarGet("_call_");
            Exec *exec = (Exec*)oe->GetExec();
            // 节点对象
            ObjectNode *obj = (ObjectNode*)Evaluat( argv->Child(0) );
            OT t = obj->type;
            if(t!=OT::Node){ //错误
                ERR("System function evaluat parameter only type <Node> !")
            }
            Node *target = obj->GetNode();
            if(name=="evaluat"){
                // 在调用环境中执行节点对象
                DO* res = exec->Evaluat( target );
                if(!res){ //错误
                    ERR("System function evaluat call error !")
                }
                return res;
            }
            if(name=="assign"){
                if(!len>1){
                    ERR("System function assign parameter number must 2 !")
                }

                // 赋值
                DO* res = exec->Assign( target, Evaluat( argv->Child(1) ) );
                if(!res){ //错误
                    ERR("System function evaluat call error !")
                }
                return res;
            }
        }
    }

    // 系统函数查询失败
    return NULL;
}