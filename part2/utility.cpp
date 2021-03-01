#include "utility.h"
#include "symtab.h"
#include <cstdlib>

extern vector<string> tiggercode;

map<RegName,RawVal> RegManager::Reg2VarMap;//全局的
map<RegName,bool> RegManager::RegDirtyMap;//全局的true代表是脏数据，false代表是已经同步数据或者没有数据
//stack<StkMngPtr> RegManager::FuncStackStack;//维护一个存储函数栈的栈
StkMngPtr RegManager::CurrStack=NULL;
int RegManager::fifoptr=18;//下一个分配的寄存器指针

string StringFormat(const char *format, ...)
{
    string result;
    //将字符串的长度初始化为1024
    int tlen = 1024;
    int len = 0;
    result.resize(tlen);
    
    //为string赋值
    va_list list;
    va_start(list, format);
    len = vsnprintf((char*)result.c_str(), tlen, format, list);
    
    //如果结果字符串长度比初始长度长，就重新设置字符串长度，然后再赋值
    if( len >= tlen )
    {
        tlen = len + 1;
        result.resize(tlen);
        len = vsnprintf((char*)result.c_str(), tlen, format, list);
    }
    va_end(list);
    
    //删除字符串尾部的0字符
    result.erase(len);
    return result;
}

string GetRegName(RegName regn){
    string res;
    string regnum;
    if(regn>=0&&regn<7){
        regnum=to_string(regn);
        res="t"+regnum;
    }
    else if(regn>=7&&regn<19){
        regnum=to_string(regn-7);
        res="s"+regnum;
    }
    else if(regn>=19&&regn<27){
        regnum=to_string(regn-19);
        res="a"+regnum;
    }
    else{
        res="x0";
    }
    return res;
}

RegName GetRegEnum(string regname){
    string regnum;
    string regtype;
    RegName res=err;
    regtype=regname.substr(0,1);
    regnum=regname.substr(1,regname.length());
    if(regtype.compare("t")==0){
        res=(RegName)atoi(regnum.c_str());
    }else if(regtype.compare("s")==0){
        res=(RegName)(atoi(regnum.c_str())+7);
    }else if(regtype.compare("a")==0){
        res=(RegName)(atoi(regnum.c_str())+19);
    }else if(regtype.compare("x")==0){
        res=x0;
    }else{
        cout<<"regname translate error!"<<endl;
    }
    return res;
}

string RegManager::GetVarReg(string varia){
    RegName rn;
    string res;
    bool isfind=false;

    for(int i=0;i<27;i++){
        rn=(RegName)i;
        if(Reg2VarMap[rn].type==VAR_TYPE&&Reg2VarMap[rn].varname.compare(varia)==0){
            isfind=true;
            res=GetRegName(rn);
        }
    }

    if(!isfind){
        res = AllocVarReg(varia);
    }

    return res;
}

string RegManager::AllocVarReg(string varia){
    UpdatePtr();
    string res=GetRegName((RegName)fifoptr);

    RawVal rawvcurr,rawvpast;
    int stackptr;

    rawvcurr.type=VAR_TYPE;
    rawvcurr.varname=varia;
    
    
    rawvpast=Reg2VarMap[(RegName)fifoptr];

    
    //如果是一个变量，那么需要将这个变量写回到栈里再进行分配,简易版本不用实现脏数据写回
    if(rawvpast.type==VAR_TYPE){
        //如果是局部变量并且不是数组的情况下，需要将寄存器里的内容保存到栈里
        if (SymTable::IsLocal(rawvpast.varname))
        {
            if(!RegManager::CurrStack){
                cout<<"CurrStack is NULL but variable is local"<<endl;
                exit(-1);
            }
            if (!SymTable::IsArray(rawvpast.varname))
            {
                stackptr=CurrStack->GetStack(rawvpast.varname);
                Reg2VarMap[(RegName)fifoptr]=rawvcurr;

                cout<<"store "<<res<<" "<<stackptr<<endl;
                tiggercode.push_back(StringFormat("store %s %d\n",res.c_str(),stackptr));
                
                CurrStack->StackBitMap[stackptr]=true;
            }
        }
    }
    
    Reg2VarMap[(RegName)fifoptr]=rawvcurr;
    return res;
}

string RegManager::AllocImmReg(int imm){
    
    string res;
    
    if(imm==0){
        res="x0";
        return res;
    }

    UpdatePtr();//更新的寄存器位置
    res=GetRegName((RegName)fifoptr);

    RawVal rawvcurr,rawvpast;
    int stackptr;
    rawvcurr.type=IMM_TYPE;
    rawvcurr.val=imm;
    
    rawvpast=Reg2VarMap[(RegName)fifoptr];

    //如果是一个变量，那么需要将这个变量写回到栈里再进行分配
    if(rawvpast.type==VAR_TYPE){
        //如果是局部变量并且不是数组的情况下，需要将寄存器里的内容保存到栈里
        if (SymTable::IsLocal(rawvpast.varname))
        {
            if(!RegManager::CurrStack){
                cout<<"CurrStack is NULL but variable is local"<<endl;
                exit(-1);
            }
            if (!SymTable::IsArray(rawvpast.varname))
            {
                stackptr=CurrStack->GetStack(rawvpast.varname);
                if(stackptr==-1){
                    cout<<"when writing back,find no stack space allocated"<<endl;
                }
                Reg2VarMap[(RegName)fifoptr]=rawvcurr;

                cout<<"store "<<res<<" "<<stackptr<<endl;
                tiggercode.push_back(StringFormat("store %s %d\n",res.c_str(),stackptr));
                
                CurrStack->StackBitMap[stackptr]=true;
            }
        }
    }
    Reg2VarMap[(RegName)fifoptr]=rawvcurr;
    return res;
}

