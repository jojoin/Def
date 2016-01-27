

#include "./type.h"


using namespace std;
using namespace def::core;


/**
 * 获取或缓存类型 
 */
Type* Type::get(const string & name)
{
    // 查询缓存
    auto ty = types.find(name);
    if (ty!=types.end()) {
        return ty->second;
    }
    // 创建
    Type* tp = nullptr;
    if (name=="Nil") {
        tp = new TypeNil();
    } else if (name=="Bool") {
        tp = new TypeBool();
    } else if (name=="Int") {
        tp = new TypeInt();
    } else if (name=="Float") {
        tp = new TypeFloat();
    } else if (name=="Char") {
        tp = new TypeChar();
    } else if (name=="String") {
        tp = new TypeString();
    }
    // 缓存
    if (tp) {
        types[name] = tp;
    }
    return tp;
}

// 静态初始化
map<string, Type*> Type::types;


long TypeStruct::auto_idx = 0;
