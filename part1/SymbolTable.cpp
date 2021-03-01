#include "SymbolTable.h"
#include "ArrayInfo.h"

#define GREEN printf("\u001b[32m");
#define YELLOW printf("\u001b[33m");
#define BLUE printf("\u001b[34m");
#define RESET printf("\u001b[0m");

extern ArrayInitVal ZERO, LEFT, RIGHT;

GlobalScope *AddLocalIntoGlobal(GlobalScope *global_scope,
                                LocalScope *local_scope) {
  if (global_scope == nullptr) {
    global_scope = new vector<GlobalScopeEntry *>;
    if (!global_scope) {
      puts("Error: Cannot malloc space for GlobalScope vector!!");
      exit(-1);
    }
    global_scope->clear();
  }

  for (LocalScope::iterator it = local_scope->begin(); it != local_scope->end();
       it++) {
    GlobalScopeEntry *global_entry =
        (GlobalScopeEntry *)malloc(sizeof(GlobalScopeEntry));
    if (!global_entry) {
      puts("Error: Cannot malloc space for GlobalScopeEntry!!");
      exit(-1);
    }
    
    char *temp = (char *)malloc(sizeof((*it)->name));
    if (!temp) {
      puts("Error: Cannot malloc space for name string!!");
      exit(-1);
    }
    strcpy(temp, (*it)->name);
    global_entry->name = temp;

    // it 不是函数
    global_entry->formal_scope = nullptr;
    global_entry->local_scope = nullptr;

    global_entry->is_const = (*it)->is_const;
    global_entry->is_func = false;
    global_entry->is_void = false;
    global_entry->is_array = (*it)->is_array;

    if (global_entry->is_array)
      global_entry->array_info = (*it)->array_info;
    if (global_entry->is_const && !global_entry->is_array)
      global_entry->int_init_value = (*it)->int_init_value;
    if (!global_entry->is_const && !global_entry->is_array)
      global_entry->int_init_expr = (*it)->int_init_expr;
    global_scope->push_back(global_entry);
  }
  /*
  for (LocalScope::iterator it = local_scope->begin(); it != local_scope->end();
       it++) {
    free(*it);
  }
  local_scope->clear();
  free(local_scope);
  */
  return global_scope;
}

// 把local_scope放入另一个local_scope中
LocalScope *AddLocalIntoLocal(LocalScope *head, LocalScope *tail) {
  // 判断 head 和 tail 是否为 NULL
  if (head == nullptr && tail == nullptr)
    return nullptr;

  else if (head == nullptr)
    return tail;

  else if (tail == nullptr)
    return head;

  // head != null and tail != null

  for (LocalScope::iterator it = tail->begin(); it != tail->end(); it++) {
    // add *it into head
    head->push_back(*it);
  }
  return head;
}

GlobalScope *AddEntryIntoGlobalScope(GlobalScope *scope,
                                     GlobalScopeEntry *entry) {
  if (!entry)
    return scope;
  if (!scope) {
    scope = new vector<GlobalScopeEntry *>(1);
    if (!scope) {
      puts("Error: Cannot malloc space for GlobalScope vector!!");
      exit(-1);
    }
    scope->clear();
  }

  scope->push_back(entry);
  return scope;
}

FormalScope *AddEntryIntoFormalScope(FormalScope *scope,
                                     FormalScopeEntry *entry) {
  if (!entry)
    return scope;
  if (!scope) {
    scope = new vector<FormalScopeEntry *>(1);
    if (!scope) {
      puts("Error: Cannot malloc space for FormalScope vector!!");
      exit(-1);
    }
    scope->clear();
  }

  scope->push_back(entry);
  return scope;
}

LocalScope *AddEntryIntoLocalScope(LocalScope *scope, LocalScopeEntry *entry) {
  if (!entry)
    return scope;
  if (!scope) {
    scope = new vector<LocalScopeEntry *>(1);
    if (!scope) {
      puts("Error: Cannot malloc space for LocalScope vector!!");
      exit(-1);
    }
    scope->clear();
  }

  scope->push_back(entry);
  return scope;
}

