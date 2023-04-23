%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"

void llerror(char const *msg){
	printf("\nLexer error: %s\n", msg);
	exit(EXIT_FAILURE);
}

char* strToLower(char* str){
	char* ret = malloc(strlen(str)+1);
	strcpy(ret, str);
	for(int i = 0; '\0' != ret[i]; i++){ ret[i] = tolower(ret[i]); }
	return ret;
}

%}
%option noyywrap
%option caseless
%x COMMENT

alpha [_a-zA-Z]
digit [0-9]
printable [ -&(-~]

%%

[ \t\n\r]+ /* ignore whitespaces */

[(][*][^*]*[*]+([^*)][^*]*[*]+)*[)]  { /* Ignore Comment */ }
[(][*]                               { llerror("Comment not terminated"); } /* Error: comment not terminated */

program   { return TOK_PROGRAM;                                                  } /* Reserved Keyowrds */
function  { return TOK_FUNCTION;                                                 }
procedure { return TOK_PROCEDURE;                                                }
const     { return TOK_CONST;                                                    }
var       { return TOK_VAR;                                                      }
begin     { return TOK_BEGIN;                                                    }
end       { return TOK_END;                                                      }
if        { return TOK_IF;                                                       }
then      { return TOK_THEN;                                                     }
else      { return TOK_ELSE;                                                     }
while     { return TOK_WHILE;                                                    }
do        { return TOK_DO;                                                       }
for       { return TOK_FOR;                                                      }
to        { return TOK_TO;                                                       }
true      { yylval.typeStr = strToLower(strdup(yytext)); return TOK_BOOL;        }
false     { yylval.typeStr = strToLower(strdup(yytext)); return TOK_BOOL;        }
integer   { yylval.typeStr = strToLower(strdup(yytext)); return TOK_TYPE_INT;    }
boolean   { yylval.typeStr = strToLower(strdup(yytext)); return TOK_TYPE_BOOL;   }
string    { yylval.typeStr = strToLower(strdup(yytext)); return TOK_TYPE_STRING; }
array     { return TOK_ARR;                                                      }
of        { return TOK_OF;                                                       }
break     { yylval.typeStr = strToLower(strdup(yytext));return TOK_BREAK;        }

"," { return TOK_COMMA;     } /* Ponctuation Signs */
"." { return TOK_PERIOD;    }
":" { return TOK_COLON;     }
";" { return TOK_SEMICOLON; }
"(" { return TOK_LPAREN;    }
")" { return TOK_RPAREN;    }
"[" { return TOK_LBRAC;     }
"]" { return TOK_RBRAC;     }

"+"  { return TOK_BINOP_PLUS;  } /* Operators */
"-"  { return TOK_BINOP_MINUS; }
"*"  { return TOK_BINOP_MULT;  }
div  { return TOK_BINOP_DIV;   }
mod  { return TOK_BINOP_MOD;   }
"="  { return TOK_BINOP_EQ;    }
"<>" { return TOK_BINOP_NOTEQ; }
"<"  { return TOK_BINOP_LT;    }
"<=" { return TOK_BINOP_LEQT;  }
">"  { return TOK_BINOP_GT;    }
">=" { return TOK_BINOP_GEQT;  }
and  { return TOK_BINOP_AND;   }
or   { return TOK_BINOP_OR;    }
not  { return TOK_UNOP_NOT;    }
":=" { return TOK_ASSIGN;      }


{alpha}({alpha}|{digit})* {  yylval.typeStr = strToLower(strdup(yytext)); return TOK_ID; }

{digit}+ { yylval.typeInt = atoi(strdup(yytext)); return TOK_NUM; }

[']({printable})*['] { yylval.typeStr = strdup(yytext); return TOK_STRING; }

. { llerror("Invalid character detected"); } /* Error: invalid charecter */

<<EOF>> { return EOF; /* end of input */ }
%%

