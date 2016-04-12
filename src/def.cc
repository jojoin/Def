/**
 *
 * Def 虚拟机入口
 */

/*


1. 统一表达形式
2. 可扩展的内核
3. 操作符绑定
4. 面向对象
5. 类型推导
6. 模式匹配：参数值匹配，精确条件匹配，参数类型匹配

 
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <map>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/Verifier.h"

// 编译执行
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"

#include "./global.h"
#include "./sys/debug.h"

#include "./util/str.h"
#include "./util/fs.h"
#include "./util/path.h"
#include "./util/argv.h"

#include "./parse/tokenizer.h"
#include "./parse/analysis.h"

#include "./compile/gen.h"
#include "./compile/target.h"

#include "./core/error.h"



using namespace std;

using namespace def::util;
using namespace def::sys;
using namespace def::parse;
// using namespace def::compile;



/**
 * 添加命令行参数处理
 */
void addArgvOption(argv::OptionParser & parser)
{
    // 开启指定调试代码
    parser.add_option("-d", "--debug")
        .help("open the specified debug code");
    // 打印 llvm ir
    parser.add_option("-p", "--print")
        // .action("store_true")
        .help("print llvm IR(ir), assembly file(asm) in console");
    // 输出内容到文件
    parser.add_option("-e", "--emit")
        .set_default("obj")
        .help("emit llvm IR file(ir), assembly file(asm) or object file(obj)");
    // 直接执行代码
    parser.add_option("-o", "--output")
        .help("output file for emit");
    // 直接执行代码
    parser.add_option("-r", "--run")
        .help("run the code immediately");





}


/**
 * 添加命令行参数处理
 */
argv::Values getArgs(int argc, char*argv[])
{
    const string usage = "usage: %prog DIR [FILE]... [OPTION]...";
    const string version = "0.1.0";
    const string desc = "Def is a ...";
    const string epilog  = "end of Def";
    // 命令行参数处理
    argv::OptionParser argvparser = argv::OptionParser()
        .usage(usage)
        .version(version)
        .description(desc)
        .epilog (epilog )
        ;
    // 添加解析
    addArgvOption(argvparser);
    // 解析参数
    argv::Values& options = argvparser.parse_args(argc, argv);
    vector<string> args = argvparser.args();
    size_t len = 0;
    for (vector<string>::const_iterator it = args.begin(); 
        it != args.end(); ++it, ++len) {
        options.set("_"+Str::l2s(len)+"_", *it);
        // cout << "arg: " << *it << endl;
    }
    // 解析参数
    return options;
}


/**
 * 开始运行
 */
int run(const string & file, argv::Values & options)
{
    // 文件绝对路径
    string absfile = Path::absolute(file);
    // cout << absfile << endl;
    
    // 初始化词法分析器
    Tokenizer tkz(absfile);

    // 执行语法分析
    Analysis als(&tkz);
    als.checkSetInclude(absfile); // include唯一性
    AST* tree = als.createAST();
    
    // 打印语法分析树
    DEBUG_WITH("ast_tree", \
        cout << endl << endl << "==== AST tree ===" << endl << endl; \
        tree->print(); \
        cout << endl << "====== end ======" << endl << endl;\
        )
    
    // 打印语法分析栈
    DEBUG_WITH("als_stack", \
        cout << endl << endl << "==== Analysis stack ( main ) ===" << endl << endl; \
        als.stack->print(); \
        cout << endl << "====== end ======" << endl << endl; \
        )


    // LLVM CodeGen
    LLVMContext context;
    Gen gen(context, "main");
    // 生成代码
    llvm::Value* value = tree->codegen(gen);
    gen.finish();

    // 优化方案
    /*
    auto theFPM = legacy::FunctionPassManager(&gen->module);
    theFPM.add(createBasicAliasAnalysisPass());
    theFPM.add(createInstructionCombiningPass());
    theFPM.add(createReassociatePass());
    theFPM.add(createGVNPass());
    theFPM.add(createCFGSimplificationPass());
    */
    
    // 打印内容
    if (options.is_set("print")) {
        string print = options["print"];
        if (print=="ir") {
            gen.module.dump();
        }
    }
    
    // 输出内容至文件
    if (options.is_set("emit")&&options.is_set("output")) {
        string emit = options["emit"];
        string filename = options["output"];
        // ast tree
        if (emit=="ast") {
            ofstream outfile(filename);
            streambuf *oldbuf = cout.rdbuf(); 
            cout.rdbuf(outfile.rdbuf());
            tree->print(); // 重定向 cout 流           
            cout.rdbuf(oldbuf);// 复位
        // llvm IR
        } else if (emit=="ir") {
            std::error_code cd;
            raw_fd_ostream ofile(filename, cd, llvm::sys::fs::OpenFlags(8));
            ofile << gen.module;
            ofile.close();
        // 汇编
        } else if(emit=="obj" || emit=="asm") {
            def::compile::Target tar(context, gen.module);
            TargetMachine::CodeGenFileType cgft;
            // 输出类型
            if (emit == "asm") {
                cgft = TargetMachine::CGFT_AssemblyFile;
            } else if (emit == "obj") {
                cgft = TargetMachine::CGFT_ObjectFile;
            }
            // 执行输出
            tar.output(filename, cgft);
        }
    }







    return 0;
}





