/***************************************************************
*                      evaluator.c
***************************************************************/
#define _EVALUATOR_SOURCE

#include "tokenizer.h"
#include "operators.h"
#include "postfixer.h"
#include "evaluator.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>

#define TERM_ARRAY_BOUNDARY		16
equation_term_s **resize_term_array(equation_term_s **array, int size);
void free_term_array(equation_term_s **array, int size);


/**
	equation_term_s Object Constructor
*/
equation_term_s *new_equation_term(void)
{
	//Allocate an empty equation_term_s object
	equation_term_s *term = malloc(sizeof(equation_term_s));
	if(term == NULL)
	{
		return NULL;
	}

	//Initialize
	memcpy(term, &equation_term_prototype, sizeof(equation_term_s));

	return term;
}


/**
	Resize an array of equation terms
*/
equation_term_s **resize_term_array(equation_term_s **array, int size)
{
	//Has the array grown beyond it's boundary?
	if((size % TERM_ARRAY_BOUNDARY) == 0)
	{
		//Expand the array to the more terms
		equation_term_s **tmp = realloc(array,
			(size + TERM_ARRAY_BOUNDARY) * sizeof(equation_term_s*));
		if(tmp == NULL)
		{
			free_term_array(array, size);
			return NULL;
		}
		array = tmp;
	}

	return array;
}

/**
	Free the contents of an equation term array
*/
void free_term_array(equation_term_s **array, int size)
{
	for(int i = 0; i < size; i++)
	{
		if(array[0] != NULL)
		{
			free(array[i]);
			array[i] = NULL;
		}
	}
	free(array);
}



/**
	equation_s Object Constructor
*/
equation_s *new_equation(void)
{
	//Allocate an empty equation_s object
	equation_s *eq = malloc(sizeof(equation_s));
	if(eq == NULL)
	{
		return NULL;
	}

	//Initialize
//	eq->term = NULL;
//	eq->tail = 0;
//	eq->head = 0;
	eq->term = NULL;
	eq->top = &equation_term_prototype;
	eq->depth = 0;

	return eq;
}

/**
	equation_s Object Destructor
*/
void free_equation(equation_s *eq)
{
	if(eq->term != NULL)
	{
		free(eq->term);
	}
	free(eq);
}

/**
	Push an equation_term_s object to an equation
*/
int push_equation_term(equation_s *eq, equation_term_s *term)
{
	//Allow the queue to grow as needed
	eq->term = resize_term_array(eq->term, eq->depth);
	if(eq->term == NULL)
	{
		return -1;
	}

	//Add the term to the top of equation
	eq->top = term;
	eq->term[eq->depth] = eq->top;
	eq->depth++;

	return 0;	
}

/**
	Pop an equation_term_s object from an equation
*/
equation_term_s *pop_equation_term(equation_s *eq)
{
	//Check if the equation is empty
	if(eq->depth <= 0)
	{
		//Reset the equation's parameters to reflect it's emptiness
		eq->depth = 0;
		eq->top = &equation_term_prototype;
		return NULL;
	}
		
	//Remove and return the term at the top of the equation
	equation_term_s *term = eq->top;
	eq->depth--;
	if(eq->depth <= 0)
	{	
		eq->top = &equation_term_prototype;
	}
	else
	{
		eq->top = eq->term[eq->depth - 1];
	}

	return term;
}




/**
	Convert a token into an equation term
*/
equation_term_s *evaluate_token(token_s *token)
{
	//Create a new term to convert the token to
	equation_term_s *term = new_equation_term();
	if(term == NULL)
	{
		return NULL;
	}

	switch(token->type)
	{
		case CONSTANT_TOKEN:
		term->value = strtof(token->text, NULL);
		term->type = CONSTANT_TERM;
		break;

		case VARIABLE_TOKEN:
		term->variableName = malloc(strlen(token->text) + 1);
		if(term->variableName == NULL)
		{
			return NULL;
		}
		strcpy(term->variableName, token->text);
		term->type = VARIABLE_TERM;
		break;

		case FUNCTION_TOKEN:
		term->operate = reference_function(token->text);
		if(term->operate == NULL)
		{
			return NULL;
		}
		term->type = OPERATOR_TERM;
		break;		

		case OPERATOR_TOKEN:
		term->operate = reference_operator(token->text);
		if(term->operate == NULL)
		{
			return NULL;
		}
		term->nArgs = 2;
		term->type = OPERATOR_TERM;
		break;

		default:
		return NULL;
	}

	return term;
}

typedef struct {
	int depth;
	float *top;
	float *operand;
} result_s;

#define OPERAND_ARRAY_BOUNDARY	16
float *resize_operand_array(float *array, int size)
{
	//Has the array grown beyond it's boundary
	if((size % OPERAND_ARRAY_BOUNDARY) == 0)
	{
		//Expand the array to fit more operands
		float *tmp = realloc(array,
			(size + OPERAND_ARRAY_BOUNDARY) * sizeof(float));
		if(tmp == NULL)
		{
			free(array);
			return NULL;
		}
		array = tmp;
	}
	return array;
}

int push_result(result_s *result, float operand)
{
	//Allow the stack to grow as needed
	result->operand = resize_operand_array(result->operand, result->depth);
	if(result->operand == NULL)
	{
		return -1;
	}

	//Add the operand to the top of the stack
	result->operand[result->depth] = operand;
	result->top = &result->operand[result->depth];
	result->depth++;

	return 0;
}

float pop_result(result_s *result)
{
	//Is the stack empty?
	if(result->depth <= 0)
	{
		result->depth = 0;
		result->top = NULL;
		return NAN;
	}		
	
	//Remove and return the operand at the top of the stack
	float operand = *result->top;
	result->depth--;
	if(result->depth <= 0)
	{
		result->top = NULL;
	}
	else
	{
		result->top = &result->operand[result->depth - 1];
	} 

	return operand;
}



/**
	Execute an expression in reverse-polish-notation 
*/
float execute_equation(equation_s *eq)
{
	//result is a stack that holds the expression's operands 
	result_s result = {.depth = 0, .top = NULL, .operand = NULL};

	//Execute the expression
	for(int i = 0; i < eq->depth; i++)
	{
		switch(eq->term[i]->type)
		{
			//Push constants and variable immediatly to the result stack
			case VARIABLE_TERM:
			if(push_result(&result, *eq->term[i]->variableLink) < 0)
			{
				return NAN;
			}
			break;

			case CONSTANT_TERM:
			if(push_result(&result, eq->term[i]->value) < 0)
			{
				return NAN;
			}
			break;

			case OPERATOR_TERM:
			{
				//Pop the number of arguments from the result stack
				//	need for the function of variable
				float argList[eq->term[i]->nArgs];
				for(int i = 0; i < eq->term[i]->nArgs; i++)
				{
					float arg = pop_result(&result);
					if(isnan(arg))
					{
						return NAN;
					}
					argList[i] = arg;
				}
				//Execute the operation
				float res = eq->term[i]->operate(eq->term[i]->nArgs, argList);
	
				//Push the result back to the result stack
				if(push_result(&result, res) < 0)
				{
					return NAN;
				}
			}
	
			default:
			return NAN;
		}
	}

	//The final result is at the top of the result stack
	float final = *result.top;

	//Clean up the result stack
	if(result.operand != NULL)
	{	
		free(result.operand);
	}

	return final;
}
