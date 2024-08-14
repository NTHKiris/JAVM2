#ifndef __STACK_FRAME_H__
#define __STACK_FRAME_H__

#include <string>


/*
StackFrame declaration
*/

class StackFrame {

private:
    int opStackMaxSize; // max size of operand stack
    int localVarSpaceSize; // size of local variable array
    
    struct Element
    {
        float value;
        int type;
        Element() : value(0), type(0) {}
        Element(float v, int t):value(v),type(t){};
    };
    struct localVarAVL{
        std::string key;
        float value;
        int type;
        int height; 
        localVarAVL *left;
        localVarAVL *right;
        localVarAVL(std::string key, float value, float type)  : key(key), value(value), type(type), height(1), left(nullptr), right(nullptr) {};
    };
    int line;
    int topStack;
    Element *opStack;
    int opStackSize;
    localVarAVL *localVar;
    int localVarSize;
    

   

    

public:
    /*
    Constructor of StackFrame
    */
    StackFrame();
    ~StackFrame();
    void deleteAVL(localVarAVL* node);
    localVarAVL *insertAVL(localVarAVL* node ,std::string & key, float value, int type);
    int getHeight(localVarAVL* node); 
    int getBalance(localVarAVL* node);
    localVarAVL* rotateRight(localVarAVL* node);
    localVarAVL* rotateLeft(localVarAVL* node);
    void pushToOpStack(float value, int type);
    Element popFromOpStack();
    Element topOpStack();
    void storeToLocalVar(std::string & key, float value, int type);
    Element loadFromLocalVar(std::string & key);
    localVarAVL* getAVL(localVarAVL* node,std::string& key);
    float getLocalVar(std::string key, int& type);
    void updateLocalVar(std::string key, float value, int type);
    void processInstruction(const std::string & instruction,const std::string &  argument);
    void getElement(const std::string &inputLine, std::string &instruction, std::string &argument);
    int preInstr(std::string instruction);
    bool isValidNumber(const std::string& s);
    bool isInt(const std::string& s);
    localVarAVL* parNode(localVarAVL* node, std::string& key);
    /*
    Run the method written in the testcase
    @param filename name of the file
    */
    void run(std::string filename);
    void printOperandStack();
    void printLocalVariables();
    void printAVLTree(localVarAVL* node, std::string prefix, bool isLeft);
    
};


#endif // !__STACK_FRAME_H__