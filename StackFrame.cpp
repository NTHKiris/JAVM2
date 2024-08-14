#include "StackFrame.h"
#include <iostream>
#include <fstream>
#include "errors.h"
#include "constants.h"
#include <iomanip>
using namespace std;
void StackFrame::printOperandStack()
{
    cout << "Operand Stack: <";
    for (int i = 0; i < opStackSize; ++i)
    {
        if (i > 0)
            cout << ", ";
        if (opStack[i].type == 1)
            cout << opStack[i].value;
        else
            cout << (opStack[i].value);
        cout << "," << opStack[i].type;
    }
    cout << ">" << endl;
}

int StackFrame::preInstr(string instruction)
{
    char pre = instruction[0];
    if (pre == 'i')
        return 0;
    if (pre == 'f')
        return 1;
    return -1;
}
void StackFrame::printLocalVariables() {
    cout << "Local Variables:" << endl;
    printAVLTree(localVar, "", true);
}

void StackFrame::printAVLTree(localVarAVL* node, string prefix, bool isLeft) {
    if (node == nullptr) {
        return;
    }

    cout << prefix;
    cout << (isLeft ? "├─" : "└─" );

    // Print the current node
    cout << node->key << " " << static_cast<int>(node->value) << " (" << (node->type == 0 ? "int" : "float") << ")" << endl;

    // Enter the next tree level - left and right branch
    printAVLTree(node->left, prefix + (isLeft ? "│  " : "   "), true);
    printAVLTree(node->right, prefix + (isLeft ? "│  " : "   "), false);
}

StackFrame::StackFrame() : opStackMaxSize(OPERAND_STACK_MAX_SIZE), localVarSpaceSize(LOCAL_VARIABLE_SPACE_SIZE)
{
    line = 0;
    opStackSize = 0;
    localVar = nullptr;
    localVarSize = 0;
    opStack = new Element[opStackMaxSize];
}
StackFrame::~StackFrame()
{
    delete[] opStack;
    deleteAVL(localVar);
}
int StackFrame::getHeight(localVarAVL *node)
{
    if (node == nullptr)
        return 0;
    return node->height;
}
int StackFrame::getBalance(localVarAVL *node)
{
    if (node == nullptr)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}
StackFrame::localVarAVL *StackFrame::rotateRight(localVarAVL *node)
{
    if (node == nullptr)
        return node;
    localVarAVL *temp = node->left;
    node->left = temp->right;
    temp->right = node;
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    temp->height = 1 + max(getHeight(temp->left), getHeight(temp->right));
    return temp;
}
StackFrame::localVarAVL *StackFrame::rotateLeft(localVarAVL *node)
{
    if (node == nullptr)
        return node;
    localVarAVL *temp = node->right;
    node->right = temp->left;
    temp->left = node;
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    temp->height = 1 + max(getHeight(temp->left), getHeight(temp->right));
    return temp;
}
StackFrame::localVarAVL *StackFrame::insertAVL(localVarAVL *node, string &key, float value, int type)
{
    
    if (node == nullptr)
    {
        if (localVarSize >= localVarSpaceSize/2) 
            throw LocalSpaceFull(line);
        localVarSize++;
        return new localVarAVL(key, value, type);
    }
    
    if (key < node->key)
    {
        node->left = insertAVL(node->left, key, value, type);
    }
    else if (key > node->key)
    {
        node->right = insertAVL(node->right, key, value, type);
    }
    else // key == node->key
    {
        // Update existing node
        node->value = value;
        node->type = type;
        return node;
    }

    // Update height of current node
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // Check balance and rotate if necessary
    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rotateRight(node);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return rotateLeft(node);

    // Left Right Case
    if (balance > 1 && key > node->left->key)
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key)
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}
void StackFrame::deleteAVL(localVarAVL *node)
{
    if (node == nullptr)
        return;
    deleteAVL(node->left);
    deleteAVL(node->right);
    delete node;
}

void StackFrame::pushToOpStack(float value, int type)
{
    if (opStackSize >= opStackMaxSize / 2)
    {
        throw StackFull(line);
    }
    opStack[opStackSize].value = static_cast<float>(value);
    opStack[opStackSize].type = type;
    opStackSize++;
}
StackFrame::Element StackFrame::popFromOpStack()
{
    if (opStackSize <= 0)
    {
        throw StackEmpty(line);
    }
    opStackSize--;

    return opStack[opStackSize];
}
StackFrame ::Element StackFrame::topOpStack()
{
    if (opStackSize <= 0)
    {
        throw StackEmpty(line);
    }
    return opStack[opStackSize - 1];
}
// void StackFrame::storeToLocalVar(string &key, float value, int type)
// {
//     insertAVL(localVar, key, value, type);

