#include <stdio.h>
#include <stdlib.h>

#define MAJOR 0
#define MINOR 5

/*

Current bugs:
- Unar processing
- Error codes (wrong missed symbol showing)
- Lexcical analyzer ids computing
- Comma delimeter in structures
*/

/*

Current todo's:
- Add pointers
- Add comments to the grammar functions
- Add more error codes
- Add def grammar functions
- Add arrays computing
- Add short if-else statement computing
+ Add terminal symbols to syntax tree
*/

typedef enum { false, true } bool; // Не выпендриваюсь особо

// enum for argv
enum {
	NO,
	LEX,
	SYNT,
	UNKNOWN
};

// syntax error codes
enum {
	UNKN,
	WRONG_VAR,
	LPAR_MISS,
	RPAR_MISS,
	LBRA_MISS,
	RBRA_MISS,
	SEMI_MISS,
	DO_MISS,
	WHILE_MISS,
	IF_MISS
};

char *charErr(int code) {
	switch(code) {
		case UNKN:   return "Unknown error";
		case WRONG_VAR: return "Wrong variable name";
		case LPAR_MISS: return "'(' required";
		case RPAR_MISS: return "')' required";
		case LBRA_MISS: return "'{' required";
		case RBRA_MISS: return "'}' required";
		case SEMI_MISS: return "';' required";
		default:        return "(unknown error code)";
	}
}

int ch = ' '; // current character
int lex; // type of the lexeme
int int_val; // value of integer
char id_name[64]; // variable name (max length = 64 characters)
int line = 1; // current line
FILE *fp; // source file (c++ file for parsing)
int type = NO; // NO, LEX, SYNT or UNKNOWN

/* Lexical analyzer */

enum {
	TYPE_INT, TYPE_REAL, TYPE_BOOL, TYPE_ENUM, TYPE_STRUCT, TYPE_POINTER,
	NEW, DELETE,
	CIN, COUT,
	DO, WHILE,
	IF, ELSE,
	TYPE_VOID,
	MAIN, RETURN,
	TYPE_CONST,
	PLUSEQ, MINUSEQ, MULTEQ, DEREQ,
	INCR, DECR,
	AND, OR,
	LBRA, RBRA, 
	LPAR, RPAR,
	PLUS, MINUS, MULTIPLY, DERIVE, 
	LESS, MORE, 
	SEMI,
	IS, 
	EQUAL, 
	INT, REAL, BOOL, 
	ID,
	COMMA,
	EOI
};

char *words[] = {
	"int", "real", "bool", "enum",
	"struct", "pointer", 
	"new", "delete",
	"cin", "cout",
	"do", "while",
	"if", "else",
	"void", "main",
	"return",
	"const",
	"comma",
	NULL
};

char *charLexeme(int lex) {
	switch(lex) {
		case TYPE_INT: return "TYPE INT";
		case TYPE_REAL: return "TYPE REAL";
		case TYPE_BOOL: return "TYPE BOOL";
		case TYPE_ENUM: return "TYPE ENUM";
		case TYPE_STRUCT: return "TYPE STRUCT";
		case TYPE_POINTER: return "TYPE POINTER";
		case NEW: return "NEW";
		case DELETE: return "DELETE";
		case PLUSEQ: return "PLUSEQ";
		case MINUSEQ: return "MINUSEQ";
		case MULTEQ: return "MULTEQ";
		case DEREQ: return "DEREQ";
		case INCR: return "INCR";
		case DECR: return "DECR";
		case CIN: return "CIN";
		case COUT: return "COUT";
		case AND: return "AND";
		case OR: return "OR";
		case DO: return "DO";
		case ELSE: return "ELSE";
		case IF: return "IF";
		case TYPE_VOID: return "TYPE VOID";
		case MAIN: return "MAIN";
		case RETURN: return "RETURN";
		case TYPE_CONST: return "TYPE CONST";
		case WHILE: return "WHILE";
		case LBRA: return "LBRA";
		case RBRA: return "RBRA";
		case LPAR: return "LPAR";
		case RPAR: return "RPAR";
		case PLUS: return "PLUS";
		case MINUS: return "MINUS";
		case MULTIPLY: return "MULTIPLY";
		case DERIVE: return "DERIVE";
		case LESS: return "LESS";
		case MORE: return "MORE";
		case SEMI: return "SEMI";
		case IS: return "IS";
		case EQUAL: return "EQUAL";
		case INT: return "INT";
		case REAL: return "REAL";
		case BOOL: return "BOOL";
		case ID: return "ID";
		case COMMA: return "COMMA";
		case EOI: return "EOI";
		default: return "Undefined lexeme\n";
	}
}

