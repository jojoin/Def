/**
 * 语法分析类
 */


#include "analysis.h"

#include "../sys/debug.h"

#include "../core/builtin.h"


using namespace std;
using namespace def::util;
using namespace def::sys;
using namespace def::compile;


namespace def {
namespace parse {

Analysis::Analysis(Tokenizer * t)
    : Build(t)
{

}



}
}