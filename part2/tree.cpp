#include "tree.h"
extern vector<string> tiggercode;

string StmNode::GenCode()
{
    switch (kind)
    {
    case GOTOIF_KIND:
    {
        string condi = child[0]->GenUseCode();
        string label = child[1]->GenCode();

        cout << "if " << condi << " goto " << label << endl;
        tiggercode.push_back(StringFormat("if %s goto %s\n",condi.c_str(),label.c_str()));
        
        break;
    }
    case GOTODO_KIND:
    {
        string label = child[0]->GenCode();
        
        cout << "goto " << label << endl;
        tiggercode.push_back(StringFormat("goto %s\n",label.c_str()));
        
        break;
    }
    case PARAME_KIND:
    {
        string paramIdx = Counter::GenParamCode();
        string paramIde = child[0]->GenUseCode();
        
        cout << paramIdx << " = " << paramIde << endl;
        tiggercode.push_back(StringFormat("%s = %s\n",paramIdx.c_str(),paramIde.c_str()));
        
        break;
    }
    case RETURN_KIND:
    {
        string paramRet = child[0]->GenUseCode();
       
        cout << "a0 = " << paramRet << endl;
        tiggercode.push_back(StringFormat("a0 = %s\n",paramRet.c_str()));
        
        cout << "return" << endl;
        tiggercode.push_back(StringFormat("return\n"));
        
        break;
    }
    case ASSIGN_KIND:
    {
        string rvalue = child[1]->GenUseCode();
        string lvalue = child[0]->GenDefCode();
        cout << lvalue << " = " << rvalue << endl;
        tiggercode.push_back(StringFormat("%s = %s\n",lvalue.c_str(),rvalue.c_str()));
        break;
    }
    case FUNDEF_KIND:
    {
        int defline=tiggercode.size();
        int stacksize;
        StackManager fstack;
        RegManager::CallFunc(&fstack);
        string funct = child[0]->GenCode();
        string parac = child[1]->GenDefCode();

        cout << funct << " [ " << parac << " ] " << endl;
        tiggercode.push_back(StringFormat("%s [ %s ]",funct.c_str(),parac.c_str()));
        
        for (int i = 0; i < atoi(parac.c_str()); i++){
            RegManager::CurrStack->AllocStack("p"+to_string(i),1);
            RegManager::SetRegVar(19+i,"p"+to_string(i));
        }
        child[2]->GenCode();

        cout << "end " << funct << endl;
        tiggercode.push_back(StringFormat("end %s\n",funct.c_str()));

        stacksize=fstack.FuncStack.size();
        tiggercode[defline]=tiggercode[defline]+" [ "+to_string(stacksize)+" ]\n";//增加一个栈信息
        
        RegManager::InitReg();
        break;
    }
    case VARDEF_KIND:
        if (this->globalStm)
        {
            string varia = child[0]->GetName();
            string globv = SymTable::InsertGlobal(varia);

            cout << globv << " = 0" << endl;
            tiggercode.push_back(StringFormat("%s = 0\n",globv.c_str()));

        }else{
            string varia = child[0]->GetName();
            SymTable::InsertLocal(varia);
            if(!RegManager::CurrStack){
                cout<<"define local var without stack"<<endl;
                exit(-1);
            }
            RegManager::CurrStack->AllocStack(varia,1);
        }
        break;
    case LABELT_KIND:
    {
        string label = child[0]->GenCode();
        
        cout << label << " :" << endl;
        tiggercode.push_back(StringFormat("%s :\n",label.c_str()));
        
        break;
    }
    case ARRDEF_KIND:
        if (this->globalStm)
        {
            string varia = child[1]->GetName();
            string asize = child[0]->GenDefCode();
            string globv = SymTable::InsertGlobal(varia);
            SymTable::InsertArray(varia);

            cout << globv << " = malloc " << asize << endl;
            tiggercode.push_back(StringFormat("%s = malloc %s\n",globv.c_str(),asize.c_str()));
        } 
        else
        {
            stringstream sin;
            string varia = child[1]->GetName();
            string asize = child[0]->GenDefCode();
            int asize_int;
            SymTable::InsertLocal(varia);
            SymTable::InsertArray(varia);
            sin<<asize;
            sin>>asize_int;
            if(!RegManager::CurrStack){
                cout<<"define local array var without stack"<<endl;
                exit(-1);
            }
            RegManager::CurrStack->AllocStack(varia,asize_int/4);
        }
        break;
    case SEQUEN_KIND:
        if (child[0] != nullptr)
            child[0]->GenCode();
        if (child[1] != nullptr)
            child[1]->GenCode();
    default: break;
    }
    return "";
}

