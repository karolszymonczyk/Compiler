#include "compiler.hpp"

extern int yylineno;

long long memoryIndex = 3;
long long cmdIndex = 0;

char const *arrVarType = "ARR VAR";
char const *arrType = "ARR";
char const *varType = "VAR";
char const *numType = "NUM";
char const *itrType = "ITR";

vector<string> code;
map<string, Variable> variables;
map<long long, long long> constants;
vector<Variable> jumps;

void startProgram()
{
    resetCurr();
    insertCmd("INC");
    insertCmd("STORE 1"); //1
    insertCmd("DEC");
    insertCmd("DEC");
    insertCmd("STORE 2"); //-1
}

void endProgram()
{
    insertCmd("HALT");
    checkInit();
}

void declareVar(char *name)
{
    if (variables.find(name) != variables.end())
    {
        string message = "Variable \"" + string(name) + string("\" already declared");
        printError(message);
    }
    else
    {
        Variable variable;
        char const *type = "VAR";
        createVariable(&variable, name, type);
    }
}

void declareArr(char *name, long long a, long long b)
{

    if (variables.find(name) != variables.end())
    {
        string message = "Variable \"" + string(name) + string("\" already declared");
        printError(message);
    }
    else if (a > b)
    {
        string message = "Incorrect range in \"" + string(name) + string("\" array");
        printError(message);
    }
    else
    {
        Variable variable;
        char const *type = "ARR";
        long long size = (b - a) + 1;
        createArray(&variable, name, size, a, type);
    }
}

void initVar(Variable var)
{
    if (var.type == itrType)
    {
        string message = "Loop iterator \"" + string(var.name) + string("\" can not be changed");
        printError(message);
    }
    else
    {
        if (var.type != arrVarType && var.type != arrType)
        {
            variables.at(var.name).init = true;
        }
        storeVar(var);
    }
}

void readCmd(Variable var)
{
    insertCmd("GET");
    storeVar(var);
    if (var.type != arrVarType && var.type != arrType)
    {
        variables.at(var.name).init = true;
    }
}

void writeCmd(Variable var)
{
    if (var.type == numType)
    {
        createNum(var.value);
        insertCmd("PUT");
    }
    else if (var.type == arrType)
    {
        loadVar(var);
        insertCmd("PUT");
    }
    // else if (var.type == arrVarType)
    // {
    //     loadVar(var);
    //     insertCmd("PUT");
    // }
    // else if (!var.init)
    // {
    //     string message = "Variable \"" + string(var.name) + string("\" was not initialised");
    //     printError(message);
    // }
    else
    {
        if (var.type != arrVarType)
        {
            if (var.used == false)
            {
                variables.at(var.name).line = yylineno;
                variables.at(var.name).used = true;
            }
        }
        loadVar(var);
        insertCmd("PUT");
    }
}

void valCmd(Variable var)
{
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
    if (a.type == numType && a.value == 0)
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
    else if (b.type == numType && b.value == 0)
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
            if (llabs(a.value) < 10) // added
            {
                loadVar(b);
                string cmd = a.value > 0 ? "INC" : "DEC";
                for (long long i = 0; i < llabs(a.value); i++)
                {
                    insertCmd(cmd);
                }
            }
            else
            {
                createNum(a.value);
                if (b.type == arrVarType)
                {
                    insertCmd("STORE " + to_string(memoryIndex));
                    loadVar(b);
                    insertCmd("ADD " + to_string(memoryIndex));
                }
                else
                {
                    insertCmd("ADD " + to_string(b.index));
                }
            }
        }
        else if (b.type == numType)
        {
            if (llabs(b.value) < 10) // added
            {
                loadVar(a);
                string cmd = b.value > 0 ? "INC" : "DEC";
                for (long long i = 0; i < llabs(b.value); i++)
                {
                    insertCmd(cmd);
                }
            }
            else
            {
                createNum(b.value);
                if (a.type == arrVarType)
                {
                    insertCmd("STORE " + to_string(memoryIndex));
                    loadVar(a);
                    insertCmd("ADD " + to_string(memoryIndex));
                }
                else
                {
                    insertCmd("ADD " + to_string(a.index));
                }
            }
        }
        else
        {
            if (a.type == arrVarType && b.type == arrVarType)
            {
                loadVar(a);
                insertCmd("STORE " + to_string(memoryIndex));
                loadVar(b);
                insertCmd("ADD " + to_string(memoryIndex));
            }
            else if (b.type == arrVarType)
            {
                loadVar(b);
                insertCmd("ADD " + to_string(a.index));
            }
            else
            {
                loadVar(a);
                insertCmd("ADD " + to_string(b.index));
            }
        }
    }
}

