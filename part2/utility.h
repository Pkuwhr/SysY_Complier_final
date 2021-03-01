#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include <cstdarg>
#include <map>
#include <vector>
#include <stack>
#include <sstream>
#include <iostream>
using namespace std;

extern vector<string> tiggercode;

enum RegName{
    t0,t1,t2,t3,t4,t5,t6,//0-6
    s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,//7-18
    a0,a1,a2,a3,a4,a5,a6,a7,//19-26
    x0,//27
    err//寄存器分配错误类型
};

enum RawValType{
    VAR_TYPE,IMM_TYPE,
    ARRP_TYPE,EMP_TYPE//ARRP(array padding),EMP(empty)
};

typedef struct RawVal{
    RawValType type;
    int val;
    string varname;
}RawVal;

string StringFormat(const char *format, ...);//这是一个格式化输入工具，将代码按格式打印到字符串里
string GetRegName(RegName regn);
RegName GetRegEnum(string regname);

class StackManager{
    public:
        vector<RawVal> FuncStack;//保存栈的存储信息
        vector<bool> StackBitMap;//保存栈的位置活跃信息
        //vector<int> SavedRegPtr;//寄存器被保存在的位置

        StackManager(){
            FuncStack.clear();
            StackBitMap.clear();
        };

        //int FindFreeSpace(int psize);//查找空闲的位置,根据连续空间需求,暂不实现

        int GetStack(string varia);
        int AllocStack(string varia,int vsize);

        void StoreReginStack();
        void RestoreReginStack();

        void ResetStack();
};

typedef StackManager StkMng;
typedef StkMng * StkMngPtr;

class RegManager{
    public:
        static map<RegName,RawVal> Reg2VarMap;//全局的
        static map<RegName,bool> RegDirtyMap;//全局的true代表是脏数据，false代表是已经同步数据或者没有数据
        static StkMngPtr CurrStack;
        static int fifoptr;//下一个分配的寄存器指针
   
        RegManager(){
            Reg2VarMap.clear();
            RegDirtyMap.clear();
            fifoptr=18;
            for(int i=0;i<28;i++){
                RegDirtyMap[(RegName)i]=false;
            }
        }
        static void InitReg(){
            Reg2VarMap.clear();
            RegDirtyMap.clear();
            fifoptr=18;
            for(int i=0;i<28;i++){
                Reg2VarMap[(RegName)i].type=EMP_TYPE;
                RegDirtyMap[(RegName)i]=false;
            }
        }
        
        static string GetVarReg(string varia);//为varia找寄存器
        static string AllocVarReg(string varia);//为varia分配新的寄存器
        static string AllocImmReg(int imm);//为立即数分配一个寄存器
        static void SetRegVar(int regn,string varia);//手动指定分配
        static void UpdatePtr();
        static void PrintRegUsage();
        static void CallFunc(StkMngPtr fstack);//跳转时，添加新的函数栈
        static void RegStore(string regname_s);
        static void RegEnumStore(RegName rname);
        static bool CheckReg(string varia);//判断是不是已经分配了
};


#endif