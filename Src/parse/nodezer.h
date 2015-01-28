
#ifndef DEF_NODEZER_H
#define DEF_NODEZER_H

#include <string>
#include <vector>

#include "tokenizer.h"

using namespace std;
using namespace def::token;


namespace def {
namespace node {

class Nodezer {

	public:

	Nodezer(vector<Word>&);

	private:

	// 词法分析后的单词列表
	vector<Word>& words;

}; // --end-- class Nodezer

} // --end-- namespace node
} // --end-- namespace def



#endif
// --end-- DEF_NODEZER_H

