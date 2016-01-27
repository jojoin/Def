/**
 * 代码分析错误报告
 */


#include "./error.h"


using namespace def::core;


#define EP Error::Position


Tokenizer* Error::tkz = nullptr;
vector<EP> Error::positions;


/**
 * 错误终止
 */
void Error::exit(const string & head, const string & foot)
{
    // 输出头部信息
    cerr<<endl<<"[Error]: "<<head<<endl;

    // 打印焦点行
    cerr << endl << getFoucsLine() << endl;
    
    EP cur = curPosition();
    // 焦点行
    //cerr << endl  << cur.foucs << endl;
    // 文件路径 (line, cursor)
    cerr << tkz->file << " (" << cur.line
         << "," << (cur.cursor+1) << ")" << endl;

    // 输出尾部信息
    cerr << endl << foot << endl;

    std::exit(0);
}



/**
 * 打印代码游标位置
 */
string Error::getFoucsLine()
{
    ifstream fin(tkz->file);
    EP cur = curPosition();
    int l = cur.line;
    char lcs[1024];
    while (l--)
        fin.getline(lcs, 1024); // 丢弃之前行
    string line(lcs);
    // 计算 focus位置
    int wrap = 33;
    int fcs = cur.cursor;
    // 左截断
    int lov = cur.cursor - wrap;
    if (lov>0) {
        line = "..." + line.substr(lov);
        fcs -= lov - 3;
    }
    // 右截断
    int rov = line.size() - cur.cursor - wrap;
    if (rov>0) {
        line = line.substr(0, wrap*2 + 3) + "...";
    }
    // 返回 
    string foucs(fcs, ' ');
    return line + "\n" + foucs + "^"; // ☉-^△↑^☉⊙○●◥△▲*~-
}


/**
 * 建立快照
 */
EP Error::curPosition()
{
    if (!positions.empty()) {
        return positions[positions.size()-1];
    }
    else {
        return createPosition();
    }
}



/**
 * 建立快照
 */
EP Error::createPosition()
{
    tkz->jumpWhitespace(); // 跳过空白
    EP ep;
    // ep.foucs = tkz->getFoucsLine();
    ep.line = tkz->curline;
    ep.cursor = tkz->cursor;
    return ep;
}



/**
 * 建立并保存快照
 */
void Error::snapshot()
{
    positions.push_back(createPosition());
}


/**
 * 回退快照
 */
void Error::backspace(int n)
{
    while(n--)
        positions.pop_back();
}
