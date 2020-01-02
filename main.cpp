#include <iostream>
#include <string>

#include "iofile.hpp"
#include "compiler.hpp"

extern int yyparse();
extern int yylineno;
extern FILE *yyin;

using namespace std;

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        cerr << "incorrect args number" << endl;
        exit(1);
    }

    string in_filename = argv[1];
    string out_filename = argv[2];

    yyin = read(in_filename);

    if (yyin == NULL)
    {
        exit(1);
    }

    cout << "\nCompiling..." << endl;

    int result = yyparse();

    write(out_filename, code);

    if (result == 0)
    {
        cout << "\nCompiling successfull!" << endl;
        cout << "Executed lines: " << yylineno << "\n"
             << endl;
    }
    else
    {
        cout << "\nCompiling error.\n"
             << endl;
             exit(1);
    }

    return 0;
}