string VariaExpNode::GenUseCode()
{
    //cout<<"VariaExpNode::GenUseCode"<<endl;
    string regvar,regaddr;
    int stackaddr;
    bool isexist=RegManager::CheckReg(varia);
    regaddr=RegManager::GetVarReg(varia);
    if(!isexist){
        if (SymTable::IsGlobal(varia))
        {
            if (SymTable::IsArray(varia))
            {
                string globv = SymTable::GlobalVar(varia);

                cout << "loadaddr " << globv << " " << regaddr << endl;
                tiggercode.push_back(StringFormat("loadaddr %s %s\n",globv.c_str(),regaddr.c_str()));
                
                return regaddr;
            }
            else
            {
                string globv = SymTable::GlobalVar(varia);

                cout << "load " << globv << " " << regaddr << endl;
                tiggercode.push_back(StringFormat("load %s %s\n",globv.c_str(),regaddr.c_str()));

                regvar=regaddr;
                return regvar;
            }
        }
        if (SymTable::IsLocal(varia))
        {
            stackaddr=RegManager::CurrStack->GetStack(varia);
            if(!RegManager::CurrStack){
                cout<<"CurrStack is NULL but variable is local"<<endl;
                exit(-1);
            }
            if (SymTable::IsArray(varia))
            {
                cout << "loadaddr " << stackaddr <<" "<<regaddr<< endl;
                tiggercode.push_back(StringFormat("loadaddr %d %s\n",stackaddr,regaddr.c_str()));

                return regaddr;
            }
            else
            {
                if(RegManager::CurrStack->StackBitMap[stackaddr]){
                    cout << "load " << stackaddr <<" "<<regaddr << endl;
                    tiggercode.push_back(StringFormat("load %d %s\n",stackaddr,regaddr.c_str()));
                }
                regvar=regaddr;
                return regvar;
            }
        }
    }
    else{
        if (SymTable::IsGlobal(varia) && !SymTable::IsArray(varia)){
            return regaddr+"[0]";
        }else{
            return regaddr;
        }
    }

    return regaddr + " error";
}

string VariaExpNode::GenDefCode()
{
    //cout<<"VariaExpNode::GenDefCode"<<endl;
    string regvar,regaddr;
    int stackaddr;
    bool isexist=RegManager::CheckReg(varia);
    regaddr=RegManager::GetVarReg(varia);
    if(!isexist){
        if (SymTable::IsGlobal(varia))
        {
            if (SymTable::IsArray(varia))
            {
                string globv = SymTable::GlobalVar(varia);

                cout << "loadaddr " << globv << " " << regaddr << endl;
                tiggercode.push_back(StringFormat("loadaddr %s %s\n",globv.c_str(),regaddr.c_str()));
                
                return regaddr;
            }
            else
            {
                string globv = SymTable::GlobalVar(varia);
                
                cout << "loadaddr " << globv << " " << regaddr << endl;
                tiggercode.push_back(StringFormat("loadaddr %s %s\n",globv.c_str(),regaddr.c_str()));
                
                regvar=regaddr+"[0]";
                return regvar;
            }
        }
        if (SymTable::IsLocal(varia))
        {
            stackaddr=RegManager::CurrStack->GetStack(varia);
            if(!RegManager::CurrStack){
                cout<<"CurrStack is NULL but variable is local"<<endl;
                exit(-1);
            }
            if (SymTable::IsArray(varia))
            {
                cout << "loadaddr " << stackaddr <<" "<<regaddr<< endl;
                tiggercode.push_back(StringFormat("loadaddr %d %s\n",stackaddr,regaddr.c_str()));
                return regaddr;
            }
            else
            {
                if(RegManager::CurrStack->StackBitMap[stackaddr]){
                    cout << "load " << stackaddr <<" "<<regaddr << endl;
                    tiggercode.push_back(StringFormat("load %d %s\n",stackaddr,regaddr.c_str()));
                }
                //cout << "load " << stackaddr <<" "<<regaddr << endl;
                regvar=regaddr;
                return regvar;
            }
        }
    }
    else{
        if (SymTable::IsGlobal(varia) && !SymTable::IsArray(varia)){
            return regaddr+"[0]";
        }else{
            return regaddr;
        }
    }

    return regaddr + " error";
}

string IntegExpNode::GenUseCode() { 
    string res;
    res = RegManager::AllocImmReg(value);
    if(res.compare("x0")){
        cout<<res << " = "<<value<<endl;
        tiggercode.push_back(StringFormat("%s = %d\n",res.c_str(),value));
    }
    return res;
}

string IntegExpNode::GenDefCode(){
    return std::to_string(value); 
}

string Oper1ExpNode::GenUseCode()
{
    string rval1 = child[0]->GenUseCode();
    string opstr = SymTable::OperaName(opera);
    return opstr + rval1;
}