void syntErr() {
	fprintf(stderr, "syntax error\n");
	exit(1);
}

void lexErrCode(int code) {
	fprintf(stderr, "Lexical error: %s (Line: %d)\n", charErr(code), line);
	exit(1);
}

void syntErrCode(int code) {
	fprintf(stderr, "Syntax error: %s (Line: %d)\n", charErr(code), line);
	exit(1);
}

void nextChar() {
	ch = getc(fp);
}

/*
int delimeters[] = {
	LPAR, RPAR, LBRA, RBRA, PLUS, MINUS, PLUSEQ, MINUSEQ, INCR, DECR, MULTIPLY, DERIVE, MULTEQ, DEREQ, EQUAL, SEMI
};
*/

/*
bool inArray(int* arr, int size, int num) {
	int i = 0;
	for (i = 0; i < size; ++i) {
		if (arr[i] == num) return true;
	}
	return false;
}
*/

char delimeters[] = { // 11 elements
	"(){}+-*/=;,\0"
};

bool inArray(char arr[], char chr) {
	int i = 0;
	while (arr[i] != '\0') {
		if (arr[i] == chr) return true;
		++i;
	}
	return false;
}

void nextLexeme() {
	int action = 1;
		while (action) {
			action = 0;
			switch(ch) {
				case '\n':
					nextChar();
					++line;
					action = 1;
					break;
				case ' ': case 9: // 9 is tab 
					nextChar();
					action = 1;
					break;
				case EOF:
					lex = EOI;
					if (type == LEX) return;
					else break;
				case '{':
					nextChar(); 
					lex = LBRA;
					break;
				case '}':
					nextChar();
					lex = RBRA;
					break;
				case '(':
					nextChar();
					lex = LPAR;
					break;
				case ')':
					nextChar();
					lex = RPAR;
					break;
				case '+':
					nextChar();
					if (ch == '=') {
						lex = PLUSEQ;
						nextChar();
					}
					else if (ch == '+') {
						lex = INCR;
						nextChar();
					}
					else {
						lex = PLUS;
					}
					break;
				case '-':
					nextChar();
					if (ch == '=') {
						lex = MINUSEQ;
						nextChar();
					}
					else if (ch == '-') {
						lex = DECR;
						nextChar();
					}
					else {
						lex = MINUS;
					}
					break;
				case '*':
					nextChar();
					// BUG: only character variable names
					/*
					if (ch >= 'a' && ch <= 'z') {
						while (ch >= 'a' && ch <= 'z') {
							nextChar();
						}
						if (inArray(delimeters, ch)) {
							lex = TYPE_POINTER;
						}
					}
					*/
					if (ch != '=') lex = MULTIPLY;
					else {
						lex = MULTEQ;
						nextChar();
					}
					break;
				case '/':
					nextChar();
					if (ch != '=') lex = DERIVE;
					else {
						lex = DEREQ;
						nextChar();
					}
					break;
				case '<':
					nextChar();
					lex = LESS;
					break;
				case '>':
					nextChar();
					lex = MORE;
					break;
				case ';':
					nextChar();
					lex = SEMI;
					break;
				case '=':
					nextChar();
					if (ch == '=') {
						lex = EQUAL;
						nextChar();
					}
					else {
						lex = IS;					
					}
					break;
				case ',':
					nextChar();
					lex = COMMA;
					break;
				default:
					if (ch >= '0' && ch <= '9') {
						int_val = 0;
						while (ch >= '0' && ch <= '9') {
							int_val = int_val*10 + (ch - '0');
							nextChar();
						}
						if (!inArray(delimeters, ch)) lexErrCode(WRONG_VAR);
						lex = INT;
					}
					else if (ch >= 'a' && ch <= 'z') {
						int i = 0;
						while ((ch >= 'a' && ch <= 'z') || ch == '_') {
							id_name[i++] = ch;
							nextChar();
						}
						if (ch >= '0' && ch <= '9') syntErrCode(8);
						id_name[i] = '\0';
						lex = 0;
						while (words[lex] != NULL && strcmp(words[lex], id_name) != 0) {
							++lex;
						}
						if (words[lex] == NULL) {
							if (id_name[i] == '\0') {
								if (!strcmp(id_name, "true") || !strcmp(id_name, "false")) lex = BOOL;
								else lex = ID;
							}
							else {
								syntErrCode(1);
							}
						}
					}
					else if (ch == '&') {
						nextChar();
						if (ch == '&') {
							id_name[0] = '&';
							id_name[1] = '&';
							lex = AND;
							nextChar();
						}
						else {
							syntErrCode(1);
						}
					}
					else if (ch == '|') {
						nextChar();
						if (ch == '|') {
							id_name[0] = '|';
							id_name[1] = '|';
							lex = OR;
							nextChar();
						}
						else {
							syntErrCode(1);
						}
					}
					else {
						printf("ch == %c", ch);
					}
			}
		}
		
		if (type == LEX) {	
			if (lex == INT) printf("INT: %d\n", int_val);
			else if (lex == BOOL) printf("BOOL: %s\n", id_name);
			else if (lex == ID) printf("ID: %s\n", id_name);
			else printf("%s\n", charLexeme(lex));		
			nextLexeme();
		}
}