LocalScope *AttachTypeToLocalScope(LocalScope *scope, bool _is_const) {
  for (LocalScope::iterator it = scope->begin(); it != scope->end(); it++) {
    (*it)->is_const = _is_const;
  }
  return scope;
}

vector<ArrayInitValue> *NewInitValue(vector<ArrayInitValue> *values, int type,
                                     int value, GrammarTree expr) {
  // 判断 values 是否为空
  if (!values) {
    values = new vector<ArrayInitValue>;
    if (!values) {
      puts("Error: Cannot malloc space for InitValues vector!!");
      exit(-1);
    }
    values->clear();
  }
  // 新建 ArrayInitValue
  ArrayInitValue a = (ArrayInitValue)malloc(sizeof(ArrayInitVal));
  if (!a) {
    puts("Error: Cannot malloc space for ArrayInitValue!!");
    exit(-1);
  }
  a->type = (InitValType)type;
  if (type == Expr)
    a->expr = expr;
  else if (type == Value)
    a->value = value;

  values->push_back(a);
  return values;
}

vector<ArrayInitVal *> *AppendInitValue(vector<ArrayInitValue> *head,
                                        vector<ArrayInitValue> *tail,
                                        bool add_par) {
  // 判断 head 和 tail 是否为空
  if (add_par) {
    // 判断 head 是否为 null
    if (!head) {
      head = new vector<ArrayInitValue>;
      if (!head) {
        puts("Error: Cannot malloc space for ArrayInitValue vector!!");
        exit(-1);
      }
      head->clear();
    }
    // 向 head 中添加元素
    head->push_back(&LEFT);
    if (tail) {
      for (vector<ArrayInitValue>::iterator it = tail->begin();
           it != tail->end(); it++) {
        head->push_back(*it);
      }
    }
    head->push_back(&RIGHT);

  } else {
    if (!head && !tail)
      return nullptr;

    else if (!head)
      return tail;

    else if (!tail)
      return head;

    for (vector<ArrayInitValue>::iterator it = tail->begin(); it != tail->end();
         it++) {
      head->push_back(*it);
    }
  }
  return head;
}

vector<int> *AppendDim(vector<int> *dims, int d) {
  if (!dims) {
    // 分配新的 vector
    dims = new vector<int>;
    if (!dims) {
      puts("Error: Cannot malloc space for Dims vector!!");
      exit(-1);
    }
    dims->clear();
  }

  dims->push_back(d);
  return dims;
}

vector<int> *InsertDim1(vector<int> *dims) {
  // TODO: 此函数只在 构造 ArraySubSep 时使用 故暂不实现
  vector<int> *_dims = new vector<int>;
  vector<int>::iterator it;
  _dims->push_back(0);
  if(dims){
    for(it=dims->begin();it!=dims->end();it++){
      _dims->push_back(*it);
    }
  }
  return _dims;
}

ArrayInfo *NewArrayInfo(vector<int> *_dims,
                        vector<ArrayInitValue> *_raw_values) {
  ArrayInfo *a = (ArrayInfo *)malloc(sizeof(ArrayInfo));

  a->dims = _dims;
  a->raw_values = _raw_values;

  return a;
}

