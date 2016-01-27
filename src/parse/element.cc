/**
 *
 */


#include "./element.h"

using namespace std;
using namespace def::parse;

/**
 * 通过前缀最名称过滤
 */
void ElementGroup::filter(string prefix, vector<Element*> & result)
{
    result.clear(); // 清空
    size_t ns = prefix.size();
    for (auto &it : elms) {
        if (prefix==it.first.substr(0,ns)) {
            result.push_back(it.second);
        }
    }
}



#define FUNC_HEAD_STR(T) \
string Element##T::str() { 


FUNC_HEAD_STR(Variable)
    return "<variable> "+type->getIdentify();
}


FUNC_HEAD_STR(Type)
    return "<type> "+type->getIdentify();
}


FUNC_HEAD_STR(Function)
    string s = fndef->body ? "" : "no body";
    return "<function> " + s;
}


FUNC_HEAD_STR(TemplateFuntion)
    return "<template function>";
}


FUNC_HEAD_STR(Group)
    string s("<group> ");
    for (auto &li : elms) {
        s += "\n  " + li.first + ": " + li.second->str();
    }
    return s;
}


FUNC_HEAD_STR(Let)
    string s("<let>");
    for (auto &p : params) {
        s += " " + p ;
    }
    s += "=>";
    for (auto &w : bodywords) {
        s += w.value + " ";
    }
    return s;
}



