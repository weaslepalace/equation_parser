/***************************************************************
*                      tokenizer.c
***************************************************************/
#define _TOKENIZER_SOURCE

#include "tokenizer.h"
#include "operators.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>



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
	Reallocate of allocate an array of tokens
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
		stack->top = &token_prototype;
		return NULL;
	}

	//Remove and return the token at the top of the stack
	token_s *tok = stack->top;
	stack->depth--;
	if(stack->depth <= 0)
	{
		stack->top = &token_prototype;
	}
	else
	{
		stack->top = stack->token[stack->depth - 1];
	}

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
	stack->top = &token_prototype;
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
			if(token->type == VARIABLE_TOKEN)
			{
				return NULL;
			}
			
			//Do not include '(' as part of the function name
			token->text = new_token_text(*cursor, token->length);
			*cursor = c;
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
	token->type = VARIABLE_TOKEN;
	return token;
}

/**
	Parse a numeric constant
*/
token_s *parse_number(char **cursor)
{
	char *c = *cursor;

	//If the first character is '-', the second character must be 0-9
	if((*c == '-') && ((*(c + 1) < '0') || (*(c + 1) > '9')))
	{
		return NULL;
	}
 
	//Create a token object
	token_s *token = new_token();
	if(token == NULL)
	{
		return NULL;
	}

	//If the first character is zero, the second character must be '.'
	//	Otherwise, only the zero is valid
	if((*c == '0') && (*(c + 1) != '.')) 
	{
		token->length = 1;
		c++;
		goto END_OF_NUMBER;
	}

	bool exponent = false;
	bool point = false;
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
				goto END_OF_NUMBER;
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

	token->length = 1;
	token->text = new_token_text(*cursor, token->length);
	(*cursor)++;
	token->type = OPERATOR_TOKEN;
	return token;
}


token_s *parse_token(char **cursor)
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
	if((c >= '0') && (c <= '9'))
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
	TODO: This function needs more comments.
		The state table is more cryptic than the code itself.
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
			if(token->text[0] == '-')
			{
				return NEGATIVE_TOKEN;
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
			if((operator_prec(token->text) > 0) ||
				(token->text[0] == ')') || (token->text[0] == ','))
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
	Create a new token initialized with text and type
*/
token_s *make_token(token_type_e type, char *expr, int length)
{
	token_s *token = new_token();
	if(token == NULL)
	{
		return NULL;
	}
	token->type = type;
	token->length = length;
	token->text = new_token_text(expr, length);
	if(token->text == NULL)
	{
		return NULL;
	}

	return token;
}

/**
	Enqueue a call to negate(token) 
*/
int enqueue_negate(token_queue_s *tokenQueue, token_s *token)
{
	//Enqueue "neg" as a function
	token_s *negateCall = make_token(FUNCTION_TOKEN, "neg", 3);
	if(negateCall == NULL)
	{
		return -1;
	}
	if(enqueue_token(tokenQueue, negateCall) < 0)
	{
		return -1;
	}

	//Enqueue '('
	token_s *openParen = make_token(OPERATOR_TOKEN, "(", 1);
	if(openParen == NULL)
	{
		return -1;
	}
	if(enqueue_token(tokenQueue, openParen) < 0)
	{
		return -1;
	}

	//Enqueue token as the argument to "neg"
	if(enqueue_token(tokenQueue, token) < 0)
	{
		return -1;
	}

	//Enqueue ')'
	token_s *closeParen = make_token(OPERATOR_TOKEN, ")", 1);
	if(closeParen == NULL)
	{
		return -1;
	}
	if(enqueue_token(tokenQueue, closeParen) < 0)
	{
		return -1;
	}

	return 0;
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
	token_validator_e valid = VALID_TOKEN;
	token_validator_e lastValid = VALID_TOKEN;
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
		token_s *token = parse_token(&cursor);
		if(token == NULL)
		{
			fprintf(stderr, "Unable to parse a token\n");
			return NULL;
		}
			
		valid = validate_token(token, lastToken);
		switch(valid)
		{
			case VALID_TOKEN:
			if(lastValid == VALID_TOKEN)
			{
				//Enqueue the token
				if(enqueue_token(tokenQueue, token) < 0)
				{
					return NULL;
				}
			}
			else if(lastValid == NEGATIVE_TOKEN)
			{
				//Enqueue the token as an argument to a call to negate()
				if(enqueue_negate(tokenQueue, token) < 0)
				{
					return NULL;
				}
			}
			break;
			
			case NEGATIVE_TOKEN:
			break;
		
			case INVALID_TOKEN:
			return NULL;
		}
		
		lastToken = token;
		lastValid = valid;
	}	
	while(*cursor != '\0');

	return tokenQueue;
}


