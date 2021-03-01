#include "ArrayInfo.h"
ArrayInitVal ZERO, LEFT, RIGHT;

int ArrayInfo::size() { return this->dims->size(); }

void ArrayInfo::DisplayRawValues(){
  vector<ArrayInitValue>::iterator it;
  printf("raw value is:");
  for(it=raw_values->begin();it!=raw_values->end();it++){
    switch((*it)->type){
      case Begin:{
        printf("{ ");
        break;
      }
      case End:{
        printf("} ");
        break;
      }
      case Expr:{
        printf("expr, ");
        break;
      }
      case Value:{
        printf("%d, ",(*it)->value);
        break;
      }
      default:{
        break;
      }
    }
  }
  printf("\n");
}

void ArrayInfo::DisplayDims(){
  printf("raw dims is:");
  vector<int>::iterator it;
  for(it=dims->begin();it!=dims->end();it++){
    printf("%d ",*it);
  }
  printf("\n");
}
/*
void ArrayInfo::init() {
  if (!raw_values) return;
  init_values = new vector<ArrayInitValue>;
  if (!init_values) {
    puts("Error: Cannot malloc space for init_values vector!!");
    exit(-1);
  }

  ZERO.type = Value;
  ZERO.value = 0;
  LEFT.type = Begin;
  RIGHT.type = End;

  const int max_layer =
      size(); // 超过最大层数后 只取{}里的第一个数字 忽略其它数字
  init_values->clear();

  int cur_layer = 0, left = 1, elem_index = 0;
  for (vector<int>::iterator it = dims->begin(); it != dims->end(); it++) {
    left *= *it;
  }

  for (vector<ArrayInitValue>::iterator it = raw_values->begin();
       it != raw_values->end(); it++) {
    switch ((*it)->type) {
    case Begin:
      cur_layer++;

      // 计算left值 即当前层剩余的变量数
      if (cur_layer > max_layer) {
        left = 1;
      } else {
        int modulo = 1;
        for (int i = 1; i <= max_layer - cur_layer + 1; ++i) {
          modulo *= (*dims)[dims->size() - i];
        }
        // modulo为本层变量数
        // 根据elem_index和modulo计算left
        left = (elem_index / modulo + 1) * modulo - elem_index;
      }
      break;
    case End:
      while (left) {
        init_values->push_back(&ZERO);
        left--;
        elem_index++;
      }
      cur_layer--;
      break;
    case Expr:
    case Value:
      if (left) {
        init_values->push_back(*it);
        left--;
        elem_index++;
      }
      break;
    default:
      break;
    }
  }
}
*/
void ArrayInfo::init(){
  ZERO.type = Value;
  ZERO.value = 0;
  LEFT.type = Begin;
  RIGHT.type = End;
}

int ArrayInfo::spacesize(){
  if(!dims){
    return -1;
  }
  int arraysize=1;
  for (int i = 0; i < dims->size(); ++i) {
    arraysize *= (*dims)[i];
  }
  return 4*arraysize;
}

ArrayInitVal ArrayInfo::get_element(vector<int> indices) {
  ArrayInitVal ZEROVAL;
  ZEROVAL.type = Value;
  ZEROVAL.value = 0;
  if (!raw_values) return ZEROVAL;

  int index = 0;
  for (int i = 0; i < indices.size(); ++i) {
    int factor = 1;
    for (int j = i + 1; j < dims->size(); ++j) {
      factor *= (*dims)[j];
    }
    index += indices[i] * factor;
  }

  printf("idx: %d\n",index);

  const int max_layer =size(); // 超过最大层数后 只取{}里的第一个数字 忽略其它数字
  printf("zc\n");

  int cur_layer = 0, left = 1, elem_index = 0;
  int modulo=0,preleft=0;
  for (vector<int>::iterator it = dims->begin(); it != dims->end(); it++) {
    left *= *it;
  }

  printf("type:");
  for (vector<ArrayInitValue>::iterator it = raw_values->begin();it != raw_values->end(); it++) {
    printf("%d ",(*it)->type);
  }
  printf("vec.size= %d \n",raw_values->size());
  printf("\n");
  int cnt=0;

  for (vector<ArrayInitValue>::iterator it = raw_values->begin();it != raw_values->end(); it++) {
    printf("loop[%d] cur_layer: %d, elem_index: %d, left: %d, modulo: %d, preleft: %d\n",cnt++,cur_layer,elem_index,left,modulo,preleft);

    switch ((*it)->type) {
    case Begin:{
      preleft=left;
      cur_layer++;

      modulo=1;
      for (int i = 1; i <= max_layer - cur_layer + 1; ++i) {
        modulo *= (*dims)[dims->size() - i];
      }
      printf("BEGIN_MODULO: %d\n",modulo);

      if(cur_layer>max_layer){
        left=1;
      }else{
        left=modulo;
      }

      while(preleft%modulo){
        if(elem_index==index){
          printf("匹配的数组值为0\n");
          return ZERO;
        }
        elem_index++;
        preleft--;
      }

      break;
    }
    case End:{

      while(left){
        if(elem_index==index){
          printf("匹配的数组值为0\n");
          return ZERO;
        }
        elem_index++;
        left--;
      }

      cur_layer--;
      if(cur_layer==0){
        printf("end\n");
        return ZEROVAL;
      }

      modulo=1;
      for (int i = 1; i <= max_layer - cur_layer + 1; ++i) {
        modulo *= (*dims)[dims->size() - i];
      }

      left=modulo-elem_index%modulo;

      break;
    }
    case Expr:
    case Value:{
      if (left) {
        if(elem_index==index){
          printf("匹配的数组值为%d\n", (*it)->value);
          return *(*it);
        }
        left--;
        elem_index++;
      }
      break;
    }
    default:
      break;
    }
    
  }
  return ZEROVAL;
}

