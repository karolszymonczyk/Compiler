#include "compiler.hpp"

// W CREATE NUM zrobić szukanie w constants, nie trzeba zmieniać kodu za bardzo

// MOŻNA zrobić funckę to zarządzania pamięcią i po usunięciu iteratora
// skoczyć na jego miejsce i po przypisaniu czegos tam wrócić do normalnego
// ale może nieźle zamieszać XD

//JAK w declaration jest ostatnio to daje do następnej linii błąd?? e1
// jak nie ma zmiennej w declaration to wywala zrzut pamięci xD

// trzeba powkładać do elsów wszystko
// jakieś dziwne równania z tab(a)? np a ASSING tab(a) + a

extern int yylineno;

long long memoryIndex = 3;
long long cmdIndex = 0;
// int loadedIndex = 1; // chyba nie potrzbne jak nie będę cudować z memoryIndex xD
// int currValue = 0; // będzie trzeba jakoś trzymać i zerować po LOAD
// albo zawsze po operacji a zapisywać jak STORE

char const *arrVarType = "ARR VAR";
char const *arrType = "ARR";
char const *varType = "VAR";
char const *numType = "NUM";
char const *itrType = "ITR";

vector<string> code;
map<string, Variable> variables;
map<long long, long long> constants;
// można tworzyć liczby przez zrobienie brakującej i ją dodać lub odjąć
// ITERATORS sb zrobić
vector<Variable> jumps; // vector with push_back pop_back bo można iterować

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
{ // warunek jeśli była zadeklarowana ale nie bd działać dla arr(var)?
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
        insertCmd("PUT"); // MOŻNA ZAPISYWAĆ W getTemp gdzieś te numerki stworzone
    }
    else if (var.type == arrType)
    {
        loadVar(var);
        insertCmd("PUT");
    }
    else if (var.type == arrVarType)
    {
        loadVar(var);
        insertCmd("PUT");
    }
    else if (!var.init)
    { // CHYBA WGL NIE POTRZEBNE BO SPRAWDZAM WCZEŚNIEJ PRZY PIDIDEN
        string message = "Variable \"" + string(var.name) + string("\" was not initialised");
        printError(message);
    }
    else
    {
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

void plusCmd(Variable a, Variable b) //ulepszyć i minus też jak różnica < 10 to dodawać 10razy INC
{
    // dla bezpieczeństwa ale trzeba zoptymalizować XD
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
            if (llabs(a.value) < 10) //  tego IFA DODAŁEM XD
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
            if (llabs(b.value) < 10) //  tego IFA DODAŁEM XD
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
            //todo trzeba sprawdzić czy nie jest aktualnie załadowany i ew zmienić kolejność dodawania
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
    // dla bezpieczeństwa ale trzeba zoptymalizować XD
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
    { // zopytamlizować jak a lub b jest zerem (JEDEN JUMP) i dodawanie też
        if (a.type == numType)
        {
            // if (llabs(a.value) < 10) //  tego IFA DODAŁEM XD
            // {
            //     loadVar(b);
            //     string cmd = a.value < 0 ? "INC" : "DEC";
            //     for (long long i = 0; i < llabs(a.value); i++)
            //     {
            //         insertCmd(cmd);
            //     }
            // }
            // else
            // {
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
            // }
        }
        else if (b.type == numType)
        {
            if (llabs(b.value) < 10) //  tego IFA DODAŁEM XD
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
            //todo trzeba sprawdzić czy nie jest aktualnie załadowany i ew zmienić kolejność dodawania
        }
    }
}

void timesCmd(Variable a, Variable b) // nie zmieniane jeszcze (dodać laodVar storeVar)
{
    // dla bezpieczeństwa ale trzeba zoptymalizować XD

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
    { //dwa razy jest sub 0 bez sensu (to tutaj i z createNum)

        // resetCurr();
        // // STORE 5 #wynik
        // insertCmd("STORE " + to_string(memoryIndex)); // 5
        // // STORE 6 #flaga
        // insertCmd("STORE " + to_string(memoryIndex + 1)); // 6

        // resetCurr(); // nie jest potrzebne CHYBA XD

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
        if (a.value * b.value < 0) {
            cout << a.value * b.value << endl;
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
        insertCmd("JZERO " + to_string(cmdIndex + 62)); // XXX +6
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("JPOS " + to_string(cmdIndex + 8));
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 3));
        insertCmd("LOAD " + to_string(memoryIndex + 1));
        insertCmd("INC");
        insertCmd("STORE " + to_string(memoryIndex + 1));
        insertCmd("LOAD " + to_string(memoryIndex + 3));
        insertCmd("STORE " + to_string(memoryIndex + 4)); // reszta
        loadVar(b);
        insertCmd("JZERO " + to_string(cmdIndex + 50)); // XXX +6
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
        insertCmd("JNEG " + to_string(cmdIndex + 10)); // XXX + 5
        insertCmd("JPOS " + to_string(cmdIndex + 9)); // XXX + 5
        resetCurr();
        insertCmd("SUB " + to_string(memoryIndex));
        insertCmd("STORE " + to_string(memoryIndex)); /// XXX
        insertCmd("LOAD " + to_string(memoryIndex + 4)); // XXX reszta
        insertCmd("JNEG " + to_string(cmdIndex + 4)); // XXX do LOAD 1
        insertCmd("LOAD " + to_string(memoryIndex)); // XXX wynik
        insertCmd("DEC"); // XXX
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

// pierwsze wejście true, drugie false
void eqCmd(Variable a, Variable b)
{
    //MOŻNA to uprościć jakoś

    if (a.type == numType)
    {
        createNum(a.value);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }
    else if (a.type == arrVarType)
    {
        loadVar(a);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }

    Variable jump;

    if (b.type == numType)
    {
        createNum(b.value); // poprawić jak będę zapisywał stałe
    }
    else
    {
        loadVar(b);
    }

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JZERO " + to_string(cmdIndex + 2)); // JUMP TRUE
    // insertCmd("JUMP " + to_string(cmdIndex + 2));  // JUMP FALSE
    insertCmd("JUMP false");   // JUMP FALSE ZROBIĆ DO TEGO FUNKCJE
    jump.index = cmdIndex - 1; // gdzie jest jump
    jump.size = 1;

    jumps.push_back(jump);
}

void neqCmd(Variable a, Variable b)
{
    if (a.type == numType)
    {
        createNum(a.value);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }
    else if (a.type == arrVarType)
    {
        loadVar(a);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }

    Variable jump;

    if (b.type == numType)
    {
        createNum(b.value); // poprawić jak będę zapisywał stałe
    }
    else
    {
        loadVar(b);
    }

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JNEG " + to_string(cmdIndex + 3)); // JUMP TRUE
    insertCmd("JPOS " + to_string(cmdIndex + 2)); // JUMP TRUE
    insertCmd("JUMP false");                      // JUMP FALSE ZROBIĆ DO TEGO FUNKCJE
    jump.index = cmdIndex - 1;                    // gdzie jest jump
    jump.size = 1;

    jumps.push_back(jump);
}

void leCmd(Variable a, Variable b)
{
    if (a.type == numType)
    {
        createNum(a.value);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }
    else if (a.type == arrVarType)
    {
        loadVar(a);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }

    Variable jump;

    if (b.type == numType)
    {
        createNum(b.value); // poprawić jak będę zapisywał stałe
    }
    else
    {
        loadVar(b);
    }

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JPOS " + to_string(cmdIndex + 2)); // JUMP TRUE
    insertCmd("JUMP false");                      // JUMP FALSE ZROBIĆ DO TEGO FUNKCJE
    jump.index = cmdIndex - 1;                    // gdzie jest jump
    jump.size = 1;

    jumps.push_back(jump);
}

void geCmd(Variable a, Variable b)
{
    if (a.type == numType) // JUŻ KTÓRYŚ RAZ TO WKLEJAM XD
    {
        createNum(a.value);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }
    else if (a.type == arrVarType)
    {
        loadVar(a);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }

    Variable jump;

    if (b.type == numType) // TO TEŻ XD
    {
        createNum(b.value); // poprawić jak będę zapisywał stałe
    }
    else
    {
        loadVar(b);
    }

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JNEG " + to_string(cmdIndex + 2)); // JUMP TRUE
    insertCmd("JUMP false");                      // JUMP FALSE ZROBIĆ DO TEGO FUNKCJE
    jump.index = cmdIndex - 1;                    // gdzie jest jump
    jump.size = 1;

    jumps.push_back(jump);
}

void leqCmd(Variable a, Variable b)
{
    if (a.type == numType) // JUŻ KTÓRYŚ RAZ TO WKLEJAM XD
    {
        createNum(a.value);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }
    else if (a.type == arrVarType)
    {
        loadVar(a);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }

    Variable jump;

    if (b.type == numType) // TO TEŻ XD
    {
        createNum(b.value); // poprawić jak będę zapisywał stałe
    }
    else
    {
        loadVar(b);
    }

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JPOS " + to_string(cmdIndex + 3)); // JUMP TRUE
    insertCmd("JZERO " + to_string(cmdIndex + 2));
    insertCmd("JUMP false");   // JUMP FALSE ZROBIĆ DO TEGO FUNKCJE
    jump.index = cmdIndex - 1; // gdzie jest jump
    jump.size = 1;

    jumps.push_back(jump);
}

void geqCmd(Variable a, Variable b)
{
    if (a.type == numType) // JUŻ KTÓRYŚ RAZ TO WKLEJAM XD
    {
        createNum(a.value);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }
    else if (a.type == arrVarType)
    {
        loadVar(a);
        insertCmd("STORE " + to_string(memoryIndex));
        a.index = memoryIndex;
        memoryIndex++; // tutaj trzeba bo przy pętlach by się wywróciło
    }

    Variable jump;

    if (b.type == numType) // TO TEŻ XD
    {
        createNum(b.value); // poprawić jak będę zapisywał stałe
    }
    else
    {
        loadVar(b);
    }

    insertCmd("SUB " + to_string(a.index));
    insertCmd("JNEG " + to_string(cmdIndex + 3)); // JUMP TRUE
    insertCmd("JZERO " + to_string(cmdIndex + 2));
    insertCmd("JUMP false");   // JUMP FALSE ZROBIĆ DO TEGO FUNKCJE
    jump.index = cmdIndex - 1; // gdzie jest jump
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
    newJump.index = cmdIndex - 1; // gdzie jest jump
    newJump.size = 1;

    jumps.push_back(newJump);
}

void forToCmd(char *it, Variable from, Variable to) // jakąś cześć wspólną dać do funkcji tych FORÓW??
{
    // dodać warunki jak dwie liczby ITD.

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
    jump.index = cmdIndex - 1; // gdzie jest jump
    jump.size = 1;
    jump.iterator = itr.name; // DODANE

    jumps.push_back(jump);
    loadVar(itr);
    insertCmd("INC"); // bo TO
    storeVar(itr);
}

void forDowntoCmd(char *it, Variable from, Variable to)
{
    // dodać warunki jak dwie liczby ITD.

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
    insertCmd("JNEG " + to_string(cmdIndex + 2)); // R
    insertCmd("JUMP end");
    jump.index = cmdIndex - 1; // gdzie jest jump
    jump.size = 1;
    jump.iterator = itr.name; // DODANE

    jumps.push_back(jump);
    loadVar(itr);
    insertCmd("DEC"); // bo DONTO  Różnica
    storeVar(itr);
}

void endForCmd()
{
    Variable jump = jumps.back();
    jumps.pop_back();
    variables.erase(jump.iterator); // DODANE
    replaceCmd(jump.index, "JUMP " + to_string(jump.index + jump.size + 1));

    insertCmd("JUMP " + to_string(jump.index - 3)); //skok do load(itr) ZMIANA
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

void doCmd() // to samo co whileCmd
{
    Variable jump;

    jump.index = cmdIndex;
    jump.size = 1;

    jumps.push_back(jump);
}

void endDoCmd() // to samo co endWhileCmd
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

Variable checkInit(Variable var)
{

    if (var.type != arrType && var.type != arrVarType && !var.init)
    {
        string message = "Variable \"" + string(var.name) + string("\" was not initialized");
        printError(message);
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

    Variable var = variables.at(name); // można chyba od razu to zwrócić?

    if (var.type == arrType)
    {
        string message = "Calling array \"" + string(var.name) + string("\" without index.");
        printError(message);
    }

    return var;
}

Variable setArr(char *name, char *id) // sprawdzać czy jest w variables?
{
    if (variables.find(id) == variables.end())
    {
        string message = "Variable \"" + string(id) + string("\" was not declared");
        printError(message);
    }

    Variable a = variables.at(id);

    if ((a.type != arrVarType || a.type != arrType) && !a.init) // chyba nie dotyczy tablic
    {
        string message = "Variable \"" + string(a.name) + string("\" was not initialized");
        printError(message);
    }
    else
    {
        Variable arr = variables.at(name);

        if (arr.type != arrType) // dodać arrVar ??
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

    return a; //zastąpić nullem?
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

    // string newName = var.name + to_string(arrIdx);
    // Variable element = variables.at(newName);
    Variable element;
    element.type = "ARR";
    element.index = var.index + arrIdx;

    return element;
}

void createNum(long long a)
{

    if (a == 0)
    { // jak nie będzie coś działało to usunąć korzystanie z zapisanych const
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

    // DODAĆ JESZCZE W zasięgu 10? albo wgl najbliższą i na niej jakoś bazować
    if (constants.find(a) != constants.end())
    {
        // cout << yylineno << " : LOAD " << a << " from index " << to_string(constants.at(a)) << endl;
        insertCmd("LOAD " + to_string(constants.at(a)));
        return;
    }

    if (llabs(a) > 10000 - 22) //PRZEZ TO MOŻE NIE DZIAŁAĆ (CAŁE XD) 70
    {
        for (long long i = a - 22; i <= a + 22; i++)
        {
            // long long diff = a + i;
            if (constants.find(i) != constants.end()) // to samo co u góry?
            {
                // cout << yylineno << " : LOAD " << i << " from index " << to_string(constants.at(a)) << endl;
                insertCmd("LOAD " + to_string(constants.at(i)));
                string cmd = i < a ? "INC" : "DEC";
                cout << i << endl;
                for (long long j = 0; j < llabs(i - a); j++)
                {
                    insertCmd(cmd);
                }
                return;
            }
        }
    }

    string cmd = a > 0 ? "INC" : "DEC";
    resetCurr(); //tymczasowe
    // później sprawdzanie co jest na stosie i czy jest większe czy mniejsze itd
    // można przy zapisywaniu zmiennych dodawać im też value i jak się różnią
    // o odpowiednią ilość to ladować najbliższą i odpowiednio znajdywać

    // if (a == 0)
    // {
    //     createConstant(a);
    //     return;
    // }

    if (llabs(a) <= 23)
    {
        for (long long i = 0; i < llabs(a); i++)
        {
            insertCmd(cmd);
        }
    }
    else
    {
        // insertCmd("INC");
        // insertCmd("STORE " + to_string(memoryIndex));
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
    // cout << yylineno << " : STORE " << a  << " on index " << memoryIndex << endl;
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
    // currValue = 0;
}

void loadVar(Variable var)
{
    // if (currValue != var.value)
    // {
    // currValue = var.value;
    // }
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

    exit(EXIT_FAILURE); // EXIT_SUCCESS
}
