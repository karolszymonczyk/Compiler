%option noyywrap
%option nounput
%option yylineno

%{
#include "parser.tab.h"
#include <iostream>
#include <string>
%}

identifier          ([_a-z])+
number              ("-"?[0-9])+
whitespace          ([ \t\r\n])+
semicolon           ";"
colon               ":"
comma               ","
l_bracket           "("
r_bracket           ")"

%x comment

%%
"DECLARE"           { return DECLARE; }
"BEGIN"             { return BEGINN; }
"END"               { return END; }

"ASSIGN"            { return ASSIGN; }
"READ"              { return READ; }
"WRITE"             { return WRITE; }
"PLUS"              { return PLUS; }
"MINUS"             { return MINUS; }
"TIMES"             { return TIMES; }
"DIV"               { return DIV; }
"MOD"               { return MOD; }

"EQ"                { return EQ; }
"NEQ"               { return NEQ; }
"LE"                { return LE; }
"GE"                { return GE; }
"LEQ"               { return LEQ; }
"GEQ"               { return GEQ; }

{number}	        {  yylval.num = std::stoll(yytext); return num; }
{identifier}        {
                         
                        yylval.str = (char *)strdup(yytext);
                        return pidentifier;
                    }
{semicolon}         { return SEMICOLON; }
{colon}             { return COLON; }
{comma}             { return COMMA; }
{l_bracket}         { return LBR; }
{r_bracket}         { return RBR; }

"["                 { BEGIN(comment); }
<comment>{
    "]"             { BEGIN(INITIAL); }
    (\n|.)          
    <<EOF>>         { printf("Error: comment closure missing \"]\"\n"); yyterminate(); }
}

{whitespace}        ;
.                   { return ERROR; }
%%
