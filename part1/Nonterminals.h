/*
 * @Date: 2020-06-15 18:46:46
 * @LastEditors: zyk
 * @LastEditTime: 2020-06-15 20:53:49
 * @FilePath: \compiler\Nonterminals.h
 */


#ifndef _NONTERMINALS_H
#define _NONTERMINALS_H

#include <string>

using namespace std;

enum yyntermtype {
    Program = 1000,
    CompUnit,
    Decl,
    FuncDef,
    ConstDecl,
    BType,
    ConstDefSeq,
    ConstDef,
    ConstInitVal,
    ConstArraySubSeq,
    ConstExp,
    ArraySubSeq,
    Exp,
    ConstInitValSeq,
    VarDecl,
    VarDefSeq,
    VarDef,
    InitVal,
    InitValSeq,
    Block,
    FuncFParams,
    FuncFParam,
    BlockItemSeq,
    BlockItem,
    Stmt,
    LVal,
    Cond,
    AddExp,
    LOrExp,
    Number,
    PrimaryExp,
    UnaryOp,
    UnaryExp,
    FuncRParams,
    MulExp,
    RelExp,
    EqExp,
    LAndExp
};

string NameOf(int type);

#endif