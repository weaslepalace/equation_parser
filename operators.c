/***************************************************************
*                       operators.c
***************************************************************/

#include "operators.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/**
	Return the operator precedence
*/
int operator_prec(char *op)
{
	//Return precedence on single-character operators
	switch(op[0])
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
	Return the operator associativity
*/
op_assoc_e operator_assoc(char *op)
{
	switch(op[0])
	{
		case '^':
		return RIGHT_ASSOC;
		
		case '*':
		case '/':
		case '%':
		case '+':
		case '-':
		return LEFT_ASSOC;
	}

	return NO_ASSOC;
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

/**
	Operator executions
*/

float execute_add(int nArgs, float *args)
{
	return args[1] + args[0];
}

float execute_sub(int nArgs, float *args)
{
	return args[1] - args[0];
}

float execute_mul(int nArgs, float *args)
{
	return args[1] * args[0];
}

float execute_div(int nArgs, float *args)
{
	return args[1] / args[0];
}

float execute_mod(int nArgs, float *args)
{
	//The arguments must be a whole integers
	if((args[0] - (int)args[0] != 0) || 
		(args[1] - (int)args[1] != 0))
	{
		return NAN;
	}

	return (int)args[1] % (int)args[0];
}

float execute_exp(int nArgs, float *args)
{
	return powf(args[1], args[0]);
}


/**
	Function executions
*/

float execute_neg(int nArgs, float *args)
{
	return -args[0];
}

float execute_avg(int nArgs, float *args)
{
	if(nArgs == 0)
	{
		return 0;
	}

	float acc = 0;
	for(int i = 0; i < nArgs; i++)
	{
		acc += args[i];	
	}
	return acc / nArgs;
}

float execute_min(int nArgs, float *args)
{
	float min = args[0];
	for(int i = 1; i < nArgs; i++)
	{
		if(args[i] < min)
		{
			min = args[i];
		}
	}

	return min;
}

float execute_max(int nArgs, float *args)
{
	float max = args[0];
	for(int i = 1; i < nArgs; i++)
	{
		if(args[i] > max)
		{
			max = args[i];
		}
	}

	return max;
}


/**
	Return a function pointer cooresponging to the operator character op
*/
void *reference_operator(char *op)
{
	switch(op[0])
	{
		case '+':
		return execute_add;

		case '-':
		return execute_sub;

		case '*':
		return execute_mul;

		case '/':
		return execute_div;

		case '%':
		return execute_mod;

		case '^':
		return execute_exp;

		default:
		return NULL;
	}
	return NULL;
}

/**
	Return a function pointer corresponding to the function name
*/
void *reference_function(char *func)
{
	if(strcmp("neg", func) == 0)
	{
		return execute_neg;
	}
	if(strcmp("avg", func) == 0)
	{
		return execute_avg;
	}
	if(strcmp("min", func) == 0)
	{
		return execute_min;
	}
	if(strcmp("max", func) == 0)
	{
		return execute_max;
	}
	
	return NULL;
}