void minusCmd(Variable a, Variable b)
{
    if (a.type == numType && a.value == 0)
    {
        if (b.type == numType)
        {
            createNum(-b.value);
        }
        else
        {
            if (b.type == arrVarType)
            {
                loadVar(b);
                insertCmd("STORE " + to_string(memoryIndex));
                resetCurr();
                insertCmd("SUB " + to_string(memoryIndex));
            }
            else
            {
                resetCurr();
                insertCmd("SUB " + to_string(b.index));
            }
        }
        return;
    }
    else if (b.type == numType && b.value == 0)
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
            if (b.type == arrVarType)
            {
                loadVar(b);
                insertCmd("STORE " + to_string(memoryIndex));
                createNum(a.value);
                insertCmd("SUB " + to_string(memoryIndex));
            }
            else
            {
                createNum(a.value);
                insertCmd("SUB " + to_string(b.index));
            }
        }
        else if (b.type == numType)
        {
            if (llabs(b.value) < 10) // added
            {
                loadVar(a);
                string cmd = b.value < 0 ? "INC" : "DEC";
                for (long long i = 0; i < llabs(b.value); i++)
                {
                    insertCmd(cmd);
                }
            }
            else
            {
                if (a.type == arrVarType)
                {
                    loadVar(a);
                    insertCmd("STORE " + to_string(memoryIndex));
                    createNum(-b.value);
                    insertCmd("ADD " + to_string(memoryIndex));
                }
                else
                {
                    createNum(-b.value);
                    insertCmd("ADD " + to_string(b.index));
                }
            }
        }
        else
        {
            if (a.type == arrVarType && b.type == arrVarType)
            {
                loadVar(b);
                insertCmd("STORE " + to_string(memoryIndex));
                loadVar(a);
                insertCmd("SUB " + to_string(memoryIndex));
            }
            else if (b.type == arrVarType)
            {
                loadVar(b);
                insertCmd("STORE " + to_string(memoryIndex));
                loadVar(a);
                insertCmd("SUB " + to_string(memoryIndex));
            }
            else
            {
                loadVar(a);
                insertCmd("SUB " + to_string(b.index));
            }
        }
    }
}

void timesCmd(Variable a, Variable b)
{
    if ((a.type == numType && a.value == 0) || (b.type == numType && b.value == 0))
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
        if (a.type == numType)
        {
            createNum(a.value);
            a.index = memoryIndex + 6;
            insertCmd("STORE " + to_string(a.index));
        }
        else if (b.type == numType)
        {
            createNum(b.value);
            b.index = memoryIndex + 6;
            insertCmd("STORE " + to_string(b.index));
        }

        resetCurr();
        insertCmd("STORE " + to_string(memoryIndex));
        insertCmd("STORE " + to_string(memoryIndex + 1));
        loadVar(a);
        insertCmd("JZERO " + to_string(cmdIndex + 57));
        insertCmd("STORE " + to_string(memoryIndex + 2));
        loadVar(b);
        insertCmd("JZERO " + to_string(cmdIndex + 54));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("JPOS " + to_string(cmdIndex + 7));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("INC");
        insertCmd("STORE " + to_string(memoryIndex + 1));
        insertCmd("LOAD " + to_string(memoryIndex + 2));
        insertCmd("JPOS " + to_string(cmdIndex + 20));
        insertCmd("ADD " + to_string(memoryIndex + 3));
        insertCmd("JNEG " + to_string(cmdIndex + 20));
        insertCmd("LOAD " + to_string(memoryIndex + 2));
        insertCmd("STORE " + to_string(memoryIndex + 4));
        insertCmd("LOAD " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 2));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("JPOS " + to_string(cmdIndex + 14));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("JPOS " + to_string(cmdIndex + 3));
        insertCmd("INC");
        insertCmd("JUMP " + to_string(cmdIndex + 2));
        insertCmd("DEC");
        insertCmd("STORE " + to_string(memoryIndex + 1));
        insertCmd("JUMP " + to_string(cmdIndex + 3));
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("JNEG " + to_string(cmdIndex - 18));
        insertCmd("LOAD " + to_string(memoryIndex + 3));
        insertCmd("JZERO " + to_string(cmdIndex + 15));
        insertCmd("SHIFT 2");
        insertCmd("SHIFT 1");
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("JZERO " + to_string(cmdIndex + 4));
        insertCmd("LOAD " + to_string(memoryIndex));
        insertCmd("ADD " + to_string(memoryIndex + 2));
        insertCmd("STORE " + to_string(memoryIndex));
        insertCmd("LOAD " + to_string(memoryIndex + 2));
        insertCmd("SHIFT 1");
        insertCmd("STORE " + to_string(memoryIndex + 2));
        insertCmd("LOAD " + to_string(memoryIndex + 3));
        insertCmd("SHIFT 2");
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("JUMP " + to_string(cmdIndex - 14));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("JZERO " + to_string(cmdIndex + 4));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex));
        insertCmd("JUMP " + to_string(cmdIndex + 2));
        insertCmd("LOAD " + to_string(memoryIndex));
    }
}

