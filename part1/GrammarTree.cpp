/*
 * @Date: 2020-06-13 17:07:18
 * @LastEditors: zyk
 * @LastEditTime: 2020-07-15 21:29:54
 * @FilePath: /compiler/GrammarTree.cpp
 */


#include"GrammarTree.h"
#include"Nonterminals.h"
#include"ArrayInfo.h"
#include"ScopeStack.h"
#include"main.h"



extern int tuple_trigger;

extern FILE *yyin;
extern int yylineno;

extern "C" {
  extern char *yytext;
  extern int yyparse();
}
extern vector<codeline> programcode;

GrammarTree CreateGrammarTree(int type, int num, ...) {
    char *value_buffer; // 存放标识符名称/常量的值
    va_list variables; // 可变参数列表
    GrammarTree current = (GrammarTree) malloc(
            sizeof(GrammarTreeNode));  // 当前输入所有节点的父节点
    current->type = type; // 标识父节点类型
    //printf("%s,create begin\n",NameOf(current->type).c_str());
    current->lchild=NULL;
    current->rchild=NULL;
    va_start(variables, num);  // 初始化可变参数列表
    if (num > 0) // 将num后面的参数设置为current的子节点
    {
        //printf("here1\n");
        GrammarTree tmp = va_arg(variables, GrammarTree); // 获取列表中的第一个节点
        current->lchild = tmp; // 将第一个节点设置为current的右子节点（即孩子节点）
        current->line = tmp->line;      // Current grammar unit's line number is equal to its left child's
        for (int i = 0; i < num - 1; i++) // 处理剩下的(num - 1)个节点，将他们设置为第一个节点的兄弟
        {                               // Brothers
            //printf("here%d\n",i+10);
            tmp->rchild = va_arg(variables, GrammarTree);
            tmp = tmp->rchild; // 可以视为将指针移动到链表尾
        }
        tmp->rchild = NULL; // make sure the end flag is set correctly
        // Modify the line number when reduce using rule "Stmt->SimpleStmt" and SimpleStmt is Epsilon
         if (current->lchild->type == ConstInitVal  && current->lchild->line == -1)
             current->line = current->lchild->rchild->line;
    } else {   // 处理终结符或空规则
        current->line = va_arg(variables,
        int); // 行号

        switch (current->type) // 此变量正是bison中定义的token和type
        {
            case T_Identifier:
            case T_StringConstant:
                // 将标识符名称放入string_value
                value_buffer = (char *) malloc(sizeof(char) * strlen(yytext));
                strcpy(value_buffer, yytext);
                current->string_value = value_buffer;
                break;
            case T_IntConstant:
                // 计算不同进制的常量值
                if (yytext[0] == '0' && (yytext[1] == 'x' || yytext[1] == 'X')) {
                    current->int_value = (int) strtol(yytext, NULL, 16);
                } else if (yytext[0] == '0') {
                    current->int_value = (int) strtol(yytext, NULL, 8);
                } else {
                    current->int_value = (int) strtol(yytext, NULL, 10);
                }
                break;
            default:
                break;
        }
    }
    printf("%s,create successfully\n",NameOf(current->type).c_str());
    return current;
}

void TraverseGrammarTree(GrammarTree gmtree, int level) {
    if (gmtree != NULL) {
        for (int i = 0; i < level; i++)
            printf("    ");
        if (gmtree->line != -1) {
            // Not void rule
            cout << NameOf(gmtree->type);
            if (gmtree->type == T_Identifier || gmtree->type == T_StringConstant)
                printf(": %s\n", gmtree->string_value);
            else if (gmtree->type == T_IntConstant)
                printf(": %d\n", gmtree->int_value);
            else
                printf(" (%d)\n", gmtree->line);
            
            if(gmtree->is_constant_expr){
                for (int i = 0; i < level; i++){
                    printf("    ");
                }    
                printf("expr_value: %d\n",gmtree->expr_value);
            }
        } else
            printf("%s (Epsilon)\n", NameOf(gmtree->type).c_str());
        TraverseGrammarTree(gmtree->lchild, level + 1);
        TraverseGrammarTree(gmtree->rchild, level);
    }
}

