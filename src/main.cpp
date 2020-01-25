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
        cerr << "\033[1;31mError\033[0m incorrect args number" << endl;
        exit(1);
    }

    string in_filename = argv[1];
    string out_filename = argv[2];

    yyin = read(in_filename);

    if (yyin == NULL)
    {
        exit(1);
    }

    cout << "\n\33[1;37mCompiling...\33[0m" << endl;

    int result = yyparse();

    write(out_filename, code);

    if (result == 0)
    {
        cout << "\n\033[1;32mCompilation completed successfully!\033[0m" << endl;
        cout << "\33[1;37mExecuted lines: " << yylineno << "\33[0m\n"
             << endl;
    }
    else
    {
        cout << "\n\033[1;31mCompilation error\033[0m\n"
             << endl;
             exit(1);
    }

    return 0;
}