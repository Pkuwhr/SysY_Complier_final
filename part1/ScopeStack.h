/*
 * @Date: 2020-07-15 21:53:37
 * @LastEditors: zyk
 * @LastEditTime: 2020-07-26 20:15:46
 * @FilePath: /compiler/ScopeStack.h
 */
#ifndef _SCOPESTACK_H
#define _SCOPESTACK_H

#include "GrammarTree.h"
#include "SymbolTable.h"
#include "Nonterminals.h"


#include <string>
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

extern int smerror;

typedef vector<GrammarTree> Scope;

typedef vector<Scope > ScopeStack;

// 初始化作用域栈
void InitScopeStack(ScopeStack *stack);
// 销毁作用域栈
void DestroyScopeStack(ScopeStack *stack);
// 返回作用域栈的长度
int ScopeStackLength(ScopeStack *stack);
// 作用域的入栈
void ScopeStackPush(ScopeStack *stack, Scope scope);
// 添加新的符号
void AddNewSym(ScopeStack *stack, GrammarTree new_node);
// 作用域的出栈
void ScopeStackPop(ScopeStack *stack);
// 根据name查找符号表对应的符号
GrammarTree TraverseScopeStack(ScopeStack *stack, char *name);
// 查找对应的tree是否是constant元素
void CheckExprValue(GrammarTree tree);

#endif