// }

StackFrame::localVarAVL *StackFrame::getAVL(localVarAVL *node, string &key)
{
    if (node == nullptr)
        return node;
    if (node->key == key)
        return node;
    if (key < node->key)
        return getAVL(node->left, key);
    return getAVL(node->right, key);
}
StackFrame::Element StackFrame::loadFromLocalVar(string &key)
{
    // if(localVar->height==0) throw StackEmpty(line);
    localVarAVL *node = getAVL(localVar, key);
    if (node == nullptr)
    {
        throw UndefinedVariable(line);
    }
    return Element(node->value, node->type);
}
bool StackFrame::isValidNumber(const string &s)
{
    if (s.empty())
        return false;

    int start = 0;
    if (s[0] == '-' || s[0] == '+')
        start = 1;

    bool hasDecimal = false;
    for (size_t i = start; i < s.length(); i++)
    {
        if (s[i] == '.')
        {
            if (hasDecimal)
                return false;
            hasDecimal = true;
        }
        else if (!isdigit(s[i]))
        {
            return false;
        }
    }

    return true;
}
bool StackFrame::isInt(const std::string &s)
{
    if (s.empty())
        return false;
    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] == '.')
        {
            return false;
        }
    }
    return true;
}
StackFrame::localVarAVL *StackFrame::parNode(localVarAVL *root, string &key)
{
    if (root == nullptr || root->key == key)
    {
        return nullptr; 
    }

    if ((root->left && root->left->key == key) || (root->right && root->right->key == key))
    {
        return root; 
    }

    if (key < root->key)
    {
        return parNode(root->left, key); 
    }
    else
    {
        return parNode(root->right, key); 
    }
}
void StackFrame::processInstruction(const string &instruction, const string &argument)
{
    // print
    if (instruction == "top" || instruction == "val" || instruction == "par")
    {
        if (instruction == "top")
        {
            Element e = topOpStack();
            if (e.type == 0)
            {
                int x = e.value;
                cout << x << endl;
            }
            else
            {
                float x = static_cast<float>(e.value);
                cout << x << endl;
            }
        }
        if (instruction == "val")
        {
            string key = argument;
            Element e = loadFromLocalVar(key);
            if (e.type == 0)
            {
                int x = e.value;
                cout << x << endl;
            }
            else
            {
                float x = static_cast<float>(e.value);
                cout << x << endl;
            }
        }
        if (instruction == "par")
        {
            if (localVar == nullptr)
                throw UndefinedVariable(line);
            string key = argument;
            localVarAVL *parent = parNode(localVar, key);
            if (parent == nullptr)
                cout << "null" << endl;
            else
                cout << parent->key << endl;
        }
    }
    string instruct = instruction.substr(1, instruction.length());
    int type = preInstr(instruction);

    // const load store
    if (instruct == "const" || instruct == "store" || instruct == "load")
    {

        if (instruct == "const")
        {
            if (!isValidNumber(argument))
            {
                throw TypeMisMatch(line);
            }

            if (type == 0)
            {
                if (!isInt(argument))
                    throw TypeMisMatch(line);
                pushToOpStack(stoi(argument), type);
            }
            else if (type == 1)
            {
                // if (isInt(argument))
                //     throw TypeMisMatch(line);
                pushToOpStack(stof(argument), type);
            }
        }

        else if (instruct == "store")
        {
            if (!isValidNumber(argument))
            {
                string key = argument;
                Element e = popFromOpStack();
                if (type != e.type)
                    throw TypeMisMatch(line);

                localVar = insertAVL(localVar, key, e.value, e.type);
            }
            else
            {
                throw TypeMisMatch(line);
            }
        }
        else if (instruct == "load")
        {

            if (!isValidNumber(argument))
            {

                string key = argument;
                Element e = loadFromLocalVar(key);
                if (type != e.type)
                    throw TypeMisMatch(line);
                pushToOpStack(e.value, e.type);
            }
            else
            {
                throw UndefinedVariable(line);
            }
        }
    }
    // change type
    if (instruct == "2f" || instruct == "2i")
    {
        if (instruct == "2f")
        {
            Element e = popFromOpStack();
            if (type != e.type)
                throw TypeMisMatch(line);
            pushToOpStack(e.value, 1);
        }
        if (instruct == "2i")
        {
            Element e = popFromOpStack();
            if (type != e.type)
                throw TypeMisMatch(line);
            // const float INT_MAX_F = 2147483647.0f;
            // const float INT_MIN_F = -2147483648.0f;

            // if (e.value >= INT_MAX_F || e.value <= INT_MIN_F)
            // {
            //     throw UndefinedVariable(line);
            // }
            int x = static_cast<int>(e.value);
            pushToOpStack(x, 0);
        }
    }

    // math
    if (instruct == "add" || instruct == "sub" || instruct == "mul" || instruct == "div")
    {
        Element e1 = popFromOpStack();
        // if (type != e1.type)
        //     throw TypeMisMatch(line);
        Element e2 = popFromOpStack();
        // if (type != e2.type)
        //     throw TypeMisMatch(line);
        if (type == 0 && e1.type == 1)
            throw TypeMisMatch(line);
        if (type == 0 && e2.type == 1)
            throw TypeMisMatch(line);
        if (e1.type != e2.type)
        {
            e1.type = e2.type = 1;
        }
        float x1 = e1.value;
        float x2 = e2.value;
        if (instruct == "add")
        {
            pushToOpStack(x1 + x2, type);
        }
        if (instruct == "sub")
        {
            pushToOpStack(x2 - x1, type);
        }
        if (instruct == "mul")
        {
            pushToOpStack(x1 * x2, type);
        }
        if (instruct == "div")
        {
            if (x1 == 0)
            {
                throw DivideByZero(line);
            }
            if (type == 0)
            {
                pushToOpStack(static_cast<int>(x2 / x1), type);
            }
            else
            {
                pushToOpStack(x2 / x1, type);
            }
        }
    }
    // bitwise
    if (instruct == "rem" || instruct == "and" || instruct == "or" || instruct == "eq" || instruct == "neq" || instruct == "lt" || instruct == "gt")
    {

        Element e1 = popFromOpStack();
        Element e2 = popFromOpStack();
        float x1 = e1.value;
        float x2 = e2.value;

        if (type == 0 && e1.type == 1)
            throw TypeMisMatch(line);
        if (type == 0 && e2.type == 1)
            throw TypeMisMatch(line);

        if (instruct == "rem")
        {
            if (e1.value == 0)
                throw DivideByZero(line);
            int x = ((int)x2 % (int)x1);
            pushToOpStack(x, type);
        }

        if (instruct == "and")
        {
            int x = (int)x1 & (int)x2;
            pushToOpStack(x, 0);
        }

        if (instruct == "or")
        {
            int x = (int)x1 | (int)x2;
            pushToOpStack(x, 0);
        }

        if (instruct == "eq")
        {
            int x = x1 == x2 ? 1 : 0;
            pushToOpStack(x, 0);
        }

        if (instruct == "neq")
        {
            int x = x1 != x2 ? 1 : 0;
            pushToOpStack(x, 0);
        }

        if (instruct == "lt")
        {
            int x = x2 < x1 ? 1 : 0;
            pushToOpStack(x, 0);
        }
        if (instruct == "gt")
        {
            int x = x2 > x1 ? 1 : 0;
            pushToOpStack(x, 0);
        }
    }
    // bnot neg
    if (instruct == "bnot" || instruct == "neg")
    {
        Element e = popFromOpStack();
        float x = e.value;
        if (type == 0 && e.type == 1)
            throw TypeMisMatch(line);

        if (instruct == "bnot")
        {
            x == 0 ? pushToOpStack(1, type) : pushToOpStack(0, type);
        }
        if (instruct == "neg")
        {
            pushToOpStack(e.value * -1, type);
        }
    }
}

void StackFrame::getElement(const string &inputLine, string &instruction, string &argument)
{
    size_t pos = inputLine.find(' ');
    if (pos != string::npos)
    {
        instruction = inputLine.substr(0, pos);
        argument = inputLine.substr(pos + 1);
    }
    else
    {
        instruction = inputLine;
    }
}
void StackFrame::run(string filename)
{

    fstream readFile(filename, std::ios::in);

    // string filepath =  filename;
    // // string filepath =  filename;
    // ifstream file(filepath);
    if (!readFile.is_open())
    {
        cout << "Cannot open file " << filename << endl;
        return;
    }
    string inputLine;
    while (getline(readFile, inputLine))
    {
        line++;
        string instruction = "",
               argument = "";
        cout << "Processing line " << line << ": " << inputLine << endl;
        getElement(inputLine, instruction, argument);
        // cout << instruction << " " + argument << endl;
        processInstruction(instruction, argument);
        printLocalVariables();
        printOperandStack();
    }
    readFile.close();
}