/* Syntax analyzer */

enum {
	_ID,
	_PLUS, _MINUS, _MULTIPLY, _DERIVE,
	_AND, _OR,
	_EQUAL,
	_IS,
	_LESS, _MORE,
	_IF1, _IF2,
	_DO, _WHILE,
	_VOID, _INT, _REAL, _STRUCT, _POINTER,
	_TYPE_VOID, _TYPE_INT, _TYPE_REAL, _TYPE_STRUCT, _TYPE_POINTER, _TYPE_CONST,
	_NEW, _DELETE,
	_LBRA, _RBRA, _LPAR, _RPAR,
	_SEQ,
	_EMPTY,
	_MAIN,
	_PROG
};

char *charNode(int kind) {
	switch(kind) {
		case _ID: return "_ID";
		case _PLUS: return "_PLUS";
		case _MINUS: return "_MINUS";
		case _MULTIPLY: return "_MULTIPLY";
		case _DERIVE: return "_DERIVE";
		case _AND: return "_AND";
		case _OR: return "_OR";
		case _EQUAL: return "_EQUAL";
		case _IS: return "_IS";
		case _LESS: return "_LESS";
		case _MORE: return "_MORE";
		case _IF1: return "_IF1";
		case _IF2: return "_IF2";
		case _DO: return "_DO";
		case _WHILE: return "_WHILE";
		case _VOID: return "_VOID";
		case _INT: return "_INT";
		case _REAL: return "_REAL";
		case _STRUCT: return "_STRUCT";
		case _POINTER: return "_POINTER";
		case _TYPE_VOID: return "_TYPE_VOID";
		case _TYPE_INT: return "_TYPE_INT";
		case _TYPE_REAL: return "_TYPE_REAL";
		case _TYPE_STRUCT: return "_TYPE_STRUCT";
		case _TYPE_POINTER: return "_TYPE_POINTER";
		case _TYPE_CONST: return "_TYPE_CONST";
		case _NEW: return "_NEW";
		case _DELETE: return "_DELETE";
		case _LBRA: return "{";
		case _RBRA: return "}";
		case _LPAR: return "(";
		case _RPAR: return ")";
		case _SEQ: return "_SEQ";
		case _EMPTY: return "_EMPTY";
		case _MAIN: return "_MAIN";
		case _PROG: return "_PROG";
		default: return "Unknown kind of the node\n";
	}
}

struct node {
	int kind;
	int val;
	struct node *op1;
	struct node *op2;
	struct node *op3;

