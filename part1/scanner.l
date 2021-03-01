/*
 * @Date: 2020-06-13 17:07:18
 * @LastEditors: zyk
 * @LastEditTime: 2020-07-15 15:27:28
 * @FilePath: /compiler/scanner.l
 */ 


%{
    #include<stdio.h>
    #include<stdlib.h>
    #include"GrammarTree.h"
    #include "ScopeStack.h"
    #include"parser.tab.h"

    // 用于打印词法分析结果
    #define PRINT_TUPLE(...) if(tuple_trigger) printf(__VA_ARGS__);
    // 用于打印词法分析阶段报错信息
    #define DEBUG(...) printf(__VA_ARGS__);

    // TODO: 将其改为可通过命令行参数配置
    extern int tuple_trigger;  // =1时打印词法分析结果（二元组形式）
    int yycolumn = 1;

    // 每个动作前执行的公有动作，用于维护token的位置信息
    #define YY_USER_ACTION \
      yylloc.first_line = yylloc.last_line = yylineno; \
      yylloc.first_column = yycolumn; \
      yylloc.last_column = yycolumn + yyleng - 1; \
      yycolumn += yyleng;

    ScopeStack mystack;
    ScopeStack *stack;
    GrammarTree tempnode;

%}

/* 处理多行注释。具体用法请自行了解start condition特性 */
%x COMMENT

%option yylineno
%option noyywrap

DIGIT             ([0-9])
OCT_DIGIT         ([0-7])
HEX_DIGIT         ([0-9a-fA-F])

HEX_INTEGER       (0[Xx]{HEX_DIGIT}+)
OCT_INTEGER       (0{OCT_DIGIT}*)
INTEGER           ([1-9]{DIGIT}*)

BEG_STRING        (\"[^"\n]*)
STRING            ({BEG_STRING}\")

IDENTIFIER        ([a-zA-Z][a-zA-Z_0-9]*)

OPERATOR          ([-+/*%=,;!<>()[\]{}])

BEG_COMMENT       ("/*")
END_COMMENT       ("*/")
SINGLE_COMMENT    ("//"[^\n]*)

SPACE               " "|\t

%%

 /* -------------------- Comments ----------------------------- */
{BEG_COMMENT}            {
    BEGIN(COMMENT);
    PRINT_TUPLE("Comments detected...\n")
}
<COMMENT>{END_COMMENT}   {
    BEGIN(INITIAL);
}
<COMMENT><<EOF>>            {
    DEBUG("Invalid comments\n")
    return 0;
}
<COMMENT>'\n'               {
    yycolumn = 1;
}
<COMMENT>.                  {
    /* ignore everything else that doesn't match */
}
{SINGLE_COMMENT}         {
    yycolumn = 1;
    /* skip to end of line for // comment */
}


 /* --------------------- Keywords ------------------------------- */
"void"      {
    PRINT_TUPLE("(%s, T_Void)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Void, 0, yylineno);
    return T_Void;
}
"int"       {
    PRINT_TUPLE("(%s, T_Int)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Int, 0, yylineno);
    return T_Int;
}
"const"     {
    PRINT_TUPLE("(%s, T_Const)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Const, 0, yylineno);
    return T_Const;
}
"while"     {
    PRINT_TUPLE("(%s, T_While)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_While, 0, yylineno);
    return T_While;
}
"if"        {
    PRINT_TUPLE("(%s, T_If)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_If, 0, yylineno);
    return T_If;
}
"else"      {
    PRINT_TUPLE("(%s, T_Else)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Else, 0, yylineno);
    return T_Else;
}
"return"    {
    PRINT_TUPLE("(%s, T_Return)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Return, 0, yylineno);
    return T_Return;
}
"break"     {
    PRINT_TUPLE("(%s, T_Break)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Break, 0, yylineno);
    return T_Break;
}
"continue"  {
    PRINT_TUPLE("(%s, T_Continue)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Continue, 0, yylineno);
    return T_Continue;
}


 /* -------------------- Operators ----------------------------- */
"<=" {
    PRINT_TUPLE("(%s, T_LessEqual)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_LessEqual, 0, yylineno);
    return T_LessEqual;
}
">=" {
    PRINT_TUPLE("(%s, T_GreaterEqual)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_GreaterEqual, 0, yylineno);
    return T_LessEqual;
}
"==" {
    PRINT_TUPLE("(%s, T_Equal)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Equal, 0, yylineno);
    return T_Equal;
}
"!=" {
    PRINT_TUPLE("(%s, T_NotEqual)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_NotEqual, 0, yylineno);
    return T_Equal;
}
"&&" {
    PRINT_TUPLE("(%s, T_And)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_And, 0, yylineno);
    return T_And;
}
"||" {
    PRINT_TUPLE("(%s, T_Or)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_Or, 0, yylineno);
    return T_Or;
}
{OPERATOR} {
    PRINT_TUPLE("(%s, T_Operator)\n", yytext)
    stack = &mystack;
    if(yytext[0]=='{'||yytext[0]=='('){
        Scope temp;
        ScopeStackPush(stack, temp);
    }
    else if(yytext[0]=='}'||yytext[0]==')'){
        ScopeStackPop(stack);
    }

    /* 一个字符的运算符使用其ASCII码对应的整数表示 */
    yylval.grammar_tree = CreateGrammarTree(yytext[0], 0, yylineno);
    return yytext[0];
}


 /* -------------------- Constants ------------------------------ */
{INTEGER}       {
    PRINT_TUPLE("(\"%s\" = %d, T_IntConstant)\n", yytext, (int) strtol(yytext, NULL, 10))
    yylval.grammar_tree = CreateGrammarTree(T_IntConstant, 0, yylineno);
    return T_IntConstant;
}
{OCT_INTEGER}   {
    PRINT_TUPLE("(\"%s\" = %d, T_IntConstant)\n", yytext, (int) strtol(yytext, NULL, 8))
    yylval.grammar_tree = CreateGrammarTree(T_IntConstant, 0, yylineno);
    return T_IntConstant;
}
{HEX_INTEGER}   {
    PRINT_TUPLE("(\"%s\" = %d, T_IntConstant)\n", yytext, (int) strtol(yytext, NULL, 16))
    yylval.grammar_tree = CreateGrammarTree(T_IntConstant, 0, yylineno);
    return T_IntConstant;
}                      
{STRING}        {
    PRINT_TUPLE("(%s, T_StringConstant)\n", yytext)
    yylval.grammar_tree = CreateGrammarTree(T_StringConstant, 0, yylineno);
    return T_StringConstant;
}
{BEG_STRING}    {
    DEBUG("invalid string")
}


 /* -------------------- Identifiers --------------------------- */
{IDENTIFIER}    {
    PRINT_TUPLE("(%s, T_Identifier)\n", yytext)
    stack = &mystack;
    yylval.grammar_tree = CreateGrammarTree(T_Identifier, 0, yylineno);
    tempnode=NULL;
    tempnode=TraverseScopeStack(stack, yytext);
    if(tempnode!=NULL){
        printf("find old symbol\n");
        yylval.grammar_tree=tempnode;
    }else{
        AddNewSym(stack, yylval.grammar_tree);
    }
    return T_Identifier;
}


 /* -------------------- Default rule (error) -------------------- */
{SPACE} {
    /* ignore */
}
\n {
    yycolumn = 1;
}
\r {
    /* Maintain compatibility for CRLF file. */
}
. {
    DEBUG("Error type A at Line %d column %d: Mysterious characters \"%s\".\n",
          yylineno, yylloc.first_column, yytext)
}

%%


