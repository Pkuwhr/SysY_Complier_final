/*
 * @Date: 2020-07-01 15:19:44
 * @LastEditors: zyk
 * @LastEditTime: 2020-07-25 10:24:40
 * @FilePath: /compiler/main.cpp
 */

#include "main.h"

extern int yyparse();

int tuple_trigger = 1;
int gmerror;            // the errors in grammar analyzing
int smerror;

int semantic_check = 1; // semantic_check != 0时执行语义检查ScopeTrial

vector<codeline> programcode;


string StringFormat(const char *format, ...)
{
    string result;
    //将字符串的长度初始化为1024
    int tlen = 1024;
    int len = 0;
    result.resize(tlen);
    
    //为string赋值
    va_list list;
    va_start(list, format);
    len = vsnprintf((char*)result.c_str(), tlen, format, list);
    
    //如果结果字符串长度比初始长度长，就重新设置字符串长度，然后再赋值
    if( len >= tlen )
    {
        tlen = len + 1;
        result.resize(tlen);
        len = vsnprintf((char*)result.c_str(), tlen, format, list);
    }
    va_end(list);
    
    //删除字符串尾部的0字符
    result.erase(len);
    return result;
}

void AddNewCodeline(CodeLineType type, string codestr){
  codeline tempcodeline;
  tempcodeline.type=type;
  tempcodeline.code=codestr;
  programcode.push_back(tempcodeline);
}

void DisplayCode(vector<codeline> code){
  vector<codeline>::iterator it;
  for(it=code.begin();it!=code.end();it++){
    cout<<(*it).type<<" "<<(*it).code;
  }
}

vector<codeline> ReFormCode(vector<codeline> code){
  vector<codeline> res,tempres;//用来保存结果,保存中间结果
  vector<codeline>::iterator it,itt;
  vector<codeline> funcvardefcode,funcothercode,globalvarinitcode;//保存函数体内的变量声明语句，保存全局变量声明语句，
  vector<codeline> othercode;
  bool infunc=false;
  bool inmain=false;
  for(it=code.begin();it!=code.end();it++){
    switch ((*it).type){
      case VARDEC:{
        //如果不在函数体里
        if(!infunc){
          res.push_back(*it);
        }
        //如果在函数里
        else{
          funcvardefcode.push_back(*it);
        }
        break;
      }
      case VARINIT:{
        //对在函数之外初始化的变量挪到main函数开头
        if(!infunc){
          globalvarinitcode.push_back(*it);
        }else{
          funcothercode.push_back(*it);
        }
        break;
      }
      case FUNCBEGIN:{
        infunc=true;
        //压入第一条语句
        tempres.push_back(*it);
        break;
      }
      case FUNCEND:{
        infunc=false;
        //压入变量声明语句
        for(itt=funcvardefcode.begin();itt!=funcvardefcode.end();itt++){
          tempres.push_back(*itt);
        }
        //压入其他语句
        for(itt=funcothercode.begin();itt!=funcothercode.end();itt++){
          tempres.push_back(*itt);
        }
        //压入end语句
        tempres.push_back(*it);
        //清空语句向量
        funcothercode.clear();
        funcvardefcode.clear();
        break;
      }
      case MAINBEGIN:{
        infunc=true;
        inmain=true;
        tempres.push_back(*it);
        break;
      }
      case MAINEND:{
        infunc=false;
        inmain=false;
        //压入全局变量初始化语句
        for(itt=globalvarinitcode.begin();itt!=globalvarinitcode.end();itt++){
          tempres.push_back(*itt);
        }
        //压入变量声明语句
        for(itt=funcvardefcode.begin();itt!=funcvardefcode.end();itt++){
          tempres.push_back(*itt);
        }
        //压入其他语句
        for(itt=funcothercode.begin();itt!=funcothercode.end();itt++){
          tempres.push_back(*itt);
        }
        //压入结束语句
        tempres.push_back(*it);
        //清空无用的语句变量
        funcothercode.clear();
        funcvardefcode.clear();
        globalvarinitcode.clear();
        break;
      }
      default:{
        funcothercode.push_back(*it);
        break;
      }
    }
  }
  for(itt=tempres.begin();itt!=tempres.end();itt++){
    res.push_back(*itt);
  }
  return res;
}

int main(int argc, char **argv) {
  vector<codeline> eeyoreoutcode;
  vector<codeline>::iterator it;
  FILE *outfile;
  gmerror = 0;
  if (argc > 1) {
    if (!(yyin = fopen(argv[3], "r"))) {
      perror(argv[3]);
      return 1;
    }
    if (!(outfile = fopen(argv[5], "w"))) {
      perror(argv[5]);
      return 1;
    }
  }
  programcode.clear();
  if (tuple_trigger) {
    std::cout << "\nNow print the two-tuples of \"Lexical Analyzing\":\n";
    cout << "__________________________________________________\n\n";
  }
  yyparse();

  DisplayCode(programcode);
  printf("reform:\n\n");
  eeyoreoutcode=ReFormCode(programcode);
  DisplayCode(eeyoreoutcode);
  for(it=eeyoreoutcode.begin();it!=eeyoreoutcode.end();it++){
    fprintf(outfile,"%s",(*it).code.c_str());
  }
  fclose(outfile);
  return 0;
}