	struct node *spec1;
	struct node *spec2;
	struct node *spec3;
	struct node *spec4;
	struct node *spec5;
	struct node *spec6;

	struct node *term1;
	struct node *term2;
};

typedef struct node node;

void printTree(node *root, int k);
node *S();
node *newNode(int k);
node *programStart();
node *Instruction();
node *InstExp();
node *Comp_Instr();
node *If_Instr();
node *Do_Instr();
node *Def_Instr();
node *Expression();
node *Log_OR();
node *Log_AND();
node *Equivalence();
node *N_Equivalence();
node *Additional();
node *Multip();
node *Unar();
node *Postfix();
node *First_Exp();
node *Type();
node *New();
node *Delete();
node *Foll_Ins();
node *SeqSpecif_Types();
node *Specif_Types();
node *List_InitDef();
node *Specif_Struct();
node *Specif_Struct_List();
node *Elem_Struct_List();
node *List_InitDef();
node *InitDef();

node *newNode(int k) {
	node *x = (node*)malloc(sizeof(node));
	x->kind = k;

	x->op1 = NULL;
	x->op2 = NULL;
	x->op3 = NULL;

	x->spec1 = NULL;
	x->spec2 = NULL;
	x->spec3 = NULL;
	x->spec4 = NULL;
	x->spec5 = NULL;
	x->spec6 = NULL;

	x->term1 = NULL;
	x->term2 = NULL;
	return x;
}

node *programStart() {
	node *x = newNode(_PROG);
	nextLexeme();
	x->op1 = S();
	if (lex != EOI) syntErrCode(0);
	return x;
}

node *S() {
	node *x, *t, *m;
	/*
	S->void main()
	S->void main() Instruction
	*/
	if (lex == TYPE_VOID) {
		x = newNode(_VOID);
		nextLexeme();
		if (lex == MAIN) {
			t = newNode(_MAIN);
			nextLexeme();
			x->op1 = t;
			if (lex == LPAR) {
				nextLexeme();
				if (lex == RPAR) {
					nextLexeme();
					if (lex == EOI) {
						return x;
					}
					else {
						t->op1 = Instruction();
						printf("Here7\n");
						return x;
					}
				}
				else syntErrCode(RPAR_MISS);
			}
			else syntErrCode(LPAR_MISS);
		}
		else syntErrCode(13);
	}
	else syntErrCode(14);
}

node *InstExp() {
	node *x;
	x = Expression();
	if (lex == SEMI) nextLexeme();
	else syntErrCode(SEMI_MISS);
	return x;
}

node *Expression() {
	node *x, *t, *z;
	x = Log_OR();
	if (lex == IS) {
		t = x;
		x = newNode(_IS);
		nextLexeme();
		x->op1 = t;
		x->op2 = Expression();
	}
	else if (lex == PLUSEQ || lex == MINUSEQ || lex == MULTEQ || lex == DEREQ) {
		t = x;
		x = newNode(_IS);
		x->op1 = t;
		if (lex == PLUSEQ) z = newNode(_PLUS);
		else if (lex == MINUSEQ) z = newNode(_MINUS);
		else if (lex == MULTEQ) z = newNode(_MULTIPLY);
		else z = newNode(_DERIVE);
		nextLexeme();
		z->op1 = t;
		z->op2 = Expression();
		x->op2 = z;
	}
	// short ifelse statement here!
	return x;
}

node *Log_OR() {
	node *x, *t;
	x = Log_AND();
	if (lex == OR) {
		t = x;
		x = newNode(_OR);
		nextLexeme();
		x->op1 = Log_OR();
		x->op2 = t;
	}
	return x;
}

node *Log_AND() {
	node *x, *t;
	x = Equivalence();
	if (lex == AND) {
		t = x;
		x = newNode(_AND);
		nextLexeme();
		x->op1 = t;
		x->op2 = Equivalence();
	}
	return x;
}

node *Equivalence() {
	node *x, *t;
	x = N_Equivalence();
	if (lex == EQUAL) {
		t = x;
		x = newNode(_EQUAL);
		nextLexeme();
		x->op1 = Equivalence();
		x->op2 = t;
	}
	return x;
}