int p_idx,t_idx,T_idx,l_idx;
int breakl,continuel;//记录遇到这两个语句需要跳转的label
vector<char *> t_sym,T_sym,p_sym;
vector<int> uninit;
FormalScope CurFuncFormal;

typedef struct varinfo{
    int idx;
    char type;//保存一个字符，p/t/T
}varinfo;
int MatchIdentinTSym(GrammarTree gmtree){
    for(int i=0;i<T_sym.size();i++){
        if(!strcmp(gmtree->string_value,T_sym[i])){
            return i;
        }
    }
    return -1;
}
int MatchIdentinpSym(GrammarTree gmtree){
    for(int i=0;i<p_sym.size();i++){
        if(!strcmp(gmtree->string_value,p_sym[i])){
            return i;
        }
    }
    return -1;
}
int MatchIdentintSym(GrammarTree gmtree){
    return -1;
}
GrammarTree MatchIdentinFormalSym(GrammarTree gmtree){
    GrammarTree temp=(GrammarTree)malloc(sizeof(GrammarTreeNode));
    FormalScope::iterator it;
    for(it=CurFuncFormal.begin();it!=CurFuncFormal.end();it++){
        if(!strcmp((*it)->name,gmtree->string_value)){
            temp->string_value=((*it)->name);
            if((*it)->is_array){
                temp->array_info=(*it)->array_info;
            }
            return temp;
        }
    }
    return NULL;
}
varinfo MatchIdentinSym(GrammarTree gmtree){
    varinfo res;
    if(MatchIdentinTSym(gmtree)!=-1){
        res.type='T';
        res.idx=MatchIdentinTSym(gmtree);
    }else if(MatchIdentinpSym(gmtree)!=-1){
        res.type='p';
        res.idx=MatchIdentinpSym(gmtree);
    }else{
        res.type='t';
        res.idx=MatchIdentintSym(gmtree);
    }
    return res;
}
//最后产出的都是临时变量或者常数
int BuildExp(GrammarTree tree){
    //可以做的优化：动态规划实现DAG
    int t0=-2,t1=-2,t2=-2;
    // 判断 tree 是否为空
    if (!tree){
      return -2;//返回-2代表出现错误，或者结束；
    }
    // 判断是不是一个常数
    CheckExprValue(tree);
    if(tree->is_constant_expr){
        return -1;//返回-1代表不需要生成临时变量，直接使用立即数
    }
    // * Exp 'op' Exp 形式
    if (tree->lchild->type == Exp) {
        t0=t_idx++;
        printf("Var t%d\n",t_idx);
        AddNewCodeline(VARDEC,StringFormat("Var t%d\n",t_idx));
        // 检查两个子 Exp 节点
        t1=BuildExp(tree->lchild);
        t2=BuildExp(tree->lchild->rchild->rchild);
        if(t1==-1){
            if(t2==-1){
                printf("t%d = %d %c %d\n",t0,tree->lchild->expr_value,tree->lchild->rchild->type,tree->lchild->rchild->rchild->expr_value);
                AddNewCodeline(OTHER,StringFormat("t%d = %d %c %d\n",t0,tree->lchild->expr_value,tree->lchild->rchild->type,tree->lchild->rchild->rchild->expr_value));
            }else{
                printf("t%d = %d %c t%d\n",t0,tree->lchild->expr_value,tree->lchild->rchild->type,t2);
                AddNewCodeline(OTHER,StringFormat("t%d = %d %c t%d\n",t0,tree->lchild->expr_value,tree->lchild->rchild->type,t2));
            }
        }else{
            if(t2==-1){
                printf("t%d = t%d %c %d\n",t0,t1,tree->lchild->rchild->type,tree->lchild->rchild->rchild->expr_value);
                AddNewCodeline(OTHER,StringFormat("t%d = t%d %c %d\n",t0,t1,tree->lchild->rchild->type,tree->lchild->rchild->rchild->expr_value));
            }else{
                printf("t%d = t%d %c t%d\n",t0,t1,tree->lchild->rchild->type,t2);
                AddNewCodeline(OTHER,StringFormat("t%d = t%d %c t%d\n",t0,t1,tree->lchild->rchild->type,t2));
            }
        }
        //printf("t%d = t%d %c t%d\n",t0,t1,tree->lchild->rchild->type,t2);
        return t0;
    // * ( Exp ) 形式
    } else if (tree->lchild->type == '(') {
        // '(' Exp ')'
        return BuildExp(tree->lchild->rchild);
    // * 'op' Exp 形式
    } else if (tree->lchild->rchild && tree->lchild->rchild->type == Exp) {
        t0=t_idx++;
        printf("Var t%d\n",t_idx);
        AddNewCodeline(VARDEC,StringFormat("Var t%d\n",t_idx));
        t1=BuildExp(tree->lchild->rchild);
        printf("t%d = %c t%d\n",t0,tree->lchild->type,t1);
        AddNewCodeline(OTHER,StringFormat("t%d = %c t%d\n",t0,tree->lchild->type,t1));
        return t0;
    // * 整型常量
    } else if (tree->lchild->type == T_IntConstant) {
        return -1;
    }
    // * 左值
    else if(tree->lchild->type == LVal){
        GrammarTree temp;
        if(!tree->lchild->is_constant_expr){
            t0=t_idx++;
            printf("Var t%d\n",t_idx);
            AddNewCodeline(VARDEC,StringFormat("Var t%d\n",t_idx));
            varinfo tempTp;
            int tempidx;
            vector<int> tempdims;
            tempTp=MatchIdentinSym(tree->lchild->lchild);
            if(tree->lchild->lchild->rchild){
                initdims(&tempdims,tree->lchild->lchild->rchild->raw_dims);
                if(tree->lchild->lchild->array_info){
                    tempidx=tree->lchild->lchild->array_info->dim2idx(tempdims);
                }else{
                    temp=MatchIdentinFormalSym(tree->lchild->lchild);
                    if(!temp){
                        tempidx=-1;
                    }else{
                        tempidx=temp->array_info->dim2idx(tempdims);
                    }
                }
            }else{
                tempdims.clear();
                tempidx=-1;
            }
            //tempidx=tree->lchild->lchild->array_info->dim2idx(tempdims);
            if(tempidx!=-1){
                printf("t%d = %c%d[%d]\n",t0,tempTp.type,tempTp.idx,4*tempidx);
                AddNewCodeline(OTHER,StringFormat("t%d = %c%d[%d]\n",t0,tempTp.type,tempTp.idx,4*tempidx));
            }else{
                printf("t%d = %c%d\n",t0,tempTp.type,tempTp.idx);
                AddNewCodeline(OTHER,StringFormat("t%d = %c%d\n",t0,tempTp.type,tempTp.idx));
            }
            return t0;
        }else{//发现是-1的时候，表明是一个立即数，不需要临时变量
            return -1;
        }
    // * 函数
    }else if(tree->lchild->type==T_Identifier){
        GrammarTree funcident;
        GrammarTree paramsRnode;
        vector<int> paramidx;
        paramidx.clear();
        if(!tree->lchild->isVoid){
            t0=t_idx++;
            printf("Var t%d\n",t_idx);
            AddNewCodeline(VARDEC,StringFormat("Var t%d\n",t_idx));
        }

        if(tree->lchild->rchild){
            paramsRnode=tree->lchild->rchild;
            BuildRParam(paramsRnode,paramidx);
            //printf("size :%d\n",paramidx.size());
            for(int i=0;i<paramidx.size();i++){
                printf("param t%d\n",paramidx[i]);
                AddNewCodeline(OTHER,StringFormat("param t%d\n",paramidx[i]));
            }
        }
        if(!tree->lchild->isVoid){
            printf("t%d = call f_%s\n",t0,tree->lchild->string_value);
            AddNewCodeline(OTHER,StringFormat("t%d = call f_%s\n",t0,tree->lchild->string_value));
        }else{
            printf("call f_%s\n",tree->lchild->string_value);
            AddNewCodeline(OTHER,StringFormat("call f_%s\n",tree->lchild->string_value));
        }
        return t0;
    }
    return -2;
}
void BuildWhile(GrammarTree gmtree){
    //printf("BUILD_WHILE_BEGIN\n");
    int tempt;
    int templnext,templcur;
    templcur=l_idx++;
    continuel=templcur;
    printf("l%d:\n",l_idx);
    AddNewCodeline(OTHER,StringFormat("l%d:\n",l_idx));
    templnext=l_idx++;
    breakl=templnext;
    tempt=BuildExp(gmtree->lchild->rchild);
    printf("if t%d == 0 goto l%d\n",tempt,templnext);
    AddNewCodeline(OTHER,StringFormat("if t%d == 0 goto l%d\n",tempt,templnext));
    //printf("递归\n");
    Transform(gmtree->lchild->rchild->rchild);
    printf("goto l%d\n",templcur);
    AddNewCodeline(OTHER,StringFormat("goto l%d\n",templcur));
    printf("l%d:\n",templnext);
    AddNewCodeline(OTHER,StringFormat("l%d:\n",templnext));
    //printf("BUILD_WHILE_END\n");
}
void BuildIfNoElse(GrammarTree gmtree){
    int tempt;
    int templnext;
    templnext=l_idx++;
    tempt=BuildExp(gmtree->lchild->rchild);
    printf("if t%d == 0 goto l%d\n",tempt,templnext);
    AddNewCodeline(OTHER,StringFormat("if t%d == 0 goto l%d\n",tempt,templnext));
    Transform(gmtree->lchild->rchild->rchild);
    printf("l%d:\n",templnext);
    AddNewCodeline(OTHER,StringFormat("l%d:\n",templnext));
}
void BuildIfElse(GrammarTree gmtree){
    int tempt;
    int templelse,templnext;
    templelse=l_idx++;
    templnext=l_idx++;
    tempt=BuildExp(gmtree->lchild->rchild);
    printf("if t%d == 0 goto l%d\n",tempt,templelse);
    AddNewCodeline(OTHER,StringFormat("if t%d == 0 goto l%d\n",tempt,templelse));
    Transform(gmtree->lchild->rchild->rchild);
    printf("goto l%d\n",templnext);
    AddNewCodeline(OTHER,StringFormat("goto l%d\n",templnext));
    printf("l%d:\n",templelse);
    AddNewCodeline(OTHER,StringFormat("l%d:\n",templelse));
    Transform(gmtree->lchild->rchild->rchild->rchild->rchild);
    printf("l%d:\n",templnext);
    AddNewCodeline(OTHER,StringFormat("l%d:\n",templnext));
}
void BuildRParam(GrammarTree gmtree,vector<int> &paramidx){
    int tempt;
    if(!gmtree){
        return;
    }
    if(gmtree->type==Exp){
        tempt=BuildExp(gmtree);
        if(tempt==-1){
            tempt=t_idx++;
            printf("Var t%d\n",tempt);
            AddNewCodeline(VARDEC,StringFormat("Var t%d\n",tempt));
            printf("t%d = %d\n",tempt,gmtree->expr_value);
            AddNewCodeline(OTHER,StringFormat("t%d = %d\n",tempt,gmtree->expr_value));
        }
        paramidx.push_back(tempt);
    }
    BuildRParam(gmtree->lchild,paramidx);
    BuildRParam(gmtree->rchild,paramidx);
}