void divCmd(Variable a, Variable b)
{
    if ((a.type == numType && a.value == 0) || (b.type == numType && b.value == 0))
    {
        resetCurr();
        return;
    }

    if (a.type == numType && b.type == numType)
    {
        long long result = a.value / b.value;
        if (a.value * b.value < 0)
        {
            result--;
        }
        createNum(result);
    }
    else
    {
        if (a.type == numType)
        {
            createNum(a.value);
            a.index = memoryIndex + 6;
            insertCmd("STORE " + to_string(a.index));
        }
        else if (b.type == numType)
        {
            createNum(b.value);
            b.index = memoryIndex + 6;
            insertCmd("STORE " + to_string(b.index));
        }

        resetCurr();
        insertCmd("STORE " + to_string(memoryIndex));
        insertCmd("DEC");
        insertCmd("STORE " + to_string(memoryIndex + 1));
        insertCmd("INC");
        insertCmd("INC");
        insertCmd("STORE " + to_string(memoryIndex + 2));
        loadVar(a);
        insertCmd("JZERO " + to_string(cmdIndex + 61));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("JPOS " + to_string(cmdIndex + 8));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("INC");
        insertCmd("STORE " + to_string(memoryIndex + 1));
        insertCmd("LOAD " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 4));
        loadVar(b);
        insertCmd("JZERO " + to_string(cmdIndex + 49));
        insertCmd("STORE " + to_string(memoryIndex + 5));
        insertCmd("JPOS " + to_string(cmdIndex + 7));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex + 5));
        insertCmd("STORE " + to_string(memoryIndex + 5));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("INC");
        insertCmd("STORE " + to_string(memoryIndex + 1));
        insertCmd("LOAD " + to_string(memoryIndex + 5));
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("JZERO " + to_string(cmdIndex + 9));
        insertCmd("JPOS " + to_string(cmdIndex + 8));
        insertCmd("LOAD " + to_string(memoryIndex + 5));
        insertCmd("SHIFT 1");
        insertCmd("STORE " + to_string(memoryIndex + 5));
        insertCmd("LOAD " + to_string(memoryIndex + 2));
        insertCmd("SHIFT 1");
        insertCmd("STORE " + to_string(memoryIndex + 2));
        insertCmd("JUMP " + to_string(cmdIndex - 10));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
        insertCmd("SUB " + to_string(memoryIndex + 5));
        insertCmd("JNEG " + to_string(cmdIndex + 7));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
        insertCmd("SUB " + to_string(memoryIndex + 5));
        insertCmd("STORE " + to_string(memoryIndex + 4));
        insertCmd("LOAD " + to_string(memoryIndex));
        insertCmd("ADD " + to_string(memoryIndex + 2));
        insertCmd("STORE " + to_string(memoryIndex));
        insertCmd("LOAD " + to_string(memoryIndex + 5));
        insertCmd("SHIFT 2");
        insertCmd("STORE " + to_string(memoryIndex + 5));
        insertCmd("LOAD " + to_string(memoryIndex + 2));
        insertCmd("SHIFT 2");
        insertCmd("STORE " + to_string(memoryIndex + 2));
        insertCmd("JZERO " + to_string(cmdIndex + 2));
        insertCmd("JUMP " + to_string(cmdIndex - 16));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("JNEG " + to_string(cmdIndex + 10));
        insertCmd("JPOS " + to_string(cmdIndex + 9));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex));
        insertCmd("STORE " + to_string(memoryIndex));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
        insertCmd("JNEG " + to_string(cmdIndex + 4));
        insertCmd("LOAD " + to_string(memoryIndex));
        insertCmd("DEC");
        insertCmd("JUMP " + to_string(cmdIndex + 2));
        insertCmd("LOAD " + to_string(memoryIndex));
    }
}