/**
 * 入口 
 */
int main(int argc, char *argv[])
{
    // 命令行参数
    argv::Values & options = getArgs(argc, argv);

    // 设置调试打印模式
    if (options.is_set("debug")) {
        vector<string> ds;
        Str::split(options["debug"], ",", ds);
        vector<string>::iterator it = ds.begin();
        int len = 1;
        for (; it != ds.end(); ++it, ++len) {
            Str::trim(*it);
            Debug::SetType(*it, len);
            //cout << "[" << *it << "]" <<endl;
        }
    }

    // 检测文件路径
    string file("");
    if (options.is_set("_0_")) {
        file = options["_0_"];
    }
    if (file == "") {
        FATAL("Please enter the source code file path as the first parameter")
    }

    // 正式开始运行
    int rcode = run(file, options);

    DEBUG_WITH("hold", \
        string input; \
        getline(cin, input); \
    )

    return rcode;
}


























/*


int main(int argc, char * argv[]);

int main_cmd(string & cmd);

*/


/**
 * 入口
 *
int main_1(int argc, char *argv[])
{
    // cout << "argc= " << argc << endl;

// 调试编译执行测试文件
#ifdef DEBUG_TEST
    Eval e;
    e.Main("I:/GitHub/Def/build/test.def");
	string s;
    return 0;
#endif

    //
    if(argc==1){
        // 显示简介
        cout<<"Welcome to use Def ( version 0.1.0, www.deflang.org )"<<endl;
        cout<<"available parameters:"<<endl;
        cout<<"  -c,-REPL    Open REPL(Read-Eval-Print Loop)"<<endl;
        cout<<"  -h,-help    Get more help Information"<<endl;
        cout<<"  <file>      Other input will be treated as def code file to run, like 'hello.def'"<<endl;
        cout<<"Now can input your order like above: "<<endl;
        string input;
        getline(cin, input);
        return main_cmd(input);
        
    }else if(argc==2){


        string cmd(argv[1]);
        return main_cmd(cmd);

    }

    return 0;
}
*/






/**
 * 通过命令选择行为
 *
int main_cmd(string & cmd)
{
	// 去除空格
	Str::trim(cmd);

    // 交互式解释器  REPL(Read-Eval-Print Loop)
    if(cmd=="-c"||cmd=="-REPL"){
        cout<<"Input your code, enter to run (quit to end):"<<endl;
        // 动态执行环境
        Eval exec; // 初始化
        string str("");
        bool mul=false;   //多行模式
        while(1){
            cout<<">>>";
            string input;
            getline(cin, input);
            str = input;
            if(input=="quit"){ // 退出
                break; 
            }else if(input==""){ // 进入多行模式
                while(1){
                    cout<<"   ";
                    getline(cin, input);
                    if(input==""){ // 退出多行
                        break;
                    }
                    str += " "+input;
                }
            }
            // cin >> input;
            // cout<<input<<endl;;
            Object* res = exec.Evaluat(str); // 执行求值
            if(res){
                res->Print(true); // 递归打印
                cout<<endl;
            }
            // 缓存复位
            str = "";
        }

    // 解析执行文件
    }else if(cmd!=""){
        // cout << "code file is " << argv[1] << endl;
        Eval exec; // 初始化
        exec.Main( cmd ); // 入口文件执行
    }

	return 0;
}
*/