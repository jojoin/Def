
#include "filter.h"

using namespace std;
using namespace def::parse;


/**
 * 操作符绑定筛选器
 */



filterLet::filterLet(Stack*stk, const string & name)
{
    while (stk) { // 循环检查
        initElementGroup(stk, name);
        stk = stk->parent;
    }
}


// 筛选函数组
void filterLet::initElementGroup(Stack*stk, const string & name)
{
    int len = name.size();
    for (auto &p : stk->stack) {
        if (auto *let = dynamic_cast<ElementLet*>(p.second)) {
            if (name == p.first.substr(0,len)) {
                if (lets.end()==lets.find(name)) {
                    lets[p.first] = let;
                }
                if (name == p.first) {
                    if (!unique) unique = let;
                }
            }
        }
    }
}
// 筛选并匹配 返回匹配数量
size_t filterLet::size()
{
    return lets.size();
}

// 筛选并匹配 返回匹配数量
size_t filterLet::match(const string & name)
{
    unique = nullptr; // 复位
    map<string, ElementLet*> new_lets;
    int len = name.size();
    for (auto &p : lets) {
        if (name == p.first.substr(0, len)) {
            if (name == p.first) {
                if (!unique) unique = p.second;
            }
            new_lets[p.first] = p.second;
        }
    }
    lets = new_lets;
    return new_lets.size();
}




/**
 * 函数筛选器
 */




filterFunction::filterFunction(Stack*stk, const string & fname, bool up)
{
    if(!up){
        initElementGroup(stk, fname);
    } else {
        while (stk) { // 循环检查
            initElementGroup(stk, fname);
            stk = stk->parent;
        }
    }
}
// 筛选函数组
void filterFunction::initElementGroup(Stack*stk, const string & fname)
{
    if (auto *fngr = dynamic_cast<ElementGroup*>(stk->find(fname, false))) {
        for(auto &p: fngr->elms){
            if (auto fun=dynamic_cast<ElementFunction*>(p.second)) {
                funcs.push_back(fun);
            }
        }
    }
}
// 筛选并匹配 返回匹配数量
size_t filterFunction::size()
{
    return funcs.size();
}
// 筛选并匹配 返回匹配数量
size_t filterFunction::match(TypeFunction* fty) 
{
    bool is_empty_arr = 1;
    unique = nullptr; // 复位
    vector<ElementFunction*> new_funcs;
    string tmpname = fty->getIdentify(false); // 非严格，允许数组大小不相等
    int tmplen = tmpname.size();
    for (auto &p : funcs) {
        string mname = p->fndef->ftype->getIdentify(false);
        if (tmpname == mname.substr(0, tmplen)) {
            if (tmpname==mname) {
                if(!unique) unique = p->fndef;
            }
            // 检测数组类型参数是否合格（数组参数形参长度为0或与实参相等）
            bool atyok = true;
            int i(0);
            for (auto ty : fty->types) {
                if (auto *aty = dynamic_cast<TypeArray*>(p->fndef->ftype->types[i])) {
                    if (aty->len != 0 && aty->len != ((TypeArray*)ty)->len) {
                        atyok = false;
                    }
                }
                i++;
            }
            if(atyok) new_funcs.push_back(p);
        }
    }
    funcs = new_funcs;
    return new_funcs.size();
}

