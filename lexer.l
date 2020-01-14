%option noyywrap
%option nounput
%option yylineno

%{
#include "parser.tab.h"
#include <iostream>
#include <string>
%}

pidentifier         ([_a-z])+
number              ("-"?([0-9])+)
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

"IF"                { return IF; }
"THEN"              { return THEN; }
"ELSE"              { return ELSE; } 
"ENDIF"             { return ENDIF; }

"WHILE"             { return WHILE; }
"DO"                { return DO; }
"ENDWHILE"          { return ENDWHILE; }
"ENDDO"             { return ENDDO; }

"FOR"               { return FOR; }
"FROM"              { return FROM; }
"TO"                { return TO; }
"DOWNTO"            { return DOWNTO; }
"ENDFOR"            { return ENDFOR; }

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

{number}	        {  
                        yylval.str = (char *)strdup(yytext);
                        return num;
                    }
{pidentifier}       {     
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
    (\n|.)          ;
    <<EOF>>         { printf("Error: comment closure missing \"]\"\n"); yyterminate(); }
}

{whitespace}        ;
.                   { yylval.str = (char *)strdup(yytext); return ERROR; }
%%
