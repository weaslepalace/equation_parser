//Stolen From: 
//https://web.archive.org/web/20120806183037/http://en.literateprograms.org/Special:DownloadCode/Shunting_yard_algorithm_(C)

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

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

/**
	Return the operator precedence
*/
int operator_prec(char *text)
{
	//Return precedence on single-character operators
	switch(text[0])
	{
		case '^':
		return 9;

		case '*':
		return 8;

		case '/':
		return 8;

		case '%':
		return 8;

		case '+':
		return 5;

		case '-':
		return 5;
	}

	return 0;
}

/**
	Return true if the character is a valid operator
*/
bool operator_check(char c)
{
	if((c == '+') || (c == '-') ||
		(c == '*') || (c == '/') ||
		(c == '^') || (c == '%') || 
		(c == '(') || (c == ')') || (c == ','))
	{
		//The token is an operator
		return true;
	}
	return false;
}

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

//typedef struct {
//	operation_s *opStack[MAXOPSTACK];
//	int opStackDepth;
//
//	double numStack[MAXNUMSTACK];
//	int numStackDepth;
//} equation_s;
//
//void push_opStack(operation_s *op)
//{
//	if(opStackDepth > (MAXOPSTACK - 1))
//	{
//		fprintf(stderr, "ERROR: Operator stack overflow\n");
//		exit(EXIT_FAILURE);
//	}
//
//	opStack[opStackDepth] = op;
//	opStackDepth++;
//}
//
//operation_s *pop_opStack(void)
//{
//	if(opStackDepth == 0)
//	{
//		fprintf(stderr, "ERROR: Operator stack empty\n");
//		exit(EXIT_FAILURE);
//	}
//	opStackDepth--;
//	return opStack[opStackDepth];
//}
//
//
//void push_numStack(double num)
//{
//	if(numStackDepth > (MAXNUMSTACK - 1))
//	{
//		fprintf(stderr, "ERROR: Output stack overflow\n");
//		exit(EXIT_FAILURE);
//	}
//	numStack[numStackDepth] = num;
//	numStackDepth++;
//}
//
//double pop_numStack(void)
//{
//	if(numStackDepth == 0)
//	{
//		fprintf(stderr, "ERROR: Output stack empty\n");
//		exit(EXIT_FAILURE);
//	}
//	numStackDepth--;
//	return numStack[numStackDepth];
//}
//
//void shunt_op(equation_s *eq, operation_s *op)
//{
//	operation_s *pop;
//
//	if(op->op == '(')
//	{
//		push_opStack(eq->opStack, op);
//		return;
//	}
//	else if(op->op == ')')
//	{
//		while((opStackDepth > 0) && (opStack[opStackDepth - 1]->op != '('))
//		{
//			pop = pop_opStack(eq->opStack);
//			double n1 = pop_numStack(eq->numStack);
//			if(pop->unary == 1)
//			{
//				push_numStack(pop->eval(n1, 0));
//			}
//			else
//			{
//				double n2 = pop_numStack();
//				push_numStack(pop->eval(n2, n1));
//			}
//
//		}
//		
//		if(!(pop = pop_opStack()) || (pop->op != '('))
//		{
//			fprintf(stderr, "ERROR: Stack error. No matching '('\n");
//		}
//
//		return;	
//	}
//
//	if(op->assoc == ASSOC_RIGHT)
//	{
//		while((opStackDepth > 0) && (op->prec < opStack[opStackDepth - 1]->prec))
//		{
//			pop = pop_opStack();
//			double n1 = pop_numStack();
//			if(pop->unary == 1)
//			{	
//				push_numStack(pop->eval(n1, 0));
//			}
//			else
//			{
//				double n2 = pop_numStack();
//				push_numStack(pop->eval(n2, n1));
//			}
//		}
//	}
//	else
//	{
//		while((opStackDepth > 0) && (op->prec < opStack[opStackDepth - 1]->prec))
//		{
//			pop = pop_opStack();
//			double n1 = pop_numStack();
//			if(pop->unary == 1)
//			{	
//				push_numStack(pop->eval(n1, 0));
//			}
//			else
//			{
//				double n2 = pop_numStack();
//				push_numStack(pop->eval(n2, n1));
//			}
//		}
//	}
//	push_opStack(op);
//}
//

