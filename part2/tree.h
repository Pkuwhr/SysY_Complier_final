#ifndef __TREE_H__
#define __TREE_H__

#include "count.h"
#include "symtab.h"
#include "op.h"
#include "utility.h"

enum NodKind { STM_KIND, EXP_KIND };

enum ExpKind {
    OPERA_KIND,
    INTEG_KIND,
    VARIA_KIND,
    ARRAY_KIND,
    FUNCT_KIND,
    CALLF_KIND,
    LABEL_KIND
};

enum StmKind {
    GOTODO_KIND,
    GOTOIF_KIND,
    PARAME_KIND,
    RETURN_KIND,
    LABELT_KIND,
    ASSIGN_KIND,
    VARDEF_KIND,
    ARRDEF_KIND,
    FUNDEF_KIND,
    SEQUEN_KIND
};

class TreeNode {
  private:
    static const int MAX_CHILDREN = 3;

  public:
    TreeNode *child[MAX_CHILDREN];
    NodKind nodeKind;

    TreeNode(NodKind nodeKind) {
        this->nodeKind = nodeKind;

        for (int i = 0; i < MAX_CHILDREN; i++) {
            child[i] = NULL;
        }
    }

    virtual string GetName() {}
    virtual void SetGlobal() {}
    virtual string GenCode() {}
    virtual string GenDefCode() {}
    virtual string GenUseCode() {}
    virtual int GetKind(){}
};

class StmNode : public TreeNode {
  public:
    StmKind kind;
    bool globalStm;

    StmNode(StmKind _kind) : TreeNode(STM_KIND) { this->kind = _kind; }
    void SetGlobal() { this->globalStm = true; }
    string GenCode();
};

class ExpNode : public TreeNode {
  public:
    ExpKind kind;

    ExpNode(ExpKind kind) : TreeNode(EXP_KIND) { this->kind = kind; }
};

class VariaExpNode : public ExpNode {
  public:
    string varia;

    VariaExpNode(string _varia) : ExpNode(VARIA_KIND) { this->varia = _varia; }
    string GetName() { return varia; }
    string GenUseCode();
    string GenDefCode();
    int GetKind(){return kind;}
};

class LabelExpNode : public ExpNode {
  public:
    string label;

    LabelExpNode(string _label) : ExpNode(LABEL_KIND) { this->label = _label; }
    string GetName() { return label; }
    string GenCode() { return GetName(); }
    int GetKind(){return kind;}
};

class FunctExpNode : public ExpNode {
  public:
    string funct;

    FunctExpNode(string _funct) : ExpNode(FUNCT_KIND) { this->funct = _funct;}
    string GetName() { return funct; }
    string GenCode() { 
      return GetName(); 
    }
    int GetKind(){return kind;}
};

class Oper2ExpNode : public ExpNode {
  public:
    OpeKind opera;

    Oper2ExpNode(OpeKind _opera) : ExpNode(OPERA_KIND) { this->opera = _opera; }
    string GenUseCode();
    string GenDefCode(){return GenUseCode();}
    int GetKind(){return kind;}
};

class Oper1ExpNode : public ExpNode {
  public:
    OpeKind opera;

    Oper1ExpNode(OpeKind _opera) : ExpNode(OPERA_KIND) { this->opera = _opera; }
    string GenUseCode();
    string GenDefCode(){return GenUseCode();}
    int GetKind(){return kind;}
};

class CallfExpNode : public ExpNode {
  public:
    string funct;

    CallfExpNode(string _funct) : ExpNode(CALLF_KIND) { this->funct = _funct; }
    string GenUseCode();
    string GenDefCode(){return GenUseCode();}
    int GetKind(){return kind;}
};

class IntegExpNode : public ExpNode {
  public:
    int value;

    IntegExpNode(string _value) : ExpNode(INTEG_KIND) {
        stringstream sout;
        sout << _value;
        sout >> this->value;
    }
    /*
    string GenUseCode() { 
      string res;
      res = RegManager::AllocImmReg(value);
      if(res.compare("x0")){
        cout<<res << " = "<<value<<endl;

      }
      return res;
    }
    string GenDefCode(){
      return std::to_string(value); 
    }
    */
    string GenUseCode();
    string GenDefCode();
    int GetKind(){return kind;}
};

class ArrayExpNode : public ExpNode {
  public:
    string varia;

    ArrayExpNode(string _varia) : ExpNode(ARRAY_KIND) { this->varia = _varia; }
    string GenUseCode();
    string GenDefCode();
    int GetKind(){return kind;}
};

#endif