int Transform(GrammarTree gmtree){
    if (gmtree != NULL) {
        //printf("TRANSFORM TYPE:%s\n",NameOf(gmtree->type).c_str());
        if (gmtree->line != -1) {
            switch(gmtree->type){
                //初始化一些变量
                case Program:{
                    printf("This eeyore program is created by HaoRan Wang:\n\n\n");
                    p_idx=0;
                    t_idx=0;
                    T_idx=0;
                    l_idx=0;
                    breakl=-1;
                    continuel=-1;
                    T_sym.clear();
                    p_sym.clear();
                    t_sym.clear();
                    uninit.clear();
                    Transform(gmtree->lchild);
                    Transform(gmtree->rchild);
                    break;
                }
                case ConstDef:{
                    bool is_array=false;
                    ArrayInitVal initval;
                    if(gmtree->lchild->rchild&&gmtree->lchild->rchild->rchild==NULL){
                        printf("Var T%d\n",T_idx);
                        AddNewCodeline(VARDEC,StringFormat("Var T%d\n",T_idx));
                        T_idx++;
                        T_sym.push_back(gmtree->lchild->string_value);
                    }else{
                        is_array=true;
                        printf("Var %d T%d\n",gmtree->lchild->array_info->spacesize(),T_idx);
                        AddNewCodeline(VARDEC,StringFormat("Var %d T%d\n",gmtree->lchild->array_info->spacesize(),T_idx));
                        T_idx++;
                        T_sym.push_back(gmtree->lchild->string_value);
                    }

                    //赋值语句：
                    if(is_array){
                        for(int i=0;i*4<gmtree->lchild->array_info->spacesize();i++){
                            initval=gmtree->lchild->array_info->get_element_by_index(i);
                            if(initval.value!=0){
                                varinfo tempTp;
                                tempTp=MatchIdentinSym(gmtree->lchild);
                                printf("%c%d[%d] = %d\n",tempTp.type,tempTp.idx,4*i,initval.value);
                                AddNewCodeline(VARINIT,StringFormat("%c%d[%d] = %d\n",tempTp.type,tempTp.idx,4*i,initval.value));
                            }
                        }
                    }else{
                        varinfo tempTp;
                        tempTp=MatchIdentinSym(gmtree->lchild);
                        printf("%c%d = %d\n",tempTp.type,tempTp.idx,initval.value);
                        AddNewCodeline(VARINIT,StringFormat("%c%d = %d\n",tempTp.type,tempTp.idx,initval.value));
                    }
                    break;
                }
                case VarDef:{
                    bool is_array=false;
                    ArrayInitVal initval;
                    if(gmtree->lchild&&gmtree->lchild->rchild==NULL){
                        printf("Var T%d\n",T_idx);
                        AddNewCodeline(VARDEC,StringFormat("Var T%d\n",T_idx));
                        T_idx++;
                        T_sym.push_back(gmtree->lchild->string_value);
                    }else if(gmtree->lchild->rchild&&gmtree->lchild->rchild->type==ConstArraySubSeq){
                        is_array=true;
                        printf("Var %d T%d\n",gmtree->lchild->array_info->spacesize(),T_idx);
                        AddNewCodeline(VARDEC,StringFormat("Var %d T%d\n",gmtree->lchild->array_info->spacesize(),T_idx));
                        T_idx++;
                        T_sym.push_back(gmtree->lchild->string_value);
                    }else if(gmtree->lchild->rchild&&gmtree->lchild->rchild->type==InitVal){
                        printf("Var T%d\n",T_idx);
                        AddNewCodeline(VARDEC,StringFormat("Var T%d\n",T_idx));
                        T_idx++;
                        T_sym.push_back(gmtree->lchild->string_value);
                    }else{
                        is_array=true;
                        printf("Var %d T%d\n",gmtree->lchild->array_info->spacesize(),T_idx);
                        AddNewCodeline(VARDEC,StringFormat("Var %d T%d\n",gmtree->lchild->array_info->spacesize(),T_idx));
                        T_idx++;
                        T_sym.push_back(gmtree->lchild->string_value);
                    }

                    //初始化语句：
                    if(is_array){
                        gmtree->lchild->array_info->DisplayRawValues();
                        for(int i=0;i*4<gmtree->lchild->array_info->spacesize();i++){
                            initval=gmtree->lchild->array_info->get_element_by_index(i);
                            //printf("type is: %d\n",initval.type);
                            if(initval.type==Expr){
                                //printf("working?/n");
                                int tempt;
                                varinfo tempTp;
                                tempt=BuildExp(initval.expr);
                                tempTp=MatchIdentinSym(gmtree->lchild);
                                printf("%c%d[%d] = t%d\n",tempTp.type,tempTp.idx,4*i,tempt);
                                AddNewCodeline(VARINIT,StringFormat("%c%d[%d] = t%d\n",tempTp.type,tempTp.idx,4*i,tempt));
                            }else if(initval.value!=0){
                                varinfo tempTp;
                                tempTp=MatchIdentinSym(gmtree->lchild);
                                printf("%c%d[%d] = %d\n",tempTp.type,tempTp.idx,4*i,initval.value);
                                AddNewCodeline(VARINIT,StringFormat("%c%d[%d] = %d\n",tempTp.type,tempTp.idx,4*i,initval.value));
                            }
                        }
                    }else{
                        if(gmtree->lchild->rchild){
                            initval = *((*gmtree->lchild->rchild->raw_values)[0]);
                            if(initval.type==Expr){
                                int tempt;
                                varinfo tempTp;
                                tempt=BuildExp(initval.expr);
                                tempTp=MatchIdentinSym(gmtree->lchild);
                                printf("%c%d = t%d\n",tempTp.type,tempTp.idx,tempt);
                                AddNewCodeline(VARINIT,StringFormat("%c%d = t%d\n",tempTp.type,tempTp.idx,tempt));
                            }else{
                                varinfo tempTp;
                                tempTp=MatchIdentinSym(gmtree->lchild);
                                printf("%c%d = %d\n",tempTp.type,tempTp.idx,initval.value);
                                AddNewCodeline(VARINIT,StringFormat("%c%d = %d\n",tempTp.type,tempTp.idx,initval.value));
                            }
                        }
                    }
                    break;
                }
                case FuncDef:{
                    //处理Formal域和block中变量匹配的问题
                    CurFuncFormal.clear();
                    if(gmtree->global_entry->formal_scope){
                        CurFuncFormal=*(gmtree->global_entry->formal_scope);
                    }
                    //将p_idx归零
                    p_idx=0;
                    //打印函数头
                    printf("f_%s [%d]\n",gmtree->global_entry->name,gmtree->global_entry->formal_count);
                    if(strcmp(gmtree->global_entry->name,"main")){
                        AddNewCodeline(FUNCBEGIN,StringFormat("f_%s [%d]\n",gmtree->global_entry->name,gmtree->global_entry->formal_count));
                    }else{
                        AddNewCodeline(MAINBEGIN,StringFormat("f_%s [%d]\n",gmtree->global_entry->name,gmtree->global_entry->formal_count));
                    }
                    //AddNewCodeline(FUNCBEGIN,StringFormat("f_%s [%d]\n",gmtree->global_entry->name,gmtree->global_entry->formal_count));
                    //扫描型参符号表，建立符号和eeyore变量对应表
                    if(gmtree->global_entry->formal_count!=0&&gmtree->lchild->rchild->rchild){
                        GrammarTree paramsnode=gmtree->lchild->rchild->rchild;
                        vector<FormalScopeEntry *>::iterator it;
                        for(it=paramsnode->formal_scope->begin();it!=paramsnode->formal_scope->end();it++){
                            p_sym.push_back((*it)->name);
                        }
                    }
                    Transform(gmtree->lchild);
                    Transform(gmtree->rchild);
                    if(gmtree->global_entry->is_void){
                        printf("return\n");
                        AddNewCodeline(OTHER,StringFormat("return\n"));
                    }else{
                        printf("return 0\n");
                        AddNewCodeline(OTHER,StringFormat("return 0\n"));
                    }
                    printf("end f_%s\n",gmtree->global_entry->name);
                    if(strcmp(gmtree->global_entry->name,"main")){
                        AddNewCodeline(FUNCEND,StringFormat("end f_%s\n",gmtree->global_entry->name));
                    }else{
                        AddNewCodeline(MAINEND,StringFormat("end f_%s\n",gmtree->global_entry->name));
                    }
                    //AddNewCodeline(FUNCEND,StringFormat("end f_%s\n",gmtree->global_entry->name));
                    p_sym.clear();
                    break;
                }
                case Stmt:{
                    //printf("BUILD_STMT_SWITCH\n");
                    if(gmtree->lchild->type==Exp){
                        BuildExp(gmtree->lchild);
                    }
                    else if(gmtree->lchild->type==LVal){
                        varinfo tempTp;
                        int tempidx;
                        vector<int> tempdims;
                        tempTp=MatchIdentinSym(gmtree->lchild->lchild);
                        if(gmtree->lchild->lchild->rchild){
                            initdims(&tempdims,gmtree->lchild->lchild->rchild->raw_dims);
                            tempidx=gmtree->lchild->lchild->array_info->dim2idx(tempdims);
                        }else{
                            tempdims.clear();
                            tempidx=-1;
                        }
                        if(tempidx!=-1){
                            printf("%c%d[%d] = t%d\n",tempTp.type,tempTp.idx,4*tempidx,BuildExp(gmtree->lchild->rchild->rchild));
                            AddNewCodeline(VARINIT,StringFormat("%c%d[%d] = t%d\n",tempTp.type,tempTp.idx,4*tempidx,BuildExp(gmtree->lchild->rchild->rchild)));
                        }else{
                            printf("%c%d = t%d\n",tempTp.type,tempTp.idx,BuildExp(gmtree->lchild->rchild->rchild));
                            AddNewCodeline(VARINIT,StringFormat("%c%d = t%d\n",tempTp.type,tempTp.idx,BuildExp(gmtree->lchild->rchild->rchild)));
                        }
                    }
                    else if(gmtree->lchild->type==T_If){
                        if(gmtree->lchild->rchild->rchild->rchild){
                            BuildIfElse(gmtree);
                        }
                        else{
                            BuildIfNoElse(gmtree);
                        }
                    }
                    else if(gmtree->lchild->type==T_While){
                        //printf("BUILD_WHILE\n");
                        int tempb,tempc;
                        tempb=breakl;
                        tempc=continuel;
                        BuildWhile(gmtree);
                        breakl=tempb;
                        continuel=tempc;
                    }
                    else if(gmtree->lchild->type==T_Break){
                        printf("goto l%d\n",breakl);
                        AddNewCodeline(OTHER,StringFormat("goto l%d\n",breakl));
                    }
                    else if(gmtree->lchild->type==T_Continue){
                        printf("goto l%d\n",continuel);
                        AddNewCodeline(OTHER,StringFormat("goto l%d\n",continuel));
                    }
                    else if(gmtree->lchild->type==T_Return){
                        if(gmtree->lchild->rchild){
                            int tempt;
                            tempt=BuildExp(gmtree->lchild->rchild);
                            if(tempt==-1){
                                printf("return %d\n",gmtree->lchild->rchild->expr_value);
                                AddNewCodeline(OTHER,StringFormat("return %d\n",gmtree->lchild->rchild->expr_value));
                            }else{
                                printf("return t%d\n",tempt);
                                AddNewCodeline(OTHER,StringFormat("return t%d\n",tempt));
                            }
                        }else{
                            printf("return\n");
                            AddNewCodeline(OTHER,StringFormat("return\n"));
                        }
                    }else{
                        Transform(gmtree->lchild);
                        Transform(gmtree->rchild);
                    }
                    break;
                }
                default:{
                    Transform(gmtree->lchild);
                    Transform(gmtree->rchild);
                    break;
                }
            }
        }
    }
    return 0;
}