//equation_s *old_shunting_yard(char *expression)
//{
//	equation_s *eq = malloc(sizeof(equation_s));
//	if(eq == NULL)
//	{
//		return NULL;
//	}
//	
//	operation_s *op = NULL;
//	operation_s startOp = {'X', 0, ASSOC_NONE, 0, NULL};
//	operation_s *lastOp = &startOp;
//	char *tstart = NULL;
//	for(char *expr = expression; *expr != '\0'; expr++)
//	{
//		//Currently between tokens
//		if(tstart == NULL)
//		{
//			op = get_op(*expr);
//
//			//See an operator that does not follow a numeric token
//			if(op != NULL)
//			{
//				
//				if((lastOp != NULL) && ((lastOp == &startOp) || (lastOp->op != ')')))
//				{
//					if(op->op == '-')
//					{
//						op = get_op('_');
//					}
//					else if(op->op != '(')
//					{
//						fprintf(stderr, "ERROR: "
//							"Illegal use of binary operator (%c)\n", op->op);
//						exit(EXIT_FAILURE);
//					}
//				}
//				shunt_op(op);
//				lastOp = op;
//			}	
//
//			//See a digit while not tokenizing a number is the start of a token
//			else if(isdigit(*expr))
//			{
//				//Begin tokenizing a number
//				tstart = expr;
//			}
//			
//			//Invalid character
//			else if(!isspace(*expr))
//			{
//				fprintf(stderr, "ERROR: Syntax error\n");
//				exit(EXIT_FAILURE);
//			}
//		}
//
//		//Currently tokenizing a number
//		else
//		{
//			op = get_op(*expr);
//
//			//A space signals the end of a numeric token
//			if(isspace(*expr))
//			{
//				//Push the number token to the stack and reset the state variables
//				push_numStack(strtod(tstart, NULL));
//				tstart = NULL;
//				lastOp = NULL;
//			}
//
//			//An valid operator signals the end of a numeric token
//			else if(op != NULL)
//			{
//				//Push the number token to the stack and reset state variables
//				push_numStack(strtod(tstart, NULL));
//				tstart = NULL;
//				//Do magic to the operator				
//				shunt_op(op);
//				lastOp = op;
//			}
//
//			//Invalid character
//			else if(!isdigit(*expr))
//			{
//				fprintf(stderr, "ERROR: Syntax error\n");
//				exit(EXIT_FAILURE);
//			}
//		}
//	}
//	if(tstart != NULL)
//	{
//		push_numStack(strtod(tstart, NULL));
//	}
//
//	while(opStackDepth > 0)
//	{
//		op = pop_opStack();
//		double n1 = pop_numStack();
//		if(op->unary == 1)
//		{
//			push_numStack(op->eval(n1, 0));
//		}
//		else
//		{
//			double n2 = pop_numStack();
//			push_numStack(op->eval(n2, n1));
//		}
//	}
//	if(numStackDepth != 1)
//	{
//		fprintf(stderr, "ERROR: "
//			"Output stack has %d unhandled elements", numStackDepth);
//		exit(EXIT_FAILURE);
//	}
//
//	return numStack[0];
//}


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


/**
	token Object
*/
typedef struct {
//	bool variable;
//	bool function;
//	bool constant;
//	bool operator;
	enum {
		VARIABLE_TOKEN,
		FUNCTION_TOKEN,
		CONSTANT_TOKEN,
		OPERATOR_TOKEN,
		UNDEFINED_TOKEN
	} type;
	char *text;
	int length;
}token_s;
token_s token_prototype = {
//	.variable = false, .function = false, .constant = false, .operator = false, 
	.type = UNDEFINED_TOKEN,
	.text = "", .length = 0
};

typedef enum {
	INVALID_TOKEN,
	VALID_TOKEN,
	NEGATIVE_TOKEN
} token_validator_e;

token_s *new_token(void);
void free_token(token_s *token);
char *new_token_text(char *expr, int length);
token_s **resize_token_array(token_s **array, int size);
void free_token_array(token_s **array, int size);


/**
	token_queue_s Object
*/
typedef struct {
	token_s **token;
	int head;
	int tail;
} token_queue_s;
token_queue_s *new_token_queue(void);
int enqueue_token(token_queue_s *queue, token_s *token);
token_s *dequeue_token(token_queue_s *queue);
void free_token_queue(token_queue_s *queue);


/**	
	token_stack_s Object
*/
typedef struct {
	token_s **token;
	token_s *top;
	int depth;	
} token_stack_s;
token_stack_s *new_token_stack(void);
int push_token(token_stack_s *stack, token_s* token);
token_s *pop_token(token_stack_s *stack);
void free_token_stack(token_stack_s *stack);