void modCmd(Variable a, Variable b)
{
    if ((a.type == numType && a.value == 0) || (b.type == numType && b.value == 0))
    {
        resetCurr();
        return;
    }

    if (a.type == numType && b.type == numType)
    {
        long long result = a.value - floor((double)a.value / (double)b.value) * b.value;
        cout << result << endl;
        createNum(result);
    }
    else
    {
        if (a.type == numType)
        {
            createNum(a.value);
            a.index = memoryIndex + 7;
            insertCmd("STORE " + to_string(a.index));
        }
        else if (b.type == numType)
        {
            createNum(b.value);
            b.index = memoryIndex + 7;
            insertCmd("STORE " + to_string(b.index));
        }
        resetCurr();
        insertCmd("STORE " + to_string(memoryIndex));
        insertCmd("STORE " + to_string(memoryIndex + 1));
        insertCmd("INC");
        insertCmd("STORE " + to_string(memoryIndex + 2));
        loadVar(a);
        insertCmd("JZERO " + to_string(cmdIndex + 64));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("JPOS " + to_string(cmdIndex + 8));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("LOAD " + to_string(memoryIndex));
        insertCmd("INC");
        insertCmd("STORE " + to_string(memoryIndex));
        insertCmd("LOAD " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 4));
        loadVar(b);
        insertCmd("JZERO " + to_string(cmdIndex + 52));
        insertCmd("STORE " + to_string(memoryIndex + 5));
        insertCmd("STORE " + to_string(memoryIndex + 6));
        insertCmd("JPOS " + to_string(cmdIndex + 7));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex + 5));
        insertCmd("STORE " + to_string(memoryIndex + 5));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("INC");
        insertCmd("STORE " + to_string(memoryIndex + 1));
        insertCmd("LOAD " + to_string(memoryIndex + 5));
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("JZERO " + to_string(cmdIndex + 9));
        insertCmd("JPOS " + to_string(cmdIndex + 8));
        insertCmd("LOAD " + to_string(memoryIndex + 5));
        insertCmd("SHIFT 1");
        insertCmd("STORE " + to_string(memoryIndex + 5));
        insertCmd("LOAD " + to_string(memoryIndex + 2));
        insertCmd("SHIFT 1");
        insertCmd("STORE " + to_string(memoryIndex + 2));
        insertCmd("JUMP " + to_string(cmdIndex - 10));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
        insertCmd("SUB " + to_string(memoryIndex + 5));
        insertCmd("JNEG " + to_string(cmdIndex + 4));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
        insertCmd("SUB " + to_string(memoryIndex + 5));
        insertCmd("STORE " + to_string(memoryIndex + 4));
        insertCmd("LOAD " + to_string(memoryIndex + 5));
        insertCmd("SHIFT 2");
        insertCmd("STORE " + to_string(memoryIndex + 5));
        insertCmd("LOAD " + to_string(memoryIndex + 2));
        insertCmd("SHIFT 2");
        insertCmd("STORE " + to_string(memoryIndex + 2));
        insertCmd("JZERO " + to_string(cmdIndex + 2));
        insertCmd("JUMP " + to_string(cmdIndex - 13));
        insertCmd("LOAD " + to_string(memoryIndex));
        insertCmd("JZERO " + to_string(cmdIndex + 10));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("JZERO " + to_string(cmdIndex + 5));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex + 4));
        insertCmd("JUMP " + to_string(cmdIndex + 10));
        insertCmd("LOAD " + to_string(memoryIndex + 6));
        insertCmd("SUB " + to_string(memoryIndex + 4));
        insertCmd("JUMP " + to_string(cmdIndex + 7));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("JZERO " + to_string(cmdIndex + 4));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
        insertCmd("ADD " + to_string(memoryIndex + 6));
        insertCmd("JUMP " + to_string(cmdIndex + 2));
        insertCmd("LOAD " + to_string(memoryIndex + 4));
    }
}

