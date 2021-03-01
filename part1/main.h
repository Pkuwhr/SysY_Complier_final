/*
 * @Date: 2020-07-01 15:18:34
 * @LastEditors: zyk
 * @LastEditTime: 2020-07-15 21:42:49
 * @FilePath: /compiler/main.h
 */

#ifndef _MAIN_H
#define _MAIN_H

#include <iostream>
#include <cstdio>
#include <cstdarg>

#include "GrammarTree.h"
#include "parser.tab.h"
extern "C" {
    extern FILE *yyin;
}

using namespace std;

typedef enum CodeLineType { VARDEC, VARINIT, FUNCBEGIN, FUNCEND,MAINBEGIN,MAINEND, OTHER } CodeLineType;

typedef struct codeline{
    CodeLineType type;
    string code;
}codeline;

string StringFormat(const char *format, ...);
void AddNewCodeline(CodeLineType type, string codestr);
void DisplayCode(vector<codeline> code);
vector<codeline> ReFormCode(vector<codeline> code);

#endif // _MAIN_H