node *N_Equivalence() {
	node *x, *t;
	x = Additional();
	if (lex == LESS || lex == MORE) {
		t = x;
		x = newNode(lex == LESS ? _LESS : _MORE);
		nextLexeme();
		x->op1 = N_Equivalence();
		x->op2 = t;
	}
	return x;
}

node *Additional() {
	node *x, *t;
	x = Multip();
	if (lex == PLUS || lex == MINUS) {
		t = x;
		x = newNode(lex == PLUS ? _PLUS : _MINUS);
		nextLexeme();
		x->op1 = Additional();
		x->op2 = t;
	}
	return x;
}

node *Multip() {
	node *x, *t;
	x = Unar();
	if (lex == MULTIPLY || lex == DERIVE) {
		t = x;
		x = newNode(lex == MULTIPLY ? _MULTIPLY : _DERIVE);
		nextLexeme();
		x->op1 = Multip();
		x->op2 = t;
	}
	return x;
}

node *Unar() {
	node *x, *t, *z;
	if (lex == INCR || lex == DECR) {
		x = newNode(_IS);
		x->op1 = Unar();
		exit(0);
		t = newNode(lex == INCR ? _PLUS : _MINUS);
		nextLexeme();
		t->op1 = x->op1;
		// Where is t connects?
		t->op2 = newNode(_INT);
		
	}
	else if (lex == LPAR || lex == ID || lex == INT || lex == REAL) {
		x = Postfix();
	}
	else if (lex == NEW) {
		x = New();
	}
	else {
		x = Delete();
	}
	return x;
}

node *Postfix() {
	node *x;
	x = First_Exp();
	if (lex == INCR || lex == DECR) x = Postfix();
	return x;
	// Problem function, may be error!
}

node *First_Exp() {
	node *x;
	if (lex == LPAR) {
		nextLexeme();
		x = Expression();
		if (lex == RPAR) {
			nextLexeme();
		}
		else syntErrCode(12);
	}
	else if (lex == ID) {
		x = newNode(_ID);
		nextLexeme();
	}
	else if (lex == INT || lex == REAL) {
		x = newNode(lex == INT ? _INT : _REAL);
		nextLexeme();
	}
	else syntErrCode(12);
	return x;
	// Arrays missing!
}

node *Type() {
	node *x;
	if (lex == TYPE_INT || lex == TYPE_REAL) {
		x = newNode(lex == TYPE_INT ? _TYPE_INT : _TYPE_REAL);
		nextLexeme();
	}
	else syntErrCode(1);
	return x;
}

node *New() {
	node *x;
	if (lex == NEW) {
		x = newNode(_NEW);
		nextLexeme();
		x->op1 = Type();
		if (lex == LPAR) {
			nextLexeme();
			x->op2 = First_Exp();
			if (lex == RPAR) {
				nextLexeme();
			}
			else syntErrCode(RPAR_MISS);
		}
		else syntErrCode(LPAR_MISS);
	}
	else syntErrCode(1);
	return x;
}

node *Delete() {
	node *x;
	if (lex == DELETE) {
		x = newNode(_DELETE);
		nextLexeme();
		if (lex == ID) {
			x->op1 = newNode(_ID);
			nextLexeme();
		}
		else syntErrCode(1);
	}
	else syntErrCode(1);
	return x;
}

node *If_Instr() {
	node *x;
	bool ispar = 0;
	if (lex == IF) {
		x = newNode(_IF1);
		nextLexeme();
		if (lex == LPAR) {
			x->spec1 = newNode(_LPAR);
			nextLexeme();
			x->op1 = Expression();
			if (lex == RPAR) {
				nextLexeme();
				x->spec2 = newNode(_RPAR);
				if (lex == LBRA) { x->spec3 = newNode(_LBRA); ispar = 1; }
				x->op2 = Instruction();
				if (ispar) { x->spec4 = newNode(_RBRA); ispar = 0; }
				if (lex == ELSE) {
					x->kind = _IF2;
					nextLexeme();
					if (lex == LBRA) { x->spec5 = newNode(_LBRA); ispar = 1; }
					x->op3 = Instruction();	
					if (ispar) { x->spec6 = newNode(_RBRA); ispar = 0; }
				}
			}
			else syntErrCode(RPAR_MISS);
		}
		else syntErrCode(LPAR_MISS);
	}
	else syntErrCode(IF_MISS);
	return x;
}