void eqCmd(Variable a, Variable b)
{
    a = prepareCond(a, b);
    Variable jump;

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JZERO " + to_string(cmdIndex + 2));
    insertCmd("JUMP false");
    jump.index = cmdIndex - 1;
    jump.size = 1;

    jumps.push_back(jump);
}

void neqCmd(Variable a, Variable b)
{
    a = prepareCond(a, b);
    Variable jump;

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JNEG " + to_string(cmdIndex + 3));
    insertCmd("JPOS " + to_string(cmdIndex + 2));
    insertCmd("JUMP false");
    jump.index = cmdIndex - 1;
    jump.size = 1;

    jumps.push_back(jump);
}

void leCmd(Variable a, Variable b)
{
    a = prepareCond(a, b);
    Variable jump;

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JPOS " + to_string(cmdIndex + 2));
    insertCmd("JUMP false");
    jump.index = cmdIndex - 1;
    jump.size = 1;

    jumps.push_back(jump);
}

void geCmd(Variable a, Variable b)
{
    a = prepareCond(a, b);
    Variable jump;

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JNEG " + to_string(cmdIndex + 2));
    insertCmd("JUMP false");
    jump.index = cmdIndex - 1;
    jump.size = 1;

    jumps.push_back(jump);
}

void leqCmd(Variable a, Variable b)
{
    a = prepareCond(a, b);
    Variable jump;

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JPOS " + to_string(cmdIndex + 3));
    insertCmd("JZERO " + to_string(cmdIndex + 2));
    insertCmd("JUMP false");
    jump.index = cmdIndex - 1;
    jump.size = 1;

    jumps.push_back(jump);
}

void geqCmd(Variable a, Variable b)
{
    a = prepareCond(a, b);
    Variable jump;

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JNEG " + to_string(cmdIndex + 3));
    insertCmd("JZERO " + to_string(cmdIndex + 2));
    insertCmd("JUMP false");
    jump.index = cmdIndex - 1;
    jump.size = 1;

    jumps.push_back(jump);
}

void ifCmd()
{
    Variable jump = jumps.back();
    jumps.pop_back();
    replaceCmd(jump.index, "JUMP " + to_string(jump.index + jump.size));
}

void elseCmd()
{
    insertCmd("JUMP else");

    Variable jump = jumps.back();
    jumps.pop_back();
    replaceCmd(jump.index, "JUMP " + to_string(jump.index + jump.size));

    Variable newJump;
    newJump.index = cmdIndex - 1;
    newJump.size = 1;

    jumps.push_back(newJump);
}

void forToCmd(char *it, Variable from, Variable to)
{
    if (variables.find(it) != variables.end())
    {
        string message = "Variable \"" + string(it) + string("\" already declared");
        printError(message);
    }

    Variable itr;
    createVariable(&itr, it, "ITR");
    variables.at(it).init = true;

    Variable fin;
    fin.index = memoryIndex;
    memoryIndex++;

    if (to.type == numType)
    {
        createNum(to.value);
    }
    else
    {
        loadVar(to);
    }

    insertCmd("STORE " + to_string(fin.index));

    if (from.type == numType)
    {
        createNum(from.value);
    }
    else
    {
        loadVar(from);
    }

    insertCmd("DEC");
    storeVar(itr);

    Variable jump;

    loadVar(fin);
    insertCmd("SUB " + to_string(itr.index));
    insertCmd("JPOS " + to_string(cmdIndex + 2));
    insertCmd("JUMP end");
    jump.index = cmdIndex - 1;
    jump.size = 1;
    jump.iterator = itr.name;

    jumps.push_back(jump);
    loadVar(itr);
    insertCmd("INC");
    storeVar(itr);
}

