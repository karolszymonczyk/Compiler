%{
#include <iostream>
#include <string>
#include <memory>
#include <vector>

// #include "compiler.hpp"

using namespace std;

int yylex();
int yyparse();
void yyerror(string);
extern FILE *yyin;
extern int yylineno;
%}

%code requires {
    #include "compiler.hpp"
}

%union {
    char *str;
    long long num;
    struct Variable var;
}

%token DECLARE BEGINN END
%token LE GE LEQ GEQ EQ NEQ
%token ASSIGN
%token READ WRITE
%token ERROR
%token <str> pidentifier
%token <num> num
%token SEMICOLON COLON COMMA LBR RBR

%left PLUS MINUS
%left TIMES DIV MOD

%type <str> identifier
%type <var> value

%start program

%%
program:            DECLARE declarations 
                    BEGINN commands END                                         { endProgram(); }
                    | BEGINN commands END                                       { endProgram(); }
;

declarations:       declarations COMMA pidentifier                              { declareVar($3); }
                    | declarations COMMA pidentifier LBR num COLON num RBR
                    | pidentifier                                               { declareVar($1); }
                    | pidentifier LBR num COLON num RBR
;

commands:           commands command
                    | command
;

command:            identifier ASSIGN expression SEMICOLON                      { initVar($1); }
                    | READ identifier SEMICOLON                                 { readCmd($2); }
                    | WRITE identifier SEMICOLON                                { writeCmd($2); }
;

expression:         value                                                       { valCmd($1); }
                    | value PLUS value                                          { plusCmd($1, $3); }
                    | value MINUS value                                         { minusCmd($1, $3); }
                    | value TIMES value                                         { timesCmd($1, $3); }
                    | value DIV value
                    | value MOD value
;

condition:          value EQ value
                    | value NEQ value
                    | value LE value
                    | value GE value
                    | value LEQ value
                    | value GEQ value
;

value:               num                                                         { $$ = tempVar($1); }
                    | identifier                                                 { $$ = tempVar($1); }
;

identifier:         pidentifier 
                    | pidentifier LBR pidentifier RBR
                    | pidentifier LBR num RBR
;
    
%%

void yyerror(string) {
    // cerr << "Error in line: " << yylineno - 1 << "\n" << endl;
    // jeśli błąd na końcu linii to yylineno - 1
    cerr << "Error in line: " << yylineno << "\n" << endl;
    exit(1);
}