LocalScopeEntry *NewLocalArrayEntry(char *_name, bool _is_const,
                                    ArrayInfo *_array_info) {
  // 分配内存空间
  LocalScopeEntry *e = (LocalScopeEntry *)malloc(sizeof(LocalScopeEntry));
  // 给各字段赋值
  char *temp = (char *)malloc(sizeof(_name));
  if (!temp) {
    puts("Error: Cannot malloc space for name string!!");
    exit(-1);
  }
  strcpy(temp, _name);
  e->name = temp;
  e->is_const = _is_const;
  e->array_info = _array_info;

  e->is_array = true;
  e->is_block = false;

  return e;
}
// int / const int
LocalScopeEntry *NewLocalIntEntry(char *_name, bool _is_const,
                                  int _int_init_value,
                                  GrammarTree _int_init_expr) {
  // 分配内存空间
  LocalScopeEntry *e = (LocalScopeEntry *)malloc(sizeof(LocalScopeEntry));
  // 给各字段赋值
  char *temp = (char *)malloc(sizeof(_name));
  if (!temp) {
    puts("Error: Cannot malloc space for name string!!");
    exit(-1);
  }
  strcpy(temp, _name);
  e->name = temp;
  e->is_const = _is_const;
  if (_is_const)
    e->int_init_value = _int_init_value;
  else
    e->int_init_expr = _int_init_expr;

  e->is_array = false;
  e->is_block = false;

  return e;
}
// block
LocalScopeEntry *NewEmbeddedScopeEntry(LocalScope *_embedded_scope) {
  // 分配内存空间
  LocalScopeEntry *e = (LocalScopeEntry *)malloc(sizeof(LocalScopeEntry));
  // 给各字段赋值
  // TODO: 后续可能需要区分不同 Block
  char *temp = (char *)malloc(sizeof("Block"));
  if (!temp) {
    puts("Error: Cannot malloc space for name string!!");
    exit(-1);
  }
  strcpy(temp, "Block");
  e->name = temp;
  e->is_block = true;
  e->embedded_scope = _embedded_scope;

  e->is_array = false;
  e->is_const = false;

  return e;
}

// 2. 新建一个 formal_entry
FormalScopeEntry *NewFormalEntry(char *_name, bool _is_array,
                                 ArrayInfo *_array_info) {
  FormalScopeEntry *e = (FormalScopeEntry *)malloc(sizeof(FormalScopeEntry));

  char *temp = (char *)malloc(sizeof(_name));
  if (!temp) {
    puts("Error: Cannot malloc space for name string!!");
    exit(-1);
  }
  strcpy(temp, _name);
  e->name = temp;
  e->is_array = _is_array;
  e->array_info = _array_info;

  return e;
}

// 3. 新建一个 global_entry
GlobalScopeEntry *NewGlobalIntEntry(char *_name, bool _is_const,
                                    int _int_init_value,
                                    GrammarTree _int_init_expr) {
  // 分配内存空间
  GlobalScopeEntry *e = (GlobalScopeEntry *)malloc(sizeof(GlobalScopeEntry));

  char *temp = (char *)malloc(sizeof(_name));
  if (!temp) {
    puts("Error: Cannot malloc space for name string!!");
    exit(-1);
  }
  strcpy(temp, _name);
  e->name = temp;
  e->is_const = _is_const;
  if (_is_const)
    e->int_init_value = _int_init_value;
  else
    e->int_init_expr = _int_init_expr;

  e->is_func = false;
  e->is_void = false;
  e->is_array = false;
  e->formal_scope = nullptr;
  e->local_scope = nullptr;

  return e;
}

GlobalScopeEntry *NewGlobalArrayEntry(char *_name, bool _is_const,
                                      ArrayInfo *_array_info) {
  GlobalScopeEntry *e = (GlobalScopeEntry *)malloc(sizeof(GlobalScopeEntry));

  char *temp = (char *)malloc(sizeof(_name));
  if (!temp) {
    puts("Error: Cannot malloc space for name string!!");
    exit(-1);
  }
  strcpy(temp, _name);
  e->name = temp;
  e->is_const = _is_const;
  e->array_info = _array_info;

  e->is_func = false;
  e->is_void = false;
  e->is_array = true;
  e->formal_scope = nullptr;
  e->local_scope = nullptr;

  return e;
}

GlobalScopeEntry *NewFunctionEntry(char *_name, bool _is_void,
                                   int _formal_count,
                                   FormalScope *_formal_scope,
                                   LocalScope *_local_scope) {
  GlobalScopeEntry *e = (GlobalScopeEntry *)malloc(sizeof(GlobalScopeEntry));

  char *temp = (char *)malloc(sizeof(_name));
  if (!temp) {
    puts("Error: Cannot malloc space for name string!!");
    exit(-1);
  }
  strcpy(temp, _name);
  e->name = temp;
  e->is_void = _is_void;
  e->formal_count = _formal_count;
  e->formal_scope = _formal_scope;
  e->local_scope = _local_scope;

  e->is_func = true;
  e->is_array = false;
  e->is_const = false;

  return e;
}