node *Do_Instr() {
	node *x;
	if (lex == DO) {
		x = newNode(_DO);
		nextLexeme();
		if (lex == LBRA) {
			x->spec1 = newNode(_LBRA);
			x->op1 = Instruction();
			x->spec2 = newNode(_RBRA);
			if (lex == WHILE) {
				nextLexeme();
				if (lex == LPAR) {
					x->spec3 = newNode(_LPAR);
					nextLexeme();
					x->op2 = Expression();
					if (lex == RPAR) {
						nextLexeme();
						x->spec4 = newNode(_RPAR);
					}
					else syntErrCode(RPAR_MISS);
				}
				else syntErrCode(LPAR_MISS);
			}
			else syntErrCode(WHILE_MISS);
		}
		else syntErrCode(LBRA_MISS);
	}
	else syntErrCode(DO_MISS);
	return x;
}

node *Comp_Instr() {
	node *x;
	if (lex == MULTIPLY || lex == MINUS || lex == PLUS || lex == INCR || lex == DECR || lex == NEW || lex == DELETE || lex == ID ||
	lex == INT || lex == REAL ||
	lex == LBRA ||
	lex == IF ||
	lex == DO ||
	lex == TYPE_CONST || lex == TYPE_STRUCT || lex == TYPE_INT || lex == TYPE_REAL) {
		x = Instruction();
	}
	else {
		x = newNode(_EMPTY);
		nextLexeme();
	}
	return x;
}


node *Def_Instr() {
	node *x = newNode(_SEQ);
	x->op1 = SeqSpecif_Types();
	x->op2 = List_InitDef();
	nextLexeme();
	return x;	
}

node *SeqSpecif_Types() {
	node *x;
	x = Specif_Types();
	return x;
}

node *Specif_Types() {
	node *x;
	if (lex == TYPE_INT || lex == TYPE_REAL) {
		x = Type();
	}
	else if (lex == TYPE_STRUCT) {
		printf("Here2\n");
		x = Specif_Struct();
	}
	else if (lex == TYPE_CONST) {
		x = newNode(_TYPE_CONST);
	}
	else {
		printf("Error.\n");
		exit(1);
	}
	return x;
}

node *Specif_Struct() {
	node *x = newNode(_TYPE_STRUCT);
	nextLexeme();
	if (lex == LBRA) {
		x->spec1 = newNode(_LBRA);
		nextLexeme();
		if (lex == RBRA) {
			x->spec2 = newNode(_RBRA);
		}
		else {
			x->op1 = Specif_Struct_List();
			if (lex == RBRA) {
				x->spec2 = newNode(_RBRA);
			}
			else {
				printf("Error.\n");
				exit(1);
			}
		}
	}
	else if (lex == ID) {
		printf("Here3\n");
		nextLexeme();
		x->op1 = newNode(_ID);
		if (lex == LBRA) {
			x->spec3 = newNode(_LBRA);
			nextLexeme();
			if (lex == RBRA) {
				x->spec2 = newNode(_RBRA);
				printf("Here4\n");
			}
			else {
				x->op2 = Specif_Struct_List();
				printf("%s\n", charLexeme(lex)); // <-- charLexeme();
				if (lex == RBRA) {
					x->spec4 = newNode(_RBRA);
				}
				else {
					printf("Error RBRA.\n");
					exit(1);
				}
			}
		}
		else {
			printf("Error LBRA.\n");
			exit(1);
		}
	}
	else {
		printf("Error.\n");
		exit(1);
	}
	nextLexeme();
	return x;
}

node *Specif_Struct_List() {
	node *x, *t;
	x = Elem_Struct_List();
	printf("[%c]\n", ch);
	if (lex == COMMA) {
		t = x;
		x = newNode(_SEQ);
		nextLexeme();
		x->op1 = Specif_Struct_List();
		x->op2 = t;
	}
	return x;
}

