#ifndef _ARRAYINFO_H
#define _ARRAYINFO_H

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "GrammarTree.h"
#include "ScopeStack.h"

using namespace std;

// 标识InitVal的类型
typedef enum InitValType { Begin, Value, Expr, End } InitValType;

// 存储数组初始化语句的内容
// 可能的类型：
// 1. Begin: '{'
// 2. End: '}'
// 3. Value: ConstInt/ConstExp
// 4. Exp: other Exp
typedef struct ArrayInitVal {
  InitValType type;
  union {
    int value;        // 遇到正常数字时 添加其到vector中
    GrammarTree expr; // 未计算的表达式
    // 遇到Begin或End时 这里不需要存储值
  };
} ArrayInitVal;

typedef ArrayInitVal *ArrayInitValue;
typedef vector<ArrayInitValue> *ArrayInitValues;

// 存储数组维度、初值等信息
typedef struct ArrayInfo {
  vector<int> *dims;                   // 数组维度向量
  vector<ArrayInitValue> *raw_values;  // 数组初始化表达式
  vector<ArrayInitValue> *init_values; // 数组初值(长度和数组相同)

  int size(); // 获取数组的维数
  int spacesize();
  void init(); // 根据raw_values计算init_values
  ArrayInitVal get_element(vector<int> indices); // 获取指定的值
  ArrayInitVal get_element_by_index(int index); // 获取指定的值
  int dim2idx(vector<int> indices);
  void DisplayRawValues();
  void DisplayDims();
} ArrayInfo;

bool initdims(vector<int> *dims,vector<ArrayInitValue> *raw_dims);
#endif
