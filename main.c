//Stolen From: 
//https://web.archive.org/web/20120806183037/http://en.literateprograms.org/Special:DownloadCode/Shunting_yard_algorithm_(C)

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define MAXOPSTACK  64
#define MAXNUMSTACK 64

double eval_negate(double arg1, double arg2)
{
	return -arg1;
}

double eval_exponent(double arg1, double arg2)
{
	return pow(arg1, arg2);
}

double eval_multiply(double arg1, double arg2)
{
	return arg1 * arg2;
}

double eval_divide(double arg1, double arg2)
{
	if(arg2 == 0)
	{
		fprintf(stderr, "ERROR: Divide by zero\n");
		exit(EXIT_FAILURE);
	}
	return arg1 / arg2;
}

double eval_modulo(double arg1, double arg2)
{
	if(arg2 == 0)
	{
		fprintf(stderr, "ERROR: Divide by zero\n");
		exit(EXIT_FAILURE);
	}
	return (int)arg1 % (int)arg2;
}

double eval_add(double arg1, double arg2)
{
	return arg1 + arg2;
}

double eval_subtract(double arg1, double arg2)
{
	return arg1 - arg2;
}


typedef struct {
	char op;
	int prec;
	enum {
		ASSOC_NONE = 0, ASSOC_LEFT, ASSOC_RIGHT
	} assoc;
	int unary;
	double (*eval)(double, double);
} operation_s;
operation_s ops[] = {
	{.op = '_', .prec = 10, .assoc = ASSOC_RIGHT, .unary = 1, .eval = eval_negate},
	{.op = '^', .prec = 9,  .assoc = ASSOC_RIGHT, .unary = 0, .eval = eval_exponent},
	{.op = '*', .prec = 8,  .assoc = ASSOC_LEFT,  .unary = 0, .eval = eval_multiply},
	{.op = '/', .prec = 8,  .assoc = ASSOC_LEFT,  .unary = 0, .eval = eval_divide},
	{.op = '%', .prec = 8,  .assoc = ASSOC_LEFT,  .unary = 0, .eval = eval_modulo},
	{.op = '+', .prec = 5,  .assoc = ASSOC_LEFT,  .unary = 0, .eval = eval_add},
	{.op = '-', .prec = 5,  .assoc = ASSOC_LEFT,  .unary = 0, .eval = eval_subtract},
	{.op = '(', .prec = 0,  .assoc = ASSOC_NONE,  .unary = 0, .eval = NULL},
	{.op = ')', .prec = 0,  .assoc = ASSOC_NONE,  .unary = 0, .eval = NULL}
};


operation_s *get_op(char ch)
{
	for(int i = 0; i < (sizeof(ops) / sizeof(operation_s)); i++)
	{
		if(ch == ops[i].op)
		{
			return &ops[i];
		}
	}
	return NULL;
}

typedef struct {
	operation_s *opStack[MAXOPSTACK];
	int opStackDepth = 0;

	double numStack[MAXNUMSTACK];
	int numStackDepth = 0;
} equation_s;

void push_opStack(operation_s *op)
{
	if(opStackDepth > (MAXOPSTACK - 1))
	{
		fprintf(stderr, "ERROR: Operator stack overflow\n");
		exit(EXIT_FAILURE);
	}

	opStack[opStackDepth] = op;
	opStackDepth++;
}

operation_s *pop_opStack(void)
{
	if(opStackDepth == 0)
	{
		fprintf(stderr, "ERROR: Operator stack empty\n");
		exit(EXIT_FAILURE);
	}
	opStackDepth--;
	return opStack[opStackDepth];
}


void push_numStack(double num)
{
	if(numStackDepth > (MAXNUMSTACK - 1))
	{
		fprintf(stderr, "ERROR: Output stack overflow\n");
		exit(EXIT_FAILURE);
	}
	numStack[numStackDepth] = num;
	numStackDepth++;
}