void RegManager::SetRegVar(int regn,string varia){
    RawVal var;
    var.type=VAR_TYPE;
    var.varname=varia;
    Reg2VarMap[(RegName)regn]=var;
}

void RegManager::UpdatePtr(){//可以搭配活性检查算法
    fifoptr+=1;
    fifoptr%=19;
}

void RegManager::CallFunc(StkMngPtr fstack){
    CurrStack=fstack;
}

void RegManager::PrintRegUsage(){
    cout<<"name\t|type\t|var"<<endl;
    for(int i=0;i<28;i++){
        cout<<GetRegName((RegName)i)<<"\t|"<<Reg2VarMap[(RegName)i].type<<"\t|"<<Reg2VarMap[(RegName)i].varname<<endl;
    }
    cout<<endl;
}

bool RegManager::CheckReg(string varia){
    bool isfind=false;
    RegName rn;
    for(int i=0;i<27;i++){
        rn=(RegName)i;
        if(Reg2VarMap[rn].type==VAR_TYPE&&Reg2VarMap[rn].varname.compare(varia)==0){
            isfind=true;
        }
    }
    return isfind;
}

void RegManager::RegStore(string regname_s){
    RegName rn;
    RawVal var,emp;
    int stackptr;
    rn=GetRegEnum(regname_s);
    var=Reg2VarMap[rn];
    emp.type=EMP_TYPE;

    //如果是一个变量，那么需要将这个变量写回到栈里再进行分配,简易版本不用实现脏数据写回
    if(var.type==VAR_TYPE){
        //如果是局部变量并且不是数组的情况下，需要将寄存器里的内容保存到栈里
        if (SymTable::IsLocal(var.varname))
        {
            if(!RegManager::CurrStack){
                cout<<"CurrStack is NULL but variable is local"<<endl;
                exit(-1);
            }
            if (!SymTable::IsArray(var.varname))
            {
                stackptr=CurrStack->GetStack(var.varname);
                Reg2VarMap[(RegName)fifoptr]=emp;

                cout<<"store "<<regname_s<<" "<<stackptr<<endl;
                tiggercode.push_back(StringFormat("store %s %d\n",regname_s.c_str(),stackptr));
                
                CurrStack->StackBitMap[stackptr]=true;
            }
        }
    }
}

void RegManager::RegEnumStore(RegName rname){
    RawVal var,emp;
    int stackptr;
    string regname_s;
    regname_s=GetRegName(rname);
    var=Reg2VarMap[rname];
    emp.type=EMP_TYPE;
    
    //如果是一个变量，那么需要将这个变量写回到栈里再进行分配,简易版本不用实现脏数据写回
    if(var.type==VAR_TYPE){
        //如果是局部变量并且不是数组的情况下，需要将寄存器里的内容保存到栈里
        if (SymTable::IsLocal(var.varname))
        {
            if(!RegManager::CurrStack){
                cout<<"CurrStack is NULL but variable is local"<<endl;
                exit(-1);
            }
            if (!SymTable::IsArray(var.varname))
            {
                stackptr=CurrStack->GetStack(var.varname);
                Reg2VarMap[(RegName)fifoptr]=emp;
                cout<<"store "<<regname_s<<" "<<stackptr<<endl;
                tiggercode.push_back(StringFormat("store %s %d\n",regname_s.c_str(),stackptr));
                
                CurrStack->StackBitMap[stackptr]=true;
            }
        }
    }
}

int StackManager::GetStack(string varia){
    bool isfind=false;
    int res=-1;
    for(int i=0;i<FuncStack.size();i++){
        if(FuncStack[i].type==VAR_TYPE&&varia.compare(FuncStack[i].varname)==0){
            res=i;
            isfind=true;
        }
    }
    if(!isfind){
        printf("error, a temp var used before kept in stack,var is %s\n",varia.c_str());
    }
    return res;
}

int StackManager::AllocStack(string varia,int vsize){
    int res;
    res=FuncStack.size();
    RawVal rawv,rawp;
    rawv.type=VAR_TYPE;
    rawv.varname=varia;
    rawp.type=ARRP_TYPE;
    for(int i=0;i<vsize;i++){
        if(i==0){
            FuncStack.push_back(rawv);
            StackBitMap.push_back(false);
        }else{
            FuncStack.push_back(rawp);
            StackBitMap.push_back(false);
        }
    }
    return res;
}

void StackManager::StoreReginStack(){
    //TODO:全存语句
    //printf("将活跃的寄存器压栈\n");
}

void StackManager::RestoreReginStack(){
    //TODO:全恢复语句
    //printf("将活跃的寄存器压栈");
}

void StackManager::ResetStack(){
    FuncStack.clear();
    StackBitMap.clear();
}


