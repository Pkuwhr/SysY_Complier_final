#include "ScopeStack.h"
#include "parser.tab.h"
#include <vector>

extern "C" {
  extern int yyparse();
  extern int yylex();
}


void InitScopeStack(ScopeStack *stack) { stack->clear(); }

void DestroyScopeStack(ScopeStack *stack) { stack->clear(); }

int ScopeStackLength(ScopeStack *stack) { return stack->size(); }

void ScopeStackPush(ScopeStack *stack, Scope scope) { stack->push_back(scope); }

void AddNewSym(ScopeStack *stack, GrammarTree new_node){
  if(stack->empty()){
    Scope temp;
    ScopeStackPush(stack,temp);
    return;
  }
  if(new_node!=NULL){
    stack->back().push_back(new_node);
  }
  
}

void ScopeStackPop(ScopeStack *stack) {
  stack->pop_back();
}

GrammarTree TraverseScopeStack(ScopeStack *stack, char *name) {
  if(stack->empty()){
    return NULL;
  }
  // traverse the stack and find certain name
  GrammarTree result = NULL;
  ScopeStack::iterator itss;
  Scope::iterator its;
  for(itss=stack->begin(); itss!=stack->end(); itss++){
    for(its=(*itss).begin();its!=(*itss).end();its++){
      if(!strcmp(name,(*its)->string_value)){
        result=(GrammarTree)malloc(sizeof(GrammarTreeNode));
        printf("\n***********************************找到了匹配的项***********************************\n");
        *result=*(*its);
        return result;
      }
    }
  }
  return result;
}

// check if given root is a constant expr
void CheckExprValue(GrammarTree tree) {
  // 判断 tree 是否为空
  if (!tree) return;
  // * Exp 'op' Exp 形式
  if (tree->lchild->type == Exp) {
    // 检查两个子 Exp 节点
    if (!tree->lchild->isVisited)
      CheckExprValue(tree->lchild);
    if (!tree->lchild->rchild->rchild->isVisited)
      CheckExprValue(tree->lchild->rchild->rchild);

    if (tree->lchild->is_constant_expr &&
        tree->lchild->rchild->rchild->is_constant_expr) {
      // 两个操作数为常量 故 tree 也是常量
      tree->is_constant_expr = true;

      switch (tree->lchild->rchild->type) { // 处理不同操作符
      case '*':
        tree->expr_value =
            tree->lchild->expr_value * tree->lchild->rchild->rchild->expr_value;
        break;
      case '/':
        tree->expr_value =
            tree->lchild->expr_value / tree->lchild->rchild->rchild->expr_value;
        break;
      case '%':
        tree->expr_value =
            tree->lchild->expr_value % tree->lchild->rchild->rchild->expr_value;
        break;
      case '+':
        tree->expr_value =
            tree->lchild->expr_value + tree->lchild->rchild->rchild->expr_value;
        break;
      case '-':
        tree->expr_value =
            tree->lchild->expr_value - tree->lchild->rchild->rchild->expr_value;
        break;
      case '<':
        tree->expr_value = (tree->lchild->expr_value <
                            tree->lchild->rchild->rchild->expr_value)
                               ? 1
                               : 0;
        break;
      case '>':
        tree->expr_value = (tree->lchild->expr_value >
                            tree->lchild->rchild->rchild->expr_value)
                               ? 1
                               : 0;
        break;
      case T_LessEqual:
        tree->expr_value = (tree->lchild->expr_value <=
                            tree->lchild->rchild->rchild->expr_value)
                               ? 1
                               : 0;
        break;
      case T_GreaterEqual:
        tree->expr_value = (tree->lchild->expr_value >=
                            tree->lchild->rchild->rchild->expr_value)
                               ? 1
                               : 0;
        break;
      case T_Equal:
        tree->expr_value = (tree->lchild->expr_value ==
                            tree->lchild->rchild->rchild->expr_value)
                               ? 1
                               : 0;
        break;
      case T_NotEqual:
        tree->expr_value = (tree->lchild->expr_value !=
                            tree->lchild->rchild->rchild->expr_value)
                               ? 1
                               : 0;
        break;
      case T_And:
        tree->expr_value = (tree->lchild->expr_value &&
                            tree->lchild->rchild->rchild->expr_value)
                               ? 1
                               : 0;
        break;
      case T_Or:
        tree->expr_value = (tree->lchild->expr_value ||
                            tree->lchild->rchild->rchild->expr_value)
                               ? 1
                               : 0;
        break;

      default:
        break;
      }
    } else {
      tree->is_constant_expr = false;
    }
    tree->isVisited = true;
  
  
  // * ( Exp ) 形式
  } else if (tree->lchild->type == '(') {
    // '(' Exp ')'
    if (!tree->lchild->rchild->isVisited)
      CheckExprValue(tree->lchild->rchild);

    if (tree->lchild->rchild->is_constant_expr) {
      tree->expr_value = tree->lchild->rchild->expr_value;
      tree->is_constant_expr = true;
    } else {
      tree->is_constant_expr = false;
    }
    tree->isVisited = true;
  
  
  // * 'op' Exp 形式
  } else if (tree->lchild->rchild && tree->lchild->rchild->type == Exp) {
    // '-' Exp
    if (!tree->lchild->rchild->isVisited)
      CheckExprValue(tree->lchild->rchild);

    if (tree->lchild->rchild->is_constant_expr) {
      tree->is_constant_expr = true;

      switch (tree->lchild->type) {
      case '-':
        tree->expr_value = -(tree->lchild->rchild->expr_value);
        break;
      case '+':
        tree->expr_value = +(tree->lchild->rchild->expr_value);
        break;
      case '!':
        // ! 操作数必须为0或1
        assert(tree->lchild->rchild->expr_value == 0 ||
               tree->lchild->rchild->expr_value == 1);
        tree->expr_value = (tree->lchild->rchild->expr_value) ^ 1;
        break;

      default:
        break;
      }
    } else {
      tree->is_constant_expr = false;
    }
    tree->isVisited = true;
  
  
  // * 整型常量
  } else if (tree->lchild->type == T_IntConstant) {
    tree->isVisited = true;
    tree->is_constant_expr = true;
    tree->expr_value = tree->lchild->int_value;
  
  
  }
  // * 字符常量 / 左值 / 函数调用
  else {
    tree->isVisited = true;
    tree->is_constant_expr = tree->lchild->is_constant_expr;
    tree->expr_value=tree->lchild->expr_value;
  }
  return;
}