void DisplayArrayInfo(ArrayInfo *array_info) {
  if (!array_info || !(array_info->dims))
    puts("[ERROR] Empty Array Info");
  // 打印维度
  else {
    printf("Dimensions:");
    for (vector<int>::iterator it = array_info->dims->begin();
         it != array_info->dims->end(); it++) {
      printf("[ %d ]", *it);
    }
    printf("\n");
  }
}

void DisplayLocalScope(LocalScope *symtable) {
  YELLOW

  if (!symtable) {
    puts("[ Empty Scope ]");
    return;
  }

  for (LocalScope::iterator it = symtable->begin(); it != symtable->end();
       it++) {
    LocalScopeEntry *e = *it;

    if (e->is_block) {
      // 打印内嵌作用域信息
      printf("Embedded Scope [ %s ]: \n", e->name);
      DisplayLocalScope(e->embedded_scope);
      YELLOW
    } else if (e->is_array) {
      // 打印数组信息
      printf("\nArray [ %s ]: \n", e->name);
      printf("\tConstant: ");
      if (e->is_const)
        printf("yes\n");
      else
        printf("no\n");
      DisplayArrayInfo(e->array_info);
      YELLOW
    } else {
      // 打印变量信息
      printf("\nVariable [ %s ]: \n", e->name);
      printf("\tConstant: ");
      if (e->is_const) {
        printf("yes\t");
        printf("Value: %d\n", e->int_init_value);
      } else {
        printf("no\n");
      }
    }
  }
}

void DisplayFormalScope(FormalScope *symtable) {
  GREEN

  printf("\t");

  if (!symtable) {
    puts("[ Empty Scope ]");
    return;
  }

  for (FormalScope::iterator it = symtable->begin(); it != symtable->end();
       it++) {
    FormalScopeEntry *e = *it;

    if (e->is_array) {
      printf("Array [ %s ]\t", e->name);

    } else {
      printf("Variable [ %s ]\t", e->name);
    }
  }

  printf("\n");
}

void DisplayGlobalScope(GlobalScope *symtable) {
  BLUE

      if (!symtable) {
    puts("[ Empty Scope ]");
    return;
  }

  for (GlobalScope::iterator it = symtable->begin(); it != symtable->end();
       it++) {
    GlobalScopeEntry *e = *it;

    if (e->is_func) {
      // 打印函数信息
      printf("\nFunction [ %s ]: \n", e->name);
      // 返回 void / 参数个数 / 形参 / 函数体
      printf("\tReturn Void: ");
      if (e->is_void)
        printf("yes\t");
      else
        printf("no\t\t");
      printf("Formals: %d\n", e->formal_count);

      printf("Function [ %s ]'s Formal Scope: \n", e->name);
      DisplayFormalScope(e->formal_scope);
      BLUE printf("Function [ %s ]'s Local Scope: \n", e->name);
      DisplayLocalScope(e->local_scope);
      BLUE

    } else if (e->is_array) {
      // 打印数组信息
      printf("\nGlobal Array [ %s ]: \n", e->name);
      printf("\tConstant: ");
      if (e->is_const)
        printf("yes\n");
      else
        printf("no\n");
      DisplayArrayInfo(e->array_info);
      BLUE

    } else {
      // 打印变量信息
      printf("\nGlobal Variable [ %s ]: \n", e->name);
      printf("\tConstant: ");
      if (e->is_const) {
        printf("yes\t");
        printf("Value: %d\n", e->int_init_value);
      } else {
        printf("no\n");
      }
    }
  }

  RESET
}

/*
int ArraySpaceSize(ArrayInfo *array_info){
  return array_info->spacesize();
}
*/
