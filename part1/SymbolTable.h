#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;

struct ArrayInitVal;
typedef ArrayInitVal *ArrayInitValue;
struct ArrayInfo;
struct GrammarTreeNode;
typedef struct GrammarTreeNode *GrammarTree;



//-----------------------------------------------------------------------------
// * 符号表定义
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// 局部作用域符号表
typedef struct LocalScopeEntry {
  char *name;
  bool is_const, is_array, is_block;
  union {
    int int_init_value;        // int变量/常量初值
    GrammarTree int_init_expr; // int变量初始表达式
    ArrayInfo *array_info;
    vector<LocalScopeEntry *> *embedded_scope; // 内嵌域符号表
  };
} LocalScopeEntry;

typedef vector<LocalScopeEntry *> LocalScope;


//-----------------------------------------------------------------------------
// 函数形参作用域符号表
typedef struct FormalScopeEntry {
  char *name;
  bool is_array;
  ArrayInfo *array_info; // 这里仅存储维度信息
} FormalScopeEntry;

typedef vector<FormalScopeEntry *> FormalScope;


//-----------------------------------------------------------------------------
// 全局作用域符号表
typedef struct GlobalScopeEntry {
  char *name;
  bool is_func, is_void, is_const, is_array;
  union {
    int int_init_value;        // int变量/常量初值
    GrammarTree int_init_expr; // int变量初始表达式
    int formal_count;          // 函数形参个数
    ArrayInfo *array_info;
  };
  FormalScope *formal_scope; // 函数形参域
  LocalScope *local_scope;   // 函数体域
} GlobalScopeEntry;

typedef vector<GlobalScopeEntry *> GlobalScope;



//-----------------------------------------------------------------------------
// * 符号表处理
//-----------------------------------------------------------------------------

// ! 所有 add xxx to xxx 函数都需要注意目标符号表为空的情况 ！！！
// 把 local_scope 中的 entry 填入 global_scope 中 注意 **local_scope 中不能有 Block**
GlobalScope *AddLocalIntoGlobal(GlobalScope *global_scope, LocalScope *local_scope);

// 把local_scope放入另一个local_scope中
LocalScope *AddLocalIntoLocal(LocalScope *head, LocalScope *tail);

// 把entry放入scope中 且返回加入后的scope指针
GlobalScope *AddEntryIntoGlobalScope(GlobalScope *scope, GlobalScopeEntry *entry);

FormalScope *AddEntryIntoFormalScope(FormalScope *scope, FormalScopeEntry *entry);

LocalScope *AddEntryIntoLocalScope(LocalScope *scope, LocalScopeEntry *entry);

// 为local_scope中的entry设置统一的isConst标记  返回添加好的scope指针
LocalScope *AttachTypeToLocalScope(LocalScope *scope, bool _is_const);




//-----------------------------------------------------------------------------
// * 数组信息处理 (这里定义的是 .y 中用到的函数)
//-----------------------------------------------------------------------------

// 1. 构造初始化向量
//--------------
// 把value加入到vector中 若 init = null 则需要新建一个vector
vector<ArrayInitValue> *NewInitValue(vector<ArrayInitValue> *values, int type, int value, GrammarTree expr);

// 把tail中的元素连接到head后面 
// ! head tail都可以为Null
// ! add_par = true 时 需要在前后添加 { 和 }
vector<ArrayInitVal *> *AppendInitValue(vector<ArrayInitValue> *head, vector<ArrayInitValue> *tail, bool add_par);


// 2. 构造 dims 维度向量
//------------------
// 把int值放入dims vector中 若dims = null 则需要新建一个vector
vector<int> *AppendDim(vector<int> *dims, int d);
// 给 dims 加入一个0作为第一维长度
vector<int> *InsertDim1(vector<int> *dims);


// 3. 组装一个ArrayInfo
ArrayInfo *NewArrayInfo(vector<int> *_dims, vector<ArrayInitValue> *_raw_values);




//-----------------------------------------------------------------------------
// * 新建各类表项
//-----------------------------------------------------------------------------

// 1. 新建一个 local_entry

// array / const array
LocalScopeEntry *NewLocalArrayEntry(char *_name, bool _is_const, ArrayInfo *_array_info);
// int / const int
LocalScopeEntry *NewLocalIntEntry(char *_name, bool _is_const, int _int_init_value, GrammarTree _int_init_expr);
// block
LocalScopeEntry *NewEmbeddedScopeEntry(LocalScope *_embedded_scope);


// 2. 新建一个 formal_entry
FormalScopeEntry *NewFormalEntry(char *_name, bool _is_array, ArrayInfo *_array_info);

// 3. 新建一个 global_entry
GlobalScopeEntry *NewGlobalIntEntry(char *_name, bool _is_const, int _int_init_value, GrammarTree _int_init_expr);

GlobalScopeEntry *NewGlobalArrayEntry(char *_name, bool _is_const, ArrayInfo *_array_info);

GlobalScopeEntry *NewFunctionEntry(char *_name, bool _is_void, int _formal_count, FormalScope *_formal_scope, LocalScope *_local_scope);


//-----------------------------------------------------------------------------
// * 打印符号表 / 数组信息
//-----------------------------------------------------------------------------

void DisplayArrayInfo(ArrayInfo *array_info);

//int ArraySpaceSize(ArrayInfo *array_info);

void InitArraydims(vector<int> *dims,vector<ArrayInitValue> *raw_dims);

int ArraySubSeqIdx(vector<int> indices,ArrayInfo *array_info);

void DisplayLocalScope(LocalScope *local_symtable);

void DisplayFormalScope(FormalScope *formal_symtable);

void DisplayGlobalScope(GlobalScope *global_symtable);



#endif // _SYMBOLTABLE_H