node *Elem_Struct_List() {
	node *x;
	x = Def_Instr();
	return x;
}

/*
node *Elem_Struct_List() {
	node *x;
	if (lex == ID) {
		nextLexeme();
		if (lex == IS) {
			x = newNode(_IS);
			nextLexeme();
			x->op1 = newNode(_ID);
			x->op2 = Expression();
		}
		else {
			printf("Error1.\n");
			exit(1);
		}
	}
	else {
		printf("Error2.\n");
		exit(1);
	}
	nextLexeme();
	return x;
}
*/

/*
node *Log_OR() {
	node *x, *t;
	x = Log_AND();
	if (lex == OR) {
		t = x;
		x = newNode(_OR);
		nextLexeme();
		x->op1 = Log_OR();
		x->op2 = t;
	}
	return x;
}
*/

node *List_InitDef() {
	node *x, *t;
	x = InitDef();
	if (lex == ID) {
		t = x;
		x = newNode(_SEQ);
	}
	return NULL;
}

node *InitDef() {
	return NULL;
}

node *Instruction() {
	node *x, *t;
	/*
	Instruction->InstExp
	Log_OR Log_AND Equivalence N_Equivavelence Additional Multip Unar Postfix First_Exp
	* - + ++ -- new delete _ID int real
	*/
	
	if (lex == MULTIPLY || lex == MINUS || lex == PLUS || lex == INCR || lex == DECR || lex == NEW || lex == DELETE || lex == ID ||
	lex == INT || lex == REAL) {
		x = InstExp();
	}

	/*
	Instruction->Comp_Instr }
	{
	*/
	else if (lex == LBRA) {
		x = newNode(_EMPTY);
		nextLexeme();
		while(lex != RBRA) {
			if (lex == EOI) syntErrCode(RBRA_MISS);
			t = x;
			x = newNode(_SEQ);
			x->op1 = t;
			x->op2 = Comp_Instr();
		}
		nextLexeme();
	}

	/*
	Instruction->If_Instr
	if
	*/
	else if (lex == IF) {
		x = If_Instr();
	}

	/*
	Instruction->Do_Instr
	do
	*/
	else if (lex == DO) {
		x = Do_Instr();
	}

	/*
	Instruction->Def_Instr
	SeqSpecif_Types Specif_Types Type Specif_Struct
	const struct int real
	*/
	else {
		printf("Here\n");
		x = Def_Instr();
		printf("Here5\n");
	}
	
	printf("Here6\n");
	
	return x;
}

/* Help functions */

void printTree(node *r, int l)
{
	int i;

	if(!r) return;
	if (r->kind == _EMPTY) return;

	// op1

	printTree(r->spec1, l+1);
	printTree(r->op1, l+1);
	printTree(r->spec2, l+1);

	for(i=0; i<l; ++i) printf("   ");

	printf("%s\n", charNode(r->kind));

	// op2

	printTree(r->spec3, l+1);
	printTree(r->op2, l+1);
	printTree(r->spec4, l+1);


	// op3
	
	printTree(r->spec5, l+1);
	printTree(r->op3, l+1);
	printTree(r->spec6, l+1);

}

void printHelp() {
	printf("C++ compiler\n");
	printf("version %d.%d\n", MAJOR, MINOR);
	printf("default source file: source.cpp\n");
	printf("\n");
	printf("For lexical analysis type:\n");
	printf("	./compiler l\n");
	printf("\n");
	printf("For syntax analysis type:\n");
	printf("	./compiler s\n");
	printf("\n");
}

int main(int argc, char *argv[]) {

	fp = fopen("source.cpp", "r");

	if (argc == 1) {
		type = NO;
		printHelp();
	}

	else if (!strcmp(argv[1], "l")) {
		type = LEX;
		nextLexeme();
	}

	else if (!strcmp(argv[1], "s")) {
		type = SYNT;
		node *x = programStart();
		printf("Syntax tree: \n");
		printTree(x, 0);
	}

	else {
		type = UNKNOWN;
		printf("Unknown parameter.\n");
	}

	return 0;
}

