token_s *parse_name(char **cursor);
token_s *parse_number(char **cursor);
token_s *parse_operator(char **cursor);
token_s *parse_token(char **cursor, token_validator_e isNeg);
token_queue_s *tokenize_equation(char *expr);

token_queue_s *new_shunting_yard(token_queue_s *tokenQueue);

/**
	token_s Object Constructor
*/
token_s *new_token(void)
{
	//Allocate an empty token object
	token_s *token = malloc(sizeof(token));
	if(token == NULL)
	{
		return NULL;
	}

	//Initialize the token object
	memcpy(token, &token_prototype, sizeof(token_s));

	return token;
}

/**
	token_s Object Destructor
*/
void free_token(token_s *token)
{
	if(token->text != NULL)
	{
		free(token->text);
		token->text = NULL;
	}
	free(token);
}

/**
	Allocate and copy a string of text
*/
char *new_token_text(char *expr, int length)
{
	//Create an empty string
	char *text = malloc(length + 1);
	if(text == NULL)
	{
		return NULL;
	}

	//Copy text from expr to the string
	strncpy(text, expr, length);
	text[length] = '\0';
	
	return text;
}


/**
	Reallocate of allocate an attay of tokens
*/
#define TOKEN_ARRAY_BOUNDARY	16
token_s **resize_token_array(token_s **array, int size)
{
	//Evaluate if the array has grown beyond it's boundary
	if((size % TOKEN_ARRAY_BOUNDARY) == 0)
	{
		//Expand the array to fit more tokens
		token_s **tmp = realloc(array,
			(size + TOKEN_ARRAY_BOUNDARY) * sizeof(token_s*));
		if(tmp == NULL)
		{
			free_token_array(array, size);
			return NULL;
		}
		array = tmp;
	}
	
	return array;
}

/**
	Free the contents of a token array
*/
void free_token_array(token_s **array, int size)
{
	for(int i = 0; i < size; i++)
	{
		if(array[i] != NULL)
		{
			free_token(array[i]);
			array[i] = NULL;
		}
	}
	free(array);
}

/**
	Enqueue a token_s object in the token queue
*/
int enqueue_token(token_queue_s *queue, token_s *token)
{
	//Allow the queue to grow as needed
	queue->token = resize_token_array(queue->token, queue->tail);
	if(queue->token == NULL)
	{
		return -1;
	}
	
	//Add the token to the queue
	queue->token[queue->tail] = token;
	queue->tail++;

	return 0;	
}

/**
	Dequeue a token_s object from the token queue
*/
token_s *dequeue_token(token_queue_s *queue)
{
	//Check if the queue is empty
	if(queue->head >= queue->tail)
	{
		//Reset the queue's parameters if it is empty
		queue->head = 0;
		queue->tail = 0;
		
		return NULL;
	}

	//Remove a token from the queue and return it
	token_s *tok = queue->token[queue->head];
	queue->head++;
	return tok;
}

/**
	token_queue_s Object Constructor
*/
token_queue_s *new_token_queue(void)
{
	//Allocate an empty token_queue_s object
	token_queue_s *queue = malloc(sizeof(token_queue_s));
	if(queue == NULL)
	{
		return NULL;
	}
	
	queue->token = NULL;
	queue->tail = 0;
	queue->head = 0;
	
	return queue;
}

/**
	token_queue_s Object Destructor
*/
void free_token_queue(token_queue_s *queue)
{
	if(queue->token != NULL)
	{
		free_token_array(queue->token, queue->tail);
		queue->token = NULL;
	}
	free(queue);
}


/**
	Push a token object to the token stack
*/
int push_token(token_stack_s *stack, token_s* token)
{
	//Allow the stack to grow as needed
	stack->token = resize_token_array(stack->token, stack->depth);
	if(stack->token == NULL)
	{
		return -1;
	}

	//Add the token to the top of the stack
	stack->top = token; 
	stack->token[stack->depth] = stack->top;
	stack->depth++;

	return 0;
}

/**
	Pop a token object from the token stack
*/
token_s *pop_token(token_stack_s *stack)
{
	//Check if the stack is empty
	if(stack->depth <= 0)
	{
		stack->depth = 0;
		stack->top = NULL;
		return NULL;
	}

	//Remove and return the token at the top of the stack
	token_s *tok = stack->top;
	stack->depth--;
	stack->top = stack->token[stack->depth - 1];
	return tok;
}

