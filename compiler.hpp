#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <stdio.h>
#include <string.h>

using namespace std;

// todo dodać sobie currIndex żeby nie robić niepotrzebnych loadów (może być 0 jak mam jakąś nową wartość nie zapisaną jeszcze)
// todo podobnie można z currVal ale to dużo roboty xD

struct Variable {
    char const *name;
    char const *type; // NUM, ARR, ARR VAR, ITR
    long long index;
    long long size; // dla tablic
    long long startIdx; // początkowy index tablicy
    bool init;
    long long value; // na razie dla num
    char const *iterator; // dla pętli for
};

extern vector<string> code;
extern map<string, Variable> variables;

// commands
void startProgram();
void endProgram();
void declareVar(char *name);
void declareArr(char *name, long long a, long long b);
void initVar(Variable var);
void valCmd(Variable var);
void readCmd(Variable var);
void writeCmd(Variable var);
void plusCmd(Variable a, Variable b);
void minusCmd(Variable a, Variable b);
void timesCmd(Variable a, Variable b);
void divCmd(Variable a, Variable b);
void modCmd(Variable a, Variable b);

void eqCmd(Variable a, Variable b);
void neqCmd(Variable a, Variable b);
void leCmd(Variable a, Variable b);
void geCmd(Variable a, Variable b);
void leqCmd(Variable a, Variable b);
void geqCmd(Variable a, Variable b);

void ifCmd();
void elseCmd();

void forToCmd(char *it, Variable from, Variable to);
void forDowntoCmd(char *it, Variable from, Variable to);
void endForCmd();

void whileCmd();
void endWhileCmd();
void doCmd();
void endDoCmd();

//functions
void createVariable(Variable *var, char *name, char const *type);
void createArray(Variable *var, char *name, long long size, long long start, char const *type);
Variable tempVar(long long a);
Variable checkInit(Variable var);
Variable getVar(char *name);
Variable setArr(char *name, char *id);
Variable getArr(char *name, long long idx);
void createNum(long long a);
void createConstant(long long a);
list<string> calcSteps(long long a, string cmd);
void resetCurr();
void loadVar(Variable var);
void storeVar(Variable var);
void insertCmd(string cmd);
void refreshJumps();
void replaceCmd(long long index, string newCmd);
void printError(string message);
