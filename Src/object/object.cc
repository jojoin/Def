/**
 * 
 */

#include <string>
#include <map>


#include "object.h"
#include "operat.h"

namespace def {
namespace object {



#define T ObjectType

/**
 * 打印对象
 * @r 是否递归打印容器，并且格式化变量
 */
void DefObject::Print(DefObject *obj, bool r){

    T t = obj->type; // 获取类型

    if( t==T::Int ){
        //cout<<"-Print Int-"<<endl;
        cout << ((ObjectInt*)obj)->value;

    }else if( t==T::Float ){
        //cout<<"-Print Float-"<<endl;
        cout << ((ObjectFloat*)obj)->value;

    }else if(t==T::String){
        if(r) cout << "\"";
        cout << ((ObjectString*)obj)->value;
        if(r) cout << "\"";

    }else if(t==T::List){ // 列表
        //cout<<"-Print List-"<<endl;
        if(!r){
            cout<<"<list>";
            return;
        }
        ObjectList* list = (ObjectList*)obj;
        cout << "(";
        //size_t sz = obj->Size();
        size_t sz = list->Size();
        for(size_t i=0; i<sz; i++){
            if(i) cout<<" ";
            Print( list->Visit(i) );
        }
        cout << ")";

    }else if(t==T::Dict){ // 字典
        //cout<<"-Print Dict-"<<endl;
        if(!r){
            cout<<"<dict>";
            return;
        }
        ObjectDict* dict = (ObjectDict*)obj;
        cout << "[";
        map<string, DefObject*>::iterator it = dict->value.begin();
        bool dv = false;
        for(;it!=dict->value.end();++it){
            if(dv) cout<<", "; else dv=true;
            cout<<"'"<<it->first<<"'";
            Print( it->second );   
        }
        cout << "]";

    // 代码节点对象
    }else if(t==T::Proc){ cout << "<process>";
    }else if(t==T::Func){ cout << "<function>";
    }else if(t==T::Node){ cout << "<node>";
    }else if(t==T::Exec){ cout << "<exec>";
    }else if(t==T::Sysmod){ cout << "<sysmod>";

    }else if(t==T::None){
        cout << "none";

    }else if(t==T::Bool){
        if( Conversion::Bool(obj) ){
            cout << "true";
        }else{
            cout << "false";
        }

    }else{

    }
}


/**
 * 获取变量类型名称
 */
string DefObject::GetTypeName(DefObject *obj)
{
    T t = obj->type;
#define ELSEIF(N,S) else if(t==T::N){ return #S; }
    if(t==T::Error){ return "error"; }
    ELSEIF(None,none)
    ELSEIF(Bool,bool)
    ELSEIF(Int,int)
    ELSEIF(Float,float)
    ELSEIF(String,string)
    ELSEIF(List,list)
    ELSEIF(Dict,dict)
    ELSEIF(Block,block)
    ELSEIF(Func,function)
    ELSEIF(Proc,process)
    ELSEIF(Node,node)
    ELSEIF(Class,class)
    ELSEIF(Object,object)
    ELSEIF(Module,module)
    ELSEIF(Exec,exec)
    ELSEIF(Sysmod,sysmod)
#undef ELSEIF
    return ""; // 未匹配
}


#undef T   // ObjectType




} // --end-- namespace object
} // --end-- namespace def



