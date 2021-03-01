/*
 * @Date: 2020-06-15 19:40:12
 * @LastEditors: zyk
 * @LastEditTime: 2020-07-15 21:22:28
 * @FilePath: /compiler/Nonterminals.c
 */


#include"Nonterminals.h"
#include"GrammarTree.h"
extern "C" {
    #include"parser.tab.h"
}


string NameOf(int type) {
    switch (type) {
        case Program:
            return "Program";
            break;
        case CompUnit:
            return "CompUnit";
            break;
        case Decl:
            return "Decl";
            break;
        case FuncDef:
            return "FuncDef";
            break;
        case ConstDecl:
            return "ConstDecl";
            break;
        case BType:
            return "BType";
            break;
        case ConstDefSeq:
            return "ConstDefSeq";
            break;
        case ConstDef:
            return "ConstDef";
            break;
        case ConstInitVal:
            return "CosntInitVal";
            break;
        case ConstArraySubSeq:
            return "CosntArraySubSeq";
            break;
        case ConstExp:
            return "ConstExp";
            break;
        case ArraySubSeq:
            return "ArraySubSeq";
            break;
        case Exp:
            return "Exp";
            break;
        case ConstInitValSeq:
            return "ConstInitValSeq";
            break;
        case VarDecl:
            return "VarDecl";
            break;
        case VarDefSeq:
            return "VarDefSeq";
            break;
        case VarDef:
            return "VarDef";
            break;
        case InitVal:
            return "InitVal";
            break;
        case InitValSeq:
            return "InitValSeq";
            break;
        case Block:
            return "Block";
            break;
        case FuncFParams:
            return "FuncFParams";
            break;
        case FuncFParam:
            return "FuncFParam";
            break;
        case BlockItemSeq:
            return "BlockItemSeq";
            break;
        case BlockItem:
            return "BlockItem";
            break;
        case Stmt:
            return "Stmt";
            break;
        case LVal:
            return "LVal";
            break;
        case Cond:
            return "Cond";
            break;
        case AddExp:
            return "AddExp";
            break;
        case LOrExp:
            return "LOrExp";
            break;
        case Number:
            return "Number";
            break;
        case PrimaryExp:
            return "PrimaryExp";
            break;
        case UnaryOp:
            return "UnaryOp";
            break;
        case UnaryExp:
            return "UnaryExp";
            break;
        case FuncRParams:
            return "FuncRParams";
            break;
        case MulExp:
            return "MulExp";
            break;
        case RelExp:
            return "RelExp";
            break;
        case EqExp:
            return "EqExp";
            break;
        case LAndExp:
            return "LAndExp";
            break;
        case T_Identifier:
            return "Identifier";
            break;
        case T_StringConstant:
            return "StringConstant";
            break;
        case T_IntConstant:
            return "IntConstant";
            break;
        case T_Void:
            return "Void";
            break;
        case T_Int:
            return "Int";
            break;
        case T_Const:
            return "Const";
            break;
        case T_While:
            return "While";
            break;
        case T_If:
            return "If";
            break;
        case T_Else:
            return "Else";
            break;
        case T_Return:
            return "Return";
            break;
        case T_Break:
            return "Break";
            break;
        case T_Continue:
            return "Continue";
            break;
        case T_LessEqual:
            return "<=";
            break;
        case T_GreaterEqual:
            return ">=";
            break;
        case T_Equal:
            return "==";
            break;
        case T_NotEqual:
            return "!=";
            break;
        case T_And:
            return "&&";
            break;
        case T_Or:
            return "||";
            break;
        case '+':
            return "+";
            break;
        case '-':
            return "-";
            break;
        case '*':
            return "*";
            break;
        case '/':
            return "/";
            break;
        case '%':
            return "%";
            break;
        case '=':
            return "=";
            break;
        case '!':
            return "!";
            break;
        case '<':
            return "<";
            break;
        case '>':
            return ">";
            break;

        default:
            return "unknown";
    }
}