/**
	token_stack_s Object Constructor
*/
token_stack_s *new_token_stack(void)
{
	//Allocate an empty token_stack_s object
	token_stack_s *stack = malloc(sizeof(token_stack_s));
	if(stack == NULL)
	{
		return NULL;
	}

	stack->token = NULL;
	stack->top = NULL;
	stack->depth = 0;

	return stack;
}

/**
	token_stack_s Object Destructor
*/
void free_token_stack(token_stack_s *stack)
{
	if(stack->token != NULL)
	{
		free_token_array(stack->token, stack->depth);
		stack->token = NULL;
	}
	free(stack);	
}

/**
	Shunting-yard implementation on tokens object
*/
token_queue_s *new_shunting_yard(token_queue_s *tokenQueue)
{
	//Create an output queue
	token_queue_s *outputQueue = new_token_queue();
	if(outputQueue == NULL)
	{
		return NULL;
	}

	//Create the operator stack	
	token_stack_s *operatorStack = new_token_stack();
	if(operatorStack == NULL)
	{
		return NULL;
	}

	//Execute the shunting-yard algorithm
	for(token_s *token = dequeue_token(tokenQueue);
		token != NULL;
		token = dequeue_token(tokenQueue))
	{

		//Push numeric constants, variable names, and function names
		//	 directly to the output queue
		
//		if((token->variable == true) ||
//			(token->constant == true) ||
//			(token->function == true))
		switch(token->type)
		{
			case VARIABLE_TOKEN:
			case CONSTANT_TOKEN:
			case FUNCTION_TOKEN:
			if(enqueue_token(outputQueue, token) < 0)
			{
				return NULL;
			}
			break;
//		}

		//Push operator to the operator stack
//		else if((token->operator == true))
//		{
			case OPERATOR_TOKEN:
			if(token->text[0] == ')')
			{
				//Find the matching '(' in the operator stack
				while(operatorStack->top->text[0] != '(')
				{
					//Pop all the operators between the parens into the output queue
					token_s *topOp = pop_token(operatorStack);
					if(topOp == NULL)
					{
						//Mismatched parens will cause
						//	the operator stack to run out before finding the '('
						return NULL;
					}
					if(enqueue_token(outputQueue, topOp) < 0)
					{
						return NULL;
					}
				}
				//Once found, pop the '(' into the void
				pop_token(operatorStack);
				break;
			}

			//Look into the operator stack
			//	Pop all oprators from the top that have a >= precedence
			while((operatorStack->depth > 0) && 
				(operator_prec(token->text) <=
				operator_prec(operatorStack->top->text)) &&
				(token->text[0] != '('))

			{
				//Pop the top operator into the output stack
				token_s* topOp = pop_token(operatorStack);
				if(topOp == NULL)
				{
					return NULL;
				}
				if(enqueue_token(outputQueue, topOp) < 0)
				{
					return NULL;
				}
			}

			//Push it real good
			push_token(operatorStack, token);
			break;			

			default:
			return NULL;
		}
	}

	//Push the remaining tokens in the operator stack to the output queue
	for(token_s *topOp = pop_token(operatorStack);
		topOp != NULL;
		topOp = pop_token(operatorStack))
	{
		if(enqueue_token(outputQueue, topOp) < 0)
		{
			return NULL;
		}
	}

	return outputQueue;
}


