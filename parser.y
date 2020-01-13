%{
#include <iostream>
#include <string>
#include <memory>
#include <vector>

using namespace std;

int yylex();
int yyparse();
void yyerror(string);
extern FILE *yyin; // nie potrzebne? posprzątać tu
extern int yylineno;
%}

%code requires {
    #include "compiler.hpp"
}

%union {
    char *str;
    struct Variable var;
}

%token DECLARE BEGINN END
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE ENDDO FOR FROM ENDFOR
%token TO DOWNTO
%token LE GE LEQ GEQ EQ NEQ
%token ASSIGN
%token READ WRITE
%token ERROR
%token <str> pidentifier
%token <str> num
%token SEMICOLON COLON COMMA LBR RBR

%left PLUS MINUS
%left TIMES DIV MOD

%type <var> identifier
%type <var> value

%start program

%%
program:            DECLARE declarations                                        { startProgram(); }
                    BEGINN commands END                                         { endProgram(); }
                    | BEGINN commands END                                       { endProgram(); }
;

declarations:       declarations COMMA pidentifier                              { declareVar($3); }
                    | declarations COMMA pidentifier LBR num COLON num RBR      { declareArr($3, stoll($5), stoll($7)); }
                    | pidentifier                                               { declareVar($1); }
                    | pidentifier LBR num COLON num RBR                         { declareArr($1, stoll($3), stoll($5)); }
;

commands:           commands command
                    | command
;

command:            identifier ASSIGN expression SEMICOLON                          { initVar($1); }
                    | IF condition THEN commands ELSE                               { elseCmd(); }
                    commands ENDIF                                                  { ifCmd(); }
                    | IF condition THEN commands ENDIF                              { ifCmd(); }
                    | WHILE                                                         { whileCmd(); }
                    condition DO commands ENDWHILE                                  { endWhileCmd(); }
                    | DO                                                            { doCmd(); }
                    commands WHILE condition ENDDO                                  { endDoCmd(); }
                    | FOR pidentifier FROM value TO value DO                        { forToCmd($2, $4, $6); }
                    commands ENDFOR                                                 { endForCmd(); }
                    | FOR pidentifier FROM value DOWNTO value DO                    { forDowntoCmd($2, $4, $6); }
                    commands ENDFOR                                                 { endForCmd(); }
                    | READ identifier SEMICOLON                                     { readCmd($2); }
                    | WRITE value SEMICOLON                                         { writeCmd($2); }
;

expression:         value                                                           { valCmd($1); }
                    | value PLUS value                                              { plusCmd($1, $3); }
                    | value MINUS value                                             { minusCmd($1, $3); }
                    | value TIMES value                                             { timesCmd($1, $3); }
                    | value DIV value                                               { divCmd($1, $3); }
                    | value MOD value                                               { modCmd($1, $3); }
;

condition:          value EQ value                                                  { eqCmd($1, $3); }
                    | value NEQ value                                               { neqCmd($1, $3); }
                    | value LE value                                                { leCmd($1, $3); }
                    | value GE value                                                { geCmd($1, $3); }
                    | value LEQ value                                               { leqCmd($1, $3); }
                    | value GEQ value                                               { geqCmd($1, $3); }
;

value:               num                                                            { $$ = tempVar(stoll($1)); }
                    | identifier                                                    { $$ = checkInit($1); }
;

identifier:         pidentifier                                                     { $$ = getVar($1); }
                    | pidentifier LBR pidentifier RBR                               { $$ = setArr($1, $3); }
                    | pidentifier LBR num RBR                                       { $$ = getArr($1, stoll($3)); }
;
    
%%

void yyerror(string s) {

    string message = "Unknown token: \"" + string(yylval.str) + string("\"\n");
    printError(message);
}
