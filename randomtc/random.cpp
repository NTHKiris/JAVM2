#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

string generateRandomVariableName(int length) {
    static const char alphanum[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return result;
}

string generateRandomInstruction(int type) {
    switch(type) {
        case 0: return (rand() % 2 == 0) ? "iconst" : "fconst";
        case 1: return (rand() % 2 == 0) ? "istore" : "fstore";
        default:
            vector<string> instructions = {
                "iload", "fload", "iadd", "fadd", "isub", "fsub", 
                "imul", "fmul", "idiv", "fdiv", "irem", "ineg", "fneg", 
                "iand", "ior", "ieq", "feq", "ilt", "flt", "top", "val", "par"
            };
            return instructions[rand() % instructions.size()];
    }
}

int main() {
    srand(time(nullptr));
    ofstream outFile("test_case.txt");

    int numInstructions = 100;
    int numConstInstructions = numInstructions / 5;  // 20% const
    int numStoreInstructions = numInstructions / 5;  // 20% store
    set<string> variables;
    vector<string> instructions;

    // Tạo các lệnh const
    for (int i = 0; i < numConstInstructions; ++i) {
        string instruction = generateRandomInstruction(0);
        int value = rand() % 1000;
        instructions.push_back(instruction + " " + to_string(value));
    }

    // Tạo các lệnh store
    for (int i = 0; i < numStoreInstructions; ++i) {
        string instruction = generateRandomInstruction(1);
        string varName = generateRandomVariableName(3);
        variables.insert(varName);
        instructions.push_back(instruction + " " + varName);
    }

    // Tạo các lệnh khác
    for (int i = numConstInstructions + numStoreInstructions; i < numInstructions; ++i) {
        string instruction = generateRandomInstruction(2);
        if (instruction.find("load") != string::npos || 
            instruction == "val" || instruction == "par") {
            if (!variables.empty()) {
                auto it = variables.begin();
                advance(it, rand() % variables.size());
                string varName = *it;
                instructions.push_back(instruction + " " + varName);
            } else {
                // Nếu không có biến, tạo một lệnh khác
                i--;
                continue;
            }
        } else {
            instructions.push_back(instruction);
        }
    }

    // Sắp xếp lại để đảm bảo const và store ở đầu
    sort(instructions.begin(), instructions.end(), 
         [](const string& a, const string& b) {
             bool aIsConstOrStore = (a.substr(0, 5) == "iconst" || a.substr(0, 5) == "fconst" ||
                                     a.substr(0, 6) == "istore" || a.substr(0, 6) == "fstore");
             bool bIsConstOrStore = (b.substr(0, 5) == "iconst" || b.substr(0, 5) == "fconst" ||
                                     b.substr(0, 6) == "istore" || b.substr(0, 6) == "fstore");
             return aIsConstOrStore && !bIsConstOrStore;
         });

    // Ghi ra file
    for (const auto& instr : instructions) {
        outFile << instr << endl;
    }

    outFile.close();
    cout << "Test case has been generated in 'test_case.txt'" << endl;

    return 0;
}