void forDowntoCmd(char *it, Variable from, Variable to)
{
    if (variables.find(it) != variables.end())
    {
        string message = "Variable \"" + string(it) + string("\" already declared");
        printError(message);
    }

    Variable itr;
    createVariable(&itr, it, "ITR");
    variables.at(it).init = true;

    Variable fin;
    fin.index = memoryIndex;
    memoryIndex++;

    if (to.type == numType)
    {
        createNum(to.value);
    }
    else
    {
        loadVar(to);
    }

    insertCmd("STORE " + to_string(fin.index));

    if (from.type == numType)
    {
        createNum(from.value);
    }
    else
    {
        loadVar(from);
    }

    insertCmd("INC");
    storeVar(itr);

    Variable jump;

    loadVar(fin);
    insertCmd("SUB " + to_string(itr.index));
    insertCmd("JNEG " + to_string(cmdIndex + 2));
    insertCmd("JUMP end");
    jump.index = cmdIndex - 1;
    jump.size = 1;
    jump.iterator = itr.name;

    jumps.push_back(jump);
    loadVar(itr);
    insertCmd("DEC");
    storeVar(itr);
}

void endForCmd()
{
    Variable jump = jumps.back();
    jumps.pop_back();
    variables.erase(jump.iterator);
    replaceCmd(jump.index, "JUMP " + to_string(jump.index + jump.size + 1));

    insertCmd("JUMP " + to_string(jump.index - 3));
}

void whileCmd()
{
    Variable jump;

    jump.index = cmdIndex;
    jump.size = 1;

    jumps.push_back(jump);
}

void endWhileCmd()
{
    Variable jump = jumps.back();
    jumps.pop_back();
    replaceCmd(jump.index, "JUMP " + to_string(jump.index + jump.size + 1));

    Variable checkPoint = jumps.back();
    jumps.pop_back();
    insertCmd("JUMP " + to_string(checkPoint.index));
}

// FUNCTIONS
void replaceCmd(long long index, string newCmd)
{
    code[index] = newCmd;
}

void createVariable(Variable *var, char *name, char const *type)
{
    var->name = name;
    var->type = type;
    var->init = false;
    var->used = false;
    var->index = memoryIndex;
    memoryIndex++;

    variables.insert({name, *var});
}

void createArray(Variable *var, char *name, long long size, long long start, char const *type)
{
    var->name = name;
    var->type = type;
    var->init = false;
    var->index = memoryIndex;
    var->size = size;
    var->startIdx = start;
    memoryIndex += size;

    variables.insert({name, *var});
}

Variable tempVar(long long a)
{
    Variable var;

    var.type = numType;
    var.value = a;

    return var;
}

Variable prepareCond(Variable a, Variable b)
{
    if (a.type == numType)
    {
        createNum(a.value);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++;
    }
    else if (a.type == arrVarType)
    {
        loadVar(a);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++;
    }

    if (b.type == numType)
    {
        createNum(b.value);
    }
    else
    {
        loadVar(b);
    }

    return a;
}

Variable setUsed(Variable var)
{

    // if (var.type != arrType && var.type != arrVarType && !var.init)
    // {
    //     string message = "Variable \"" + string(var.name) + string("\" was not initialized");
    //     printError(message);
    // }
    if (var.type != arrType && var.type != arrVarType)
    {
        if (variables.at(var.name).used == false)
        {
            variables.at(var.name).line = yylineno;
            variables.at(var.name).used = true;
        }
    }

    return var;
}

Variable getVar(char *name)
{
    if (variables.find(name) == variables.end())
    {
        string message = "Variable \"" + string(name) + string("\" was not declared");
        printError(message);
    }

    Variable var = variables.at(name);

    if (var.type == arrType)
    {
        string message = "Calling array \"" + string(var.name) + string("\" without index.");
        printError(message);
    }

    return var;
}

Variable setArr(char *name, char *id)
{
    if (variables.find(id) == variables.end())
    {
        string message = "Variable \"" + string(id) + string("\" was not declared");
        printError(message);
    }

    Variable a = variables.at(id);

    if ((a.type != arrVarType || a.type != arrType) && !a.init)
    {
        string message = "Variable \"" + string(a.name) + string("\" was not initialized");
        printError(message);
    }
    else
    {
        Variable arr = variables.at(name);

        if (arr.type != arrType)
        {
            string message = "Variable \"" + string(name) + string("\" is not array");
            printError(message);
        }

        Variable element;

        createNum(arr.index - arr.startIdx);
        insertCmd("ADD " + to_string(a.index));
        insertCmd("STORE " + to_string(memoryIndex));
        element.index = memoryIndex;
        element.type = "ARR VAR";
        memoryIndex++;

        return element;
    }

    return a;
}

