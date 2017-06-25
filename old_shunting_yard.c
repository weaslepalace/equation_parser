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


operation_s *opStack[MAXOPSTACK];
int opStackDepth = 0;

double numStack[MAXNUMSTACK];
int numStackDepth = 0;

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

void shunt_op(operation_s *op)
{
	operation_s *pop;

	if(op->op == '(')
	{
		push_opStack(op);
		return;
	}
	else if(op->op == ')')
	{
		while((opStackDepth > 0) && (opStack[opStackDepth - 1]->op != '('))
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

double old_shunting_yard(char *expression)
{
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


char *tokenize_negate(token_queue_s *tokens, char *cursor)
{
	if(*cursor == '-')
	{
		if(enqueue_token(tokens, "-") < 0)
		{
			return NULL;
		}
	}
	return cursor;
}

char *tokenize_paren(token_queue_s *tokens, char *cursor)
{
	if(*cursor == '(')
	{
		if(enqueue_token(tokens, "(") < 0)
		{
			return NULL;
		}
	}
	else if(*cursor == ')')
	{
		if(enqueue_token(tokens, ")") < 0)
		{
			return NULL;
		}
	}
	return cursor;
}

char *tokenize_variable(token_queue_s *tokens, char *expr)
{
	char *cursor = expr;
	if(!isaplha(*cursor))
	{
		return NULL;
	}
	cursor++;

	//Count the token size
	int lToken = 1;
	for(char *cursor = expr; 
		isalnum(*cursor) || *cursor == '.' || *cursor == '[' || *cursor == ']';
		cursor++)
	{
		lToken++;
	}

	for(; *cursor != '\0'; cursor++)
	{
		if(isalpha(*cursor))
		{
			continue;
		}
		else if(*cursor == '[')
		{
			for(char *c = cursor; *c != ']'; c++)
			{
				if(isdigit(*c))
				{
				}
				else
				{
					return NULL;
				}
			} 
		}
		else if(*cu
	}

}

char *tokenize_constant(token_queue_s *tokens, char *expr)
{

}


token_queue_s *tokenize_equation(char *expr)
{
	for(char *cursor = expr; *cursor != '\0'; cursor++)
	{


	}
}



int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage:%s <expression>\n", argv[0]);
		return EXIT_FAILURE;
	}

	printf("%f\n", old_shunting_yard(argv[1]);

	return EXIT_SUCCESS;
}



