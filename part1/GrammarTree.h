#ifndef _GRAMMARTREE_H
#define _GRAMMARTREE_H

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "SymbolTable.h"

using namespace std;

struct ArrayInitVal;

typedef struct GrammarTreeNode {
  //---------------
  // Grammar info
  //---------------
  int line; // the number of its line
  int type; // bison自动生成的枚举常量，标记终结符/非终结符类型
  struct GrammarTreeNode *lchild; // lchild指向孩子节点
  struct GrammarTreeNode *rchild; // rchild指向兄弟节点
  union                           // the value of this grammar unit
  {
    char *string_value; // 用于StringConstant
    int int_value;      // 用于IntConstant
  };

  //---------------
  // semantic info
  //---------------

  // processing Expr
  bool isVoid;
  bool isVisited; // for debug
  bool is_constant_expr; // 若某Expr的子节点均为constant 则其本身也是constant
  int expr_value; // constant表达式的值可以在自底向上的语法分析中直接求出
  union {
    // 语义分析时节点可能具有的类型
    // scope vector
    LocalScope *local_scope;
    FormalScope *formal_scope;
    GlobalScope *global_scope;
    // scope entry (one line in vector)
    LocalScopeEntry *local_entry;
    FormalScopeEntry *formal_entry;
    GlobalScopeEntry *global_entry;

    vector<int> *dims; // 记录数组维度/下标列表
    vector<ArrayInitValue> *raw_dims;
    vector<ArrayInitValue> *raw_values; // 记录初始化列表
    ArrayInfo *array_info;//记录数组信息
  };
} GrammarTreeNode;

typedef struct GrammarTreeNode *GrammarTree;

/* Create GrammarTree Using Chile-Brother representation
 * name: the name of the grammar unit
 * num: the number of grammar unit in the varible parameter list
 */
GrammarTree CreateGrammarTree(int type, int num, ...);

/* Traverse GrammarTree Using Pre-Order
 * tree: the grammar tree
 * level: the number of the level
 */
void TraverseGrammarTree(GrammarTree gmtree, int level);
int BuildStmt(GrammarTree gmtree);
void BuildRParam(GrammarTree gmtree,vector<int> &paramidx);
int BuildExp(GrammarTree tree);
int Transform(GrammarTree gmtree);

#endif