string Oper2ExpNode::GenUseCode()
{
    string rval1 = child[0]->GenUseCode();
    string rval2;
    string opstr = SymTable::OperaName(opera);
    if(opera!=OP_ADD && opera!=OP_LT){//两者都不匹配
        rval2 = child[1]->GenUseCode();
    }else{//两者匹配其一
        if((ExpKind)(child[1]->GetKind())==INTEG_KIND){//+和*对于integer的特殊处理
            rval2 = child[1]->GenDefCode();
        }else{
            rval2 = child[1]->GenUseCode();
        }
    }
    return rval1 + opstr + rval2;
}

string CallfExpNode::GenUseCode()
{
    string regstr;
    for(int i=0;i<27;i++){
        RegManager::RegEnumStore((RegName)i);
    }
    string ret = "a0";
    Counter::ParamCountInit();

    cout << "call " << funct << endl;
    tiggercode.push_back(StringFormat("call %s\n",funct.c_str()));
    
    RegManager::InitReg();
    return ret;
}

string ArrayExpNode::GenDefCode(){
    //cout<<"ArrayExpNode::GenDefCode"<<endl;
    string res;
    string regvar,regaddr;
    int stackaddr;
    bool isexist=RegManager::CheckReg(varia);
    regaddr=RegManager::GetVarReg(varia);
    if(!isexist){
        if (SymTable::IsGlobal(varia))
        {
            string globv = SymTable::GlobalVar(varia);

            cout << "loadaddr " << globv << " " << regaddr << endl;
            tiggercode.push_back(StringFormat("loadaddr %s %s\n",globv.c_str(),regaddr.c_str()));    
        }
        if (SymTable::IsLocal(varia))
        {   
            if(!RegManager::CurrStack){
                cout<<"CurrStack is NULL but variable is local"<<endl;
                exit(-1);
            }   

            stackaddr=RegManager::CurrStack->GetStack(varia);
            
            cout << "loadaddr " << stackaddr <<" "<<regaddr<< endl;
            tiggercode.push_back(StringFormat("loadaddr %d %s\n",stackaddr,regaddr.c_str()));
        }
    }

    if((ExpKind)(child[0]->GetKind())==INTEG_KIND){
        //cout<<"here"<<endl;
        res=regaddr+"["+child[0]->GenDefCode()+"]";
    }else{
        //cout<<"there"<<endl;
        string tempchild,temparray;
        temparray=RegManager::AllocImmReg(1);//分配一个临时寄存器用来存新地址
        tempchild=child[0]->GenUseCode();

        cout<<temparray<<" = "<<regaddr<<" + "<<tempchild<<endl;
        tiggercode.push_back(StringFormat("%s = %s + %s\n",temparray.c_str(),regaddr.c_str(),tempchild.c_str()));
        
        res=temparray+"[0]";
    }
    return res;
}

string ArrayExpNode::GenUseCode(){
    //cout<<"ArrayExpNode::GenUseCode"<<endl;
    string res;
    string regvar,regaddr;
    int stackaddr;
    bool isexist=RegManager::CheckReg(varia);
    regaddr=RegManager::GetVarReg(varia);
    if(!isexist){
        if (SymTable::IsGlobal(varia))
        {
            string globv = SymTable::GlobalVar(varia);
            
            cout << "loadaddr " << globv << " " << regaddr << endl;    
            tiggercode.push_back(StringFormat("loadaddr %s %s\n",globv.c_str(),regaddr.c_str()));
        }
        if (SymTable::IsLocal(varia))
        {   
            if(!RegManager::CurrStack){
                cout<<"CurrStack is NULL but variable is local"<<endl;
                exit(-1);
            }   

            stackaddr=RegManager::CurrStack->GetStack(varia);
            
            cout << "loadaddr " << stackaddr <<" "<<regaddr<< endl;
            tiggercode.push_back(StringFormat("loadaddr %d %s\n",stackaddr,regaddr.c_str()));
        }
    }

    res=RegManager::AllocImmReg(1);//转存的临时寄存器

    if((ExpKind)(child[0]->GetKind())==INTEG_KIND){
        //cout<<"here"<<endl;
        string arrayidx=child[0]->GenDefCode();

        cout<<res<<" = "<<regaddr+"["+arrayidx+"]"<<endl;
        tiggercode.push_back(StringFormat(" %s = %s[%s]\n",res.c_str(),regaddr.c_str(),arrayidx.c_str()));
    
    }else{
        //cout<<"there"<<endl;
        string tempchild;
        tempchild=child[0]->GenUseCode();

        cout<<res<<" = "<<regaddr<<" + "<<tempchild<<endl;
        tiggercode.push_back(StringFormat("%s = %s + %s\n",res.c_str(),regaddr.c_str(),tempchild.c_str()));

        cout<<res<<" = "<<res+"[0]"<<endl;
        tiggercode.push_back(StringFormat("%s = %s[0]\n",res.c_str(),res.c_str()));
    }

    return res;
}