double pop_numStack(void)
{
	if(numStackDepth == 0)
	{
		fprintf(stderr, "ERROR: Output stack empty\n");
		exit(EXIT_FAILURE);
	}
	numStackDepth--;
	return numStack[numStackDepth];
}

void shunt_op(equation_s *eq, operation_s *op)
{
	operation_s *pop;

	if(op->op == '(')
	{
		push_opStack(eq->opStack, op);
		return;
	}
	else if(op->op == ')')
	{
		while((opStackDepth > 0) && (opStack[opStackDepth - 1]->op != '('))
		{
			pop = pop_opStack(eq->opStack);
			double n1 = pop_numStack(eq->numStack);
			if(pop->unary == 1)
			{
				push_numStack(pop->eval(n1, 0));
			}
			else
			{
				double n2 = pop_numStack();
				push_numStack(pop->eval(n2, n1));
			}

		}
		
		if(!(pop = pop_opStack()) || (pop->op != '('))
		{
			fprintf(stderr, "ERROR: Stack error. No matching '('\n");
		}

		return;	
	}

	if(op->assoc == ASSOC_RIGHT)
	{
		while((opStackDepth > 0) && (op->prec < opStack[opStackDepth - 1]->prec))
		{
			pop = pop_opStack();
			double n1 = pop_numStack();
			if(pop->unary == 1)
			{	
				push_numStack(pop->eval(n1, 0));
			}
			else
			{
				double n2 = pop_numStack();
				push_numStack(pop->eval(n2, n1));
			}
		}
	}
	else
	{
		while((opStackDepth > 0) && (op->prec < opStack[opStackDepth - 1]->prec))
		{
			pop = pop_opStack();
			double n1 = pop_numStack();
			if(pop->unary == 1)
			{	
				push_numStack(pop->eval(n1, 0));
			}
			else
			{
				double n2 = pop_numStack();
				push_numStack(pop->eval(n2, n1));
			}
		}
	}
	push_opStack(op);
}