Variable getArr(char *name, long long idx)
{
    Variable var = variables.at(name);

    if (var.type != arrType)
    {
        string message = "Variable \"" + string(name) + string("\" is not array");
        printError(message);
    }

    long long arrIdx = idx - var.startIdx;

    if (idx == var.startIdx)
    {
        return var;
    }

    if (idx < var.startIdx || idx >= var.startIdx + var.size)
    {
        string message = "Array index \"" + to_string(idx) + string("\" is out of range");
        printError(message);
    }

    Variable element;
    element.type = "ARR";
    element.index = var.index + arrIdx;

    return element;
}

void createNum(long long a)
{

    if (a == 0)
    {
        insertCmd("SUB 0");
        return;
    }
    if (a == 1)
    {
        insertCmd("LOAD 1");
        return;
    }
    else if (a == -1)
    {
        insertCmd("LOAD 2");
        return;
    }

    if (constants.find(a) != constants.end())
    {
        insertCmd("LOAD " + to_string(constants.at(a)));
        return;
    }

    if (llabs(a) > 10000 - 22)
    {
        for (long long i = a - 22; i <= a + 22; i++)
        {
            if (constants.find(i) != constants.end())
            {
                insertCmd("LOAD " + to_string(constants.at(i)));
                string cmd = i < a ? "INC" : "DEC";
                for (long long j = 0; j < llabs(i - a); j++)
                {
                    insertCmd(cmd);
                }
                return;
            }
        }
    }

    string cmd = a > 0 ? "INC" : "DEC";
    resetCurr();

    if (llabs(a) <= 23)
    {
        for (long long i = 0; i < llabs(a); i++)
        {
            insertCmd(cmd);
        }
    }
    else
    {
        list<string> steps = calcSteps(llabs(a), cmd);
        if (a < 0)
        {
            insertCmd("DEC");
        }
        else
        {
            insertCmd("INC");
        }

        for (string s : steps)
        {
            insertCmd(s);
        }
    }

    createConstant(a);
}

void createConstant(long long a)
{
    if (a > 10000)
    {
        insertCmd("STORE " + to_string(memoryIndex));
        constants.insert({a, memoryIndex});
        memoryIndex++;
    }
}

list<string> calcSteps(long long a, string cmd)
{
    list<string> steps;
    while (a != 1)
    {
        if (a % 2 == 0)
        {
            steps.push_front("SHIFT 1");
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
    insertCmd("SUB 0");
}

void loadVar(Variable var)
{
    if (var.type == arrVarType)
    {
        insertCmd("LOADI " + to_string(var.index));
    }
    else
    {
        insertCmd("LOAD " + to_string(var.index));
    }
}

void storeVar(Variable var)
{
    if (var.type == arrVarType)
    {
        insertCmd("STOREI " + to_string(var.index));
    }
    else
    {
        insertCmd("STORE " + to_string(var.index));
    }
}

void insertCmd(string cmd)
{
    code.push_back(cmd);
    cmdIndex++;

    refreshJumps();
}

void refreshJumps()
{
    for (Variable &jump : jumps)
    {
        jump.size += 1;
    }
}

void printError(string message)
{
    cerr << "\n\033[1;31mError in line: " << yylineno << "\033[0m" << endl;
    if (message != "")
    {
        cerr << "\033[1;31m" << message << "\033[0m" << endl;
    }
    cerr << endl;

    exit(EXIT_FAILURE);
}

void checkInit()
{
    map<string, Variable>::iterator it;

    for (it = variables.begin(); it != variables.end(); it++)
    {
        // std::cout << it->first // string (key)
        //           << " used->"
        //           << it->second.used // string's value
        //           << " init->"
        //           << it->second.init
        //           << " line->"
        //           << it->second.line
        //           << std::endl;
        if (it->second.used && !it->second.init && it->second.type != arrType && it->second.type != arrVarType)
        {
            cerr << "\n\033[1;31mError in line: " << yylineno - 1 << "\033[0m" << endl;
            string message = "Variable \"" + string(it->first) + string("\" was not initialized");
            cerr << "\033[1;31m" << message << "\033[0m" << endl
                 << endl;

            exit(EXIT_FAILURE);
        }
    }
}