ArrayInitVal ArrayInfo::get_element_by_index(int index){
  ArrayInitVal ZEROVAL;
  ZEROVAL.type = Value;
  ZEROVAL.value = 0;
  if (!raw_values) return ZEROVAL;

  const int max_layer =size(); // 超过最大层数后 只取{}里的第一个数字 忽略其它数字

  int cur_layer = 0, left = 1, elem_index = 0;
  int modulo=0,preleft=0;
  for (vector<int>::iterator it = dims->begin(); it != dims->end(); it++) {
    left *= *it;
  }

  for (vector<ArrayInitValue>::iterator it = raw_values->begin();it != raw_values->end(); it++) {

    switch ((*it)->type) {
    case Begin:{
      preleft=left;
      cur_layer++;

      modulo=1;
      for (int i = 1; i <= max_layer - cur_layer + 1; ++i) {
        modulo *= (*dims)[dims->size() - i];
      }
      //printf("BEGIN_MODULO: %d\n",modulo);

      if(cur_layer>max_layer){
        left=1;
      }else{
        left=modulo;
      }

      while(preleft%modulo){
        if(elem_index==index){
          //printf("匹配的数组值为0\n");
          return ZERO;
        }
        elem_index++;
        preleft--;
      }

      break;
    }
    case End:{

      while(left){
        if(elem_index==index){
          //printf("匹配的数组值为0\n");
          return ZERO;
        }
        elem_index++;
        left--;
      }

      cur_layer--;
      if(cur_layer==0){
        //printf("end\n");
        return ZEROVAL;
      }

      modulo=1;
      for (int i = 1; i <= max_layer - cur_layer + 1; ++i) {
        modulo *= (*dims)[dims->size() - i];
      }

      left=modulo-elem_index%modulo;

      break;
    }
    case Expr:{
      if (left) {
        if(elem_index==index){
          return *(*it);
        }
        left--;
        elem_index++;
      }
      break;
    }
    case Value:{
      if (left) {
        if(elem_index==index){
          //printf("匹配的数组值为%d\n", (*it)->value);
          return *(*it);
        }
        left--;
        elem_index++;
      }
      break;
    }
    default:
      break;
    }
  }
  return ZEROVAL;
}

int ArrayInfo::dim2idx(vector<int> indices){
  if(indices.size()==0){
    return -1;
  }
  int index = 0;
  for (int i = 0; i < indices.size(); ++i) {
    int factor = 1;
    for (int j = i + 1; j < dims->size(); ++j) {
      factor *= (*dims)[j];
    }
    index += indices[i] * factor;
  }
  return index;
}

bool initdims(vector<int> *dims,vector<ArrayInitValue> *raw_dims){

  //printf("initdims\n");
  if(!dims){
    //printf("创建dim\n");
    dims = new vector<int>;
  }

  if(!raw_dims){
    return false;
  }

  vector<ArrayInitValue>::iterator it;


  int idx=0;

  //vector<ArrayInitValue>::iterator it;
  for(it=raw_dims->begin();it!=raw_dims->end();it++){



    if((*it)->type == Value){
      //printf("get value: ");
      dims->push_back((*it)->value);
      //printf("%d\n",dims->back());
    }else{
      //printf("get expr or other\n");
      CheckExprValue((*it)->expr);
      if(!(*it)->expr->is_constant_expr){
        dims->clear();
        return false;
      }else{
        dims->push_back((*it)->expr->expr_value);
      }
    }
  }
  //printf("end initdims\n");
  return true;
}
