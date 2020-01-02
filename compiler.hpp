#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <list>

using namespace std;

// todo dodać sobie currIndex żeby nie robić niepotrzebnych loadów (może być 0 jak mam jakąś nową wartość nie zapisaną jeszcze)
// todo podobnie można z currVal ale to dużo roboty xD

struct Variable {
    char *name;
    char const *type;
    int index;
    bool init; 
    long long value;
};

extern vector<string> code;
extern map<string, Variable> variables;

// commands
void declareVar(char *name);
void initVar(char *name);
void valCmd(Variable var);
void readCmd(char *name);
void writeCmd(char *name);
void plusCmd(Variable a, Variable b);
void minusCmd(Variable a, Variable b);
void timesCmd(Variable a, Variable b);
void endProgram();

//functions
void createVariable(Variable *var, char *name, char const *type);
Variable tempVar(long long a);
Variable tempVar(char *name);
void createNum(long long a);
list<string> calcSteps(long long a, string cmd);
void resetCurr();
void loadVar(Variable var);
