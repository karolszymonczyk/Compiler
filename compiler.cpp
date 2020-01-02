#include "compiler.hpp"

extern int yylineno;

int memoryIndex = 1;
// int loadedIndex = 1; // chyba nie potrzbne jak nie będę cudować z memoryIndex xD
// int currValue = 0;

vector<string> code;
map<string, Variable> variables;

// commands (dodać czy jest ostatni i wtedy lineno -1)
void declareVar(char *name)
{
    // zacząć program od SUB 0 ?

    if (variables.find(name) != variables.end())
    {
        cout << "Error: in line " << yylineno << "Variable already declared!" << endl;
        string error("Error in variable declaration in line: " + yylineno);
    }
    else
    {
        Variable variable;
        char const *type = "VAR";
        createVariable(&variable, name, type);
    }
}

void endProgram()
{
    code.push_back("HALT");
}

void initVar(char *name)
{
    Variable var = variables.at(name);
    char const *itrType = "ITR";
    if (var.type == itrType)
    {
        cout << "Error: in line " << yylineno << "Loop iterator change" << endl;
        string error("Error in loop in line: " + yylineno);
    }

    variables.at(name).init = true;
    code.push_back("STORE " + to_string(var.index));
}

void readCmd(char *name)
{
    code.push_back("GET");
    Variable var = variables.at(name);
    code.push_back("STORE " + to_string(var.index));
    variables.at(name).init = true;
}

void writeCmd(char *name)
{
    Variable var = variables.at(name);
    loadVar(var);
    code.push_back("PUT");
}

void valCmd(Variable var)
{
    char const *numType = "NUM";
    if (var.type == numType)
    {
        createNum(var.value);
    }
    else
    {
        loadVar(var);
    }
}

void plusCmd(Variable a, Variable b)
{
    char const *numType = "NUM";

    // dla bezpieczeństwa ale trzeba zoptymalizować XD
    if (a.value == 0)
    {
        if (b.type == numType)
        {
            createNum(b.value);
        }
        else
        {
            loadVar(b);
        }
        return;
    }
    else if (b.value == 0)
    {
        if (a.type == numType)
        {
            createNum(a.value);
        }
        else
        {
            loadVar(a);
        }
        return;
    }

    if (a.type == numType && b.type == numType)
    {
        long long result = a.value + b.value;
        createNum(result);
    }
    else
    {
        if (a.type == numType)
        {
            createNum(a.value);
            code.push_back("ADD " + to_string(b.index));
        }
        else if (b.type == numType)
        {
            createNum(b.value);
            code.push_back("ADD " + to_string(b.index));
        }
        else
        {
            //todo trzeba sprawdzić czy nie jest aktualnie załadowany i ew zmienić kolejność dodawania
            loadVar(a);
            code.push_back("ADD " + to_string(b.index));
        }
    }
}

void minusCmd(Variable a, Variable b)
{
    char const *numType = "NUM";

    // dla bezpieczeństwa ale trzeba zoptymalizować XD
    if (a.value == 0)
    {
        if (b.type == numType)
        {
            createNum(-b.value);
        }
        else
        {
            loadVar(b);
            resetCurr();
            code.push_back("SUB " + to_string(b.index));
        }
        return;
    }
    else if (b.value == 0)
    {
        if (a.type == numType)
        {
            createNum(a.value);
        }
        else
        {
            loadVar(a);
        }
        return;
    }

    if (a.type == numType && b.type == numType)
    {
        long long result = a.value - b.value;
        createNum(result);
    }
    else
    {
        if (a.type == numType)
        {
            createNum(a.value);
            code.push_back("SUB " + to_string(b.index));
        }
        else if (b.type == numType)
        {
            createNum(-b.value);
            code.push_back("ADD " + to_string(b.index));
        }
        else
        {
            //todo trzeba sprawdzić czy nie jest aktualnie załadowany i ew zmienić kolejność dodawania
            loadVar(a);
            code.push_back("SUB " + to_string(b.index));
        }
    }
}

