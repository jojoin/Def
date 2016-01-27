#ifndef DEF_UTIL_FS_H
#define DEF_UTIL_FS_H

/**
 * 文件读取
 */

#include <locale>
#include <fstream>
#include <iostream>
#include <string>

#include "./str.h"

namespace def {
namespace util {

using namespace std;

class Fs {

    public:

    // 判断文件是否存在
    static bool exist(const string &file)
    {
        fstream _file;
        _file.open(file, ios::in);
        return !!_file;
    }

    // 读取文件
    static void read(const wstring & file, wstring & content)
    {
        string fname = Str::ws2s(file);
        read(fname, content);
    }
    static void read(const string & file, wstring & content)
    {
        wifstream winf;
        winf.open(file);
        wstring wstr;
        while(getline(winf, wstr))
        {
            // wcout << wstr << endl;
            content += wstr + L"\n";
        }
        winf.close();
    }
    static void read(const string & file, string & content)
    {
        ifstream inf;
        inf.open(file);
        string str;
        while(getline(inf, str))
        {
            // wcout << wstr << endl;
            content += str + "\n";
        }
        inf.close();
    }


}; // --end-- class Fs

} // --end-- namespace util
} // --end-- namespace def


#endif
// --end-- DEF_UTIL_FS_H