equation_s *old_shunting_yard(char *expression)
{
	equation_s *eq = malloc(sizeof(equation_s));
	if(eq == NULL)
	{
		return NULL;
	}
	
	operation_s *op = NULL;
	operation_s startOp = {'X', 0, ASSOC_NONE, 0, NULL};
	operation_s *lastOp = &startOp;
	char *tstart = NULL;
	for(char *expr = expression; *expr != '\0'; expr++)
	{
		if(tstart == NULL)
		{
			op = get_op(*expr);
			if(op != NULL)
			{
				if((lastOp != NULL) && ((lastOp == &startOp) || (lastOp->op != ')')))
				{
					if(op->op == '-')
					{
						op = get_op('_');
					}
					else if(op->op != '(')
					{
						fprintf(stderr, "ERROR: "
							"Illegal use of binary operator (%c)\n", op->op);
						exit(EXIT_FAILURE);
					}
				}
				shunt_op(op);
				lastOp = op;
			}	
			else if(isdigit(*expr))
			{
				tstart = expr;
			}
			else if(!isspace(*expr))
			{
				fprintf(stderr, "ERROR: Syntax error\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			op = get_op(*expr);
			if(isspace(*expr))
			{
				push_numStack(strtod(tstart, NULL));
				tstart = NULL;
				lastOp = NULL;
			}
			else if(op != NULL)
			{
				push_numStack(strtod(tstart, NULL));
				tstart = NULL;
				shunt_op(op);
				lastOp = op;
			}
			else if(!isdigit(*expr))
			{
				fprintf(stderr, "ERROR: Syntax error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	if(tstart != NULL)
	{
		push_numStack(strtod(tstart, NULL));
	}

	while(opStackDepth > 0)
	{
		op = pop_opStack();
		double n1 = pop_numStack();
		if(op->unary == 1)
		{
			push_numStack(op->eval(n1, 0));
		}
		else
		{
			double n2 = pop_numStack();
			push_numStack(op->eval(n2, n1));
		}
	}
	if(numStackDepth != 1)
	{
		fprintf(stderr, "ERROR: "
			"Output stack has %d unhandled elements", numStackDepth);
		exit(EXIT_FAILURE);
	}

	return numStack[0];
}


//char *tokenize_negate(token_queue_s *tokens, char *cursor)
//{
//	if(*cursor == '-')
//	{
//		if(enqueue_token(tokens, "-") < 0)
//		{
//			return NULL;
//		}
//	}
//	return cursor;
//}

//char *tokenize_paren(token_queue_s *tokens, char *cursor)
//{
//	if(*cursor == '(')
//	{
//		if(enqueue_token(tokens, "(") < 0)
//		{
//			return NULL;
//		}
//	}
//	else if(*cursor == ')')
//	{
//		if(enqueue_token(tokens, ")") < 0)
//		{
//			return NULL;
//		}
//	}
//	return cursor;
//}

//char *tokenize_variable(token_queue_s *tokens, char *expr)
//{
//	char *cursor = expr;
//	if(!isaplha(*cursor))
//	{
//		return NULL;
//	}
//	cursor++;
//
//	//Count the token size
//	int lToken = 1;
//	for(char *cursor = expr; 
//		isalnum(*cursor) || *cursor == '.' || *cursor == '[' || *cursor == ']';
//		cursor++)
//	{
//		lToken++;
//	}
//
//	for(; *cursor != '\0'; cursor++)
//	{
//		if(isalpha(*cursor))
//		{
//			continue;
//		}
//		else if(*cursor == '[')
//		{
//			for(char *c = cursor; *c != ']'; c++)
//			{
//				if(isdigit(*c))
//				{
//				}
//				else
//				{
//					return NULL;
//				}
//			} 
//		}
//		else if(*cu
//	}
//
//}

//char *tokenize_constant(token_queue_s *tokens, char *expr)
//{
//
//}

/**
	token Object
*/
typedef struct {
	bool variable;
	bool function;
	bool constant;
	bool operation;
	char *text;
	int length;
}token_s;
token_s *token_prototype = {
	.variable = false, .function = false, .operation = false,
	.text = "", .length = 0
};

/**
	Parse a function or variable name
*/
token_s *parse_name(char **cursor)
{
	//Allocate an empty token object
	token_s *token = malloc(sizeof(token));
	if(token == NULL)
	{
		return NULL;
	}
	memcpy(token, &token_prototype, sizeof(token_s));

	//Verify that all characters in the token are valid
	for(char *c = *cursor; (*c != '\0') || !isspace(*c); c++)
	{
		//The existence of parenthesis signals that the token is a function name
		//	and that the token has ended.
		if(*c == '(')
		{
			//This is the end of the function name token.
			//It is not valid if the token is a variable name
			if(token->variable == true)
			{
				return NULL;
			}
			
			//Do not include '(' as part of the function name
			*cursor = c - 1;
			token->text = cursor;
			token->function = true;
			return token;
		}

		//The following characters are valid: A-Za-z.[]0-9 
		else if((*c >= 'a') && (*c <= 'z') ||
			((*c >= 'A') && (*c <= 'Z')) ||
			((*c >= '0') && (* <= '9')))
		{
			//The character is valid
		}

		//These character: .[] are not valid if the token is a function name
		//	but are valid for if the token is a variable name
		else if((*c == '.') || (*c == '[') || (*c == ']'))
		{
			//This is a definite indicator that the token is a variable name
			token->variable = true;
		}
		
		//The token is not valid
		else
		{
			return NULL;
		}

		//Count the number of valid characters
		token->length++;
	}

	//Token is a valid variable name
	//Do not include the termator or space in the token
	*cursor = c - 1;
	token->text = *cursor;
	token->variable = true;
	return token;
}

/**
	Parse a numeric constant
*/
token_s *parse_number(char **cursor)
{
	//If the first character is zero, the second character must be '.'
	//	Otherwise the token is invalid
	if((**cursor == '0') && (*(*cursor + 1) != '.')) 
	{
		return NULL;
	}
		
	//Allocate an empty token object
	token_s *token = malloc(sizeof(token_s));
	if(token == NULL)
	{
		return NULL;
	}
	memcpy(token, &token_prototype, sizeof(token_s));

	bool exponent = false;
	bool point = false;
	bool neg = false;
	for(char *c = *cursor; *c != '\0' || !isspace(*c); c++)
	{
		//Decimal digits, of course, are valid
		if((*c >= '0') && (*c <= '9'))
		{
			//The character is valid
		}
		
		//'e' of 'E' indicated that the constant has an exponent.
		if((*c == 'e') || (*c == 'E'))
		{
			//Only one exponent is valid
			if(exponent == true)
			{
				return NULL;
			}
			//This constant has a valid exponent
			exponent = true;
		}
		
		//Negative exponents are valid,
		//	but only as the first character following the 'e'
		if(*c == '-')
		{
			//'-' is only valid for exponents
			if(exponent == false)
			{
				return NULL;
			}
			
			//Only one '-' is allowed in the token
			if(neg == true)
			{
				return NULL;
			}
		
			neg = true;
		}
		
		//'.' is a valid character
		if(*c == '.')
		{
			//Only one '.' is allowed in the token
			if(point == true)
			{
				return NULL;
			}
			
			//'.' is not allowed after the exponent
			if(exponent == true)
			{
				return NULL;
			}

			point = true;
		}

		//Count the number of valid characters
		token->length++;
	}

	//The token is a valid numeric constant
	//Do not include the termator or space in the token
	*cursor = c - 1;
	token->text = cursor;
	token->constant = true;
	return token;
}

/**
	Parse an operator
*/
token_s *parse_operator(&cursor)
{
	//Allocate an empty token object
	token_s token = malloc(sizeof(token));
	if(token == NULL)
	{
		return NULL;
	}
	memcpy(token, token_prototype, sizeof(token_s));
	
	//Let's just assume the token is valid,
	//	provided that the operator is followed by the necessary space
	//	(I hate it when people don't put a space before and after operators)
	*cursor++;
	if(!isspace(*cursor))
	{
		return NULL;
	}

	token->length = 1;
	token->text = *cursor - 1;
	token->operator = true;
	return token;
}


token_s *parse_token(char *cursor)
{
	//Check if the token is a variable name or a function name
	if(((*cursor >= 'a') && (*cursor <= 'z')) || 
		((*cursor >= 'A') && (*cursor <= 'Z')) ||
		(*cursor == '_'))
	{
		//The token is a variable name or a function name
		return parse_name(&cursor);
	}
	
	//Check if the token is a numeric constant
	else if((*cursor >= '0') && (*cursor <= '9'))
	{
		//The token is a numeric constant
		return parse_number(&cursor);
	}

	//Check if the token is an operator
	else if((*cursor == '+') || (*cursor == '-') ||
		(*cursor == '*') || (*cursor == '/') ||
		(*cursor == '^') || (*cursor == '%') || 
		(*cursor == '(') || (*cursor == ')') || (*cursor == ','))
	{
		//The token is an operator
		return parse_operator(&cursor);
	}

	else	
	{
		return NULL;
	}
}


token_queue_s *tokenize_equation(char *expr)
{
	char *cursor = expr;
	do 
	{
		if(!isspace(*cursor))
		{
			cursor = parse_token(cursor);
			if(cursor == NULL)
			{
				fprintf(stderr, "Unable to parse a token");
			}
		}
	}	
	while(*cursor != '\0');

}



int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage:%s <expression>\n", argv[0]);
		return EXIT_FAILURE;
	}
                                                              
	printf("%f\n", old_shunting_yard(argv[1]));

	return EXIT_SUCCESS;
}