void timesCmd(Variable a, Variable b)
{
    char const *numType = "NUM";

    // dla bezpieczeństwa ale trzeba zoptymalizować XD
    if (a.value == 0 || b.value == 0)
    {
        resetCurr();
        return;
    }

    if (a.type == numType && b.type == numType)
    {
        long long result = a.value * b.value;
        createNum(result);
    }
    else
    {
//         SUB 0 
// STORE 5 #wynik
// STORE 6 #flaga
// DEC
// STORE 1 #-1
// INC
// INC
// STORE 2 #1
// GET
// STORE 3 #a
// GET
// STORE 4 #b
// JPOS 20 #sakczemy do instrukcji
// SUB 0
// SUB 4
// STORE 4
// LOAD 6
// INC
// STORE 6
// LOAD 4
//     JZERO 35 # while b > 0 (if b < 0)   #JUMP
// 	SHIFT 1
// 	SHIFT 2
// 	SUB 4
// 	JZERO 28 # if b % 2 == 0 (parzyste) #JUMP
// 	LOAD 5
// 	ADD 3
// 	STORE 5
// 	LOAD 3
// 	SHIFT 2
// 	STORE 3
// 	LOAD 4
// 	SHIFT 1
// 	STORE 4
// 	JUMP 20                                 #JUMP
// LOAD 6
// JZERO 41 #do wypisania wyniku
// SUB 0
// SUB 5
// STORE 5		# czy na pewno potrzebne?
// JUMP 42
// LOAD 5
// PUT
// HALT
        // if (a.type == numType)
        // {
        //     createNum(a.value);
        //     code.push_back("SUB " + to_string(b.index));
        // }
        // else if (b.type == numType)
        // {
        //     createNum(-b.value);
        //     code.push_back("ADD " + to_string(b.index));
        // }
        // else
        // {
        //     //todo trzeba sprawdzić czy nie jest aktualnie załadowany i ew zmienić kolejność dodawania
        //     loadVar(a);
        //     code.push_back("SUB " + to_string(b.index));
        // }
    }
}

// FUNCTIONS
void createVariable(Variable *var, char *name, char const *type)
{
    var->name = name;
    var->type = type;
    var->init = false;
    var->index = memoryIndex;
    memoryIndex++;

    variables.insert({name, *var});
}

Variable tempVar(long long a)
{
    Variable var;

    char const *numType = "NUM";
    var.type = numType;
    var.value = a;

    return var;
}

Variable tempVar(char *name)
{
    Variable var = variables.at(name);

    return var;
}

void createNum(long long a)
{
    string cmd = a > 0 ? "INC" : "DEC";
    resetCurr(); //tymczasowe
    // później sprawdzanie co jest na stosie i czy jest większe czy mniejsze itd
    // można przy zapisywaniu zmiennych dodawać im też value i jak się różnią
    // o odpowiednią ilość to ladować najbliższą i odpowiednio znajdywać

    if (a == 0)
    {
        return;
    }

    if (abs(a) <= 23)
    {
        for (long long i = 0; i < abs(a); i++)
        {
            code.push_back(cmd);
        }
    }
    else
    {
        code.push_back("INC");
        code.push_back("STORE " + to_string(memoryIndex));
        list<string> steps = calcSteps(abs(a), cmd);
        if (a < 0)
        {
            code.push_back("DEC");
            code.push_back("DEC");
        }
        for (string s : steps)
        {
            code.push_back(s);
        }
    }
}

list<string> calcSteps(long long a, string cmd)
{
    list<string> steps;
    while (a != 1)
    {
        if (a % 2 == 0)
        {
            steps.push_front("SHIFT " + to_string(memoryIndex));
            a /= 2;
        }
        else
        {
            steps.push_front(cmd);
            a -= 1;
        }
    }

    return steps;
}

void resetCurr()
{
    code.push_back("SUB 0");
    // currValue = 0;
}

void loadVar(Variable var)
{
    // if (currValue != var.value)
    // {
    code.push_back("LOAD " + to_string(var.index));
    // currValue = var.value;
    // }
}