/**
	Parse a function or variable name
*/
token_s *parse_name(char **cursor)
{
	//Create a token object
	token_s *token = new_token();
	if(token == NULL)
	{
		return NULL;
	}

	//Verify that all characters in the token are valid
	char *c = *cursor;
	for( ; (*c != '\0') && !isspace(*c); c++)
	{
		//The existence of parenthesis signals that the token is a function name
		//	and that the token has ended.
		if(*c == '(')
		{
			//This is the end of the function name token.
			//It is not valid if the token is a variable name
//			if(token->variable == true)
			if(token->type == VARIABLE_TOKEN)
			{
				return NULL;
			}
			
			//Do not include '(' as part of the function name
			token->text = new_token_text(*cursor, token->length);
			*cursor = c;
//			token->function = true;
			token->type = FUNCTION_TOKEN;
			return token;
		}

		//The following characters are valid: A-Za-z.[]0-9 
		else if(((*c >= 'a') && (*c <= 'z')) ||
			((*c >= 'A') && (*c <= 'Z')) ||
			((*c >= '0') && (*c <= '9')))
		{
			//The character is valid
		}

		//These character: .[] are not valid if the token is a function name
		//	but are valid for if the token is a variable name
		else if((*c == '.') || (*c == '[') || (*c == ']'))
		{
			//This is a definite indicator that the token is a variable name
//			token->variable = true;
			token->type = VARIABLE_TOKEN;
		}

		//The presence of an operator suggests that this is the end of
		//	the token and the operator is the next token
		else if(operator_check(*c) == true)
		{
			//This is the end of the token
			goto END_OF_NAME;
		}
		
		//The token is not valid
		else
		{
			return NULL;
		}

		//Count the number of valid characters
		token->length++;
	}

	END_OF_NAME:
	//Token is a valid variable name
	//Do not include the termator or space in the token
	token->text = new_token_text(*cursor, token->length);
	*cursor = c;
//	token->variable = true;
	token->type = VARIABLE_TOKEN;
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

	//If the first character is '-', the second character must be 0-9
	if((**cursor == '-') && ((*(*cursor + 1) < '0') || (*(*cursor + 1) > '9')))
	{
		return NULL;
	}
 
	//Create a token object
	token_s *token = new_token();
	if(token == NULL)
	{
		return NULL;
	}

	bool exponent = false;
	bool point = false;
	char *c = *cursor;
	for( ; (*c != '\0') && !isspace(*c); c++)
	{
		//Decimal digits, of course, are valid
		if((*c >= '0') && (*c <= '9'))
		{
			//The character is valid
		}
		
		//'e' of 'E' indicated that the constant has an exponent.
		else if((*c == 'e') || (*c == 'E'))
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
		else if(*c == '-')
		{
			//'-' is only valid at the beginning of the string
			if(c == *cursor)
			{
				//Valid	
			}
			//or immediatly following the 'E' of an exponent
			else if((*(c - 1) == 'E') || (*(c - 1) == 'e'))
			{
				//Valid
			}
			else
			{
				return NULL;
			}
		}
		
		//'.' is a valid character
		else if(*c == '.')
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

		//The presence of an operator suggests that this is the end of
		//	the token and the operator is the next token
		else if(operator_check(*c) == true)
		{
			//This is the end of the token
			goto END_OF_NUMBER;
		}
		
		else
		{
			//Invalid character
			return NULL;
		}

		//Count the number of valid characters
		token->length++;
	}

	END_OF_NUMBER:
	//The token is a valid numeric constant
	//Do not include the termator or space in the token
	token->text = new_token_text(*cursor, token->length);
	*cursor = c;
//	token->constant = true;
	token->type = CONSTANT_TOKEN;
	return token;
}

/**
	Parse an operator
*/
token_s *parse_operator(char **cursor)
{
	//Create a token object
	token_s *token = new_token();
	if(token == NULL)
	{
		return NULL;
	}
	
	//Let's just assume the token is valid,
	//	provided that the operator is followed by the necessary space
	//	(I hate it when people don't put a space before and after operators)
//	(*cursor)++;
//	if(!isspace(**cursor))
//	{
//		return NULL;
//	}

	token->length = 1;
	token->text = new_token_text(*cursor, token->length);
	(*cursor)++;
//	token->operator = true;
	token->type = OPERATOR_TOKEN;
	return token;
}


token_s *parse_token(char **cursor, token_validator_e isNeg)
{
	char c = **cursor;
	//Check if the token is a variable name or a function name
	if(((c >= 'a') && (c <= 'z')) || 
		((c >= 'A') && (c <= 'Z')) ||
		(c == '_'))
	{
		//The token is a variable name or a function name
		return parse_name(cursor);
	}

	//Check if the token is a numeric constant
	if(((c == '-') && (isNeg == NEGATIVE_TOKEN)) ||
		((c >= '0') && (c <= '9')))
	{
		//The token is a numeric constant
		return parse_number(cursor);
	}

	//Check if the token is an operator
	if(operator_check(c) == true)
	{
		//The token is an operator
		return parse_operator(cursor);
	}

	return NULL;
}


/**
	Append the text value of a numeric constant to a '-'
	Frees the original string
*/
char *negate_constant_text(char *text)
{
	if(text == NULL)
	{
		return NULL;
	}

	char *tempConst;
	asprintf(&tempConst, "-%s", text);
	free(text);

	return tempConst;
}


/**
	Weak validator: Returns false if a token arrives out of sequence
	also differentaites '-' between difference operator
*/
token_validator_e validate_token(token_s *token, token_s *lastToken)
{
	/*
	var  con  fun  op   nvar ncon nfun nop
     0    0    0    1    1    1    1    if(nop == '(' || op == ')')
     0    0    1    0    1    1    1    0  
     0    1    0    0    0    0    0    1
     1    0    0    0    0    0    0    1
	*/
	if(lastToken == NULL)
	{
		switch(token->type)
		{
			case VARIABLE_TOKEN:
			case CONSTANT_TOKEN:
			case FUNCTION_TOKEN:
			return VALID_TOKEN;

			case OPERATOR_TOKEN:
			if(token->text[0] == '(')
			{
				return VALID_TOKEN;
			}

			default:
			return INVALID_TOKEN;
		}
	}

	switch(lastToken->type)
	{
		case VARIABLE_TOKEN:
		case CONSTANT_TOKEN:
		case FUNCTION_TOKEN:
		if(token->type == OPERATOR_TOKEN)
		{
			return VALID_TOKEN;
		}
		return INVALID_TOKEN;

		case OPERATOR_TOKEN:
		switch(lastToken->text[0])
		{
			case ')':
			if((operator_prec(token->text) > 0) || (token->text[0] == ')'))
			{
				return VALID_TOKEN;
			}
			return INVALID_TOKEN;
	
			default:
			switch(token->type)
			{
				case VARIABLE_TOKEN:
				case CONSTANT_TOKEN:
				case FUNCTION_TOKEN:
				return VALID_TOKEN;
		
				case OPERATOR_TOKEN:
				if(token->text[0] == '(')
				{
					return VALID_TOKEN;
				}
				if(token->text[0] == '-')
				{
					return NEGATIVE_TOKEN;
				}
						
				default:
				return INVALID_TOKEN;
			}
		}

		default:
		return INVALID_TOKEN; 
	}

	return INVALID_TOKEN;	
}

/**
	Parse an equation into a sequeuce of tokens stored in a queue
*/
token_queue_s *tokenize_equation(char *expr)
{
	//Create a token queue to store parsed tokens
	token_queue_s *tokenQueue = new_token_queue();
	if(tokenQueue == NULL)
	{
		return NULL;
	}
	
	//Parser loop
	token_s *lastToken = NULL;
	token_validator_e valid = NEGATIVE_TOKEN;
	char *cursor = expr;
	do 
	{
		//Loop until the character under cursor is not white-space
		if(isspace(*cursor))
		{
			cursor++;
			continue;
		}

		//Parse the token under cursor
		token_s *token = parse_token(&cursor, valid);
		if(token == NULL)
		{
			fprintf(stderr, "Unable to parse a token\n");
			return NULL;
		}
			
		valid = validate_token(token, lastToken);
		switch(valid)
		{
			case VALID_TOKEN:
			break;
			
			case NEGATIVE_TOKEN:
			goto NEXT_TOKEN;
		
			case INVALID_TOKEN:
			return NULL;
		}
	
		//Enqueue the token
		if(enqueue_token(tokenQueue, token) < 0)
		{
			return NULL;
		}
		
		NEXT_TOKEN:
		lastToken = token;
	}	
	while(*cursor != '\0');

	return tokenQueue;
}

int test_print_tokens(token_queue_s *queue)
{
	if(queue == NULL)
	{
		return -1;
	}
	
	for(int i = 0; i < queue->tail; i++)
	{
		fprintf(stderr, "%s    %s\n",
			(queue->token[i]->type == CONSTANT_TOKEN) ? "CONSTANT" :
			(queue->token[i]->type == VARIABLE_TOKEN) ? "VARIABLE" : 
			(queue->token[i]->type == FUNCTION_TOKEN) ? "FUNCTION" : 
			(queue->token[i]->type == OPERATOR_TOKEN) ? "OPERATOR" :
			"UNDEFINED", queue->token[i]->text);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage:%s <expression>\n", argv[0]);
		return EXIT_FAILURE;
	}
                                                              
//	printf("%f\n", old_shunting_yard(argv[1]));

	token_queue_s *tokenQueue = tokenize_equation(argv[1]);
	if(tokenQueue == NULL)
	{
		return EXIT_FAILURE;
	}
	fprintf(stderr, "Token Queue: \n");
	test_print_tokens(tokenQueue);

	token_queue_s *outputQueue = new_shunting_yard(tokenQueue);
	fprintf(stderr, "Output Queue: \n");
	test_print_tokens(outputQueue);

	return EXIT_SUCCESS;
}



