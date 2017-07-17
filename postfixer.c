/***************************************************************
*                     postfixer.c
***************************************************************/

#include "tokenizer.h"
#include "operators.h"
#include "evaluator.h"

#include <stdbool.h>
#include <stdlib.h>

/**
 	Helper for Shunting-yard
	Return true if the stack top should be popped
*/
bool evaluate_operator_stack(token_stack_s *stack, token_s *op)
{
	if(stack->depth <= 0)
	{ 
		return false;
	}

	if(operator_assoc(op->text) == RIGHT_ASSOC)
	{
		if(operator_prec(op->text) < operator_prec(stack->top->text))
		{
			return true;
		}
		return false;
	}

	if(operator_assoc(op->text) != RIGHT_ASSOC)
	{
		if(operator_prec(op->text) <= operator_prec(stack->top->text))
		{
			return true;
		}
		return false;
	}

	return false;
}


/**
	Helper for Shunting-yard
*/
int pop_to_equation(token_stack_s *stack, equation_s *eq)
{
	//Pop from the stack
	token_s *topOp = pop_token(stack);
	if(topOp == NULL)
	{
		return -1;
	}

	//Convert the token to a term
	equation_term_s *term = evaluate_token(topOp);
	if(term == NULL)
	{
		return -1;
	}

	//Push to the equation
	if(push_equation_term(eq, term) < 0)
	{
		return -1;
	}

	return 0;
}

/**
	count_stack_s Object
*/
typedef struct {
	int depth;
	int *top;
	int *count;
} count_stack_s;
#define COUNT_ARRAY_BOUNDARY  16
static int nullCount = -1;

/**
	Push a count value to a count stack
*/
int push_count(count_stack_s *stack, int count)
{
	//Allow the stack to grow
	if((stack->depth % COUNT_ARRAY_BOUNDARY) == 0)
	{
		int *tmp = realloc(stack->count,
			(stack->depth + COUNT_ARRAY_BOUNDARY) * sizeof(count_stack_s));
		if(tmp == NULL)
		{
			free(stack->count);
			return -1;
		}
		stack->count = tmp;
	}

	//Add count to the stack
	stack->count[stack->depth] = count;
	stack->top = &stack->count[stack->depth];
	stack->depth++;

	return 0;
}

/**
	Pop a count value from a count stack
*/
int pop_count(count_stack_s *stack)
{
	//Is the stack empty?
	if(stack->depth <= 0)
	{
		stack->depth = 0;
		stack->top = &nullCount;
		
		return -1;
	}

	//Remove and return the count value at the top of the stack
	int count = *stack->top;
	stack->depth--;
	if(stack->depth <= 0)
	{
		stack->top = &nullCount;
	}
	else
	{
		stack->top = &stack->count[stack->depth - 1];
	}

	return count;
}

/**
	Shunting-yard implementation on tokens object
*/
equation_s *shunting_yard(token_queue_s *tokenQueue)
{
	//Create an output equation
	equation_s *output = new_equation();
	if(output == NULL)
	{
		return NULL;
	}

	//Create the operator stack	
	token_stack_s *operatorStack = new_token_stack();
	if(operatorStack == NULL)
	{
		return NULL;
	}

	//Make 2 more stack:
	//	One to store argument counts to functions
	//	I have no idea what the other one does, but maybe the url has the answer
	count_stack_s argCountStack = {.depth = 0, .top = &nullCount, .count = NULL};
	count_stack_s mysteryStack = {.depth = 0, .top = &nullCount, .count = NULL};

	//Execute the shunting-yard algorithm
	for(token_s *token = dequeue_token(tokenQueue);
		token != NULL;
		token = dequeue_token(tokenQueue))
	{

		//Push numeric constants, variable names, and function names
		//	 directly to the output queue
		switch(token->type)
		{
			case VARIABLE_TOKEN:
			case CONSTANT_TOKEN:
			{
//				if(enqueue_token(outputQueue, token) < 0)
//				{
//					return NULL;
//				}
				equation_term_s *term = evaluate_token(token);
				if(term == NULL)
				{
					return NULL;
				}
				if(push_equation_term(output, term) < 0)
				{
					return NULL;
				}
				
				//Update the top of the mystery stack to true
				if(mysteryStack.depth > 0)
				{
					*mysteryStack.top = 1;
				}
			}
			break;

			case FUNCTION_TOKEN:
			//Push functions to the operator stack
			push_token(operatorStack, token);
		
			//Update the top of the mystery stack to true
			if(mysteryStack.depth > 0)
			{
				*mysteryStack.top = 1;
			}
			
			//Push false to the mystery stack
			if(push_count(&mysteryStack, 0) < 0)
			{
				return NULL;
			}
			
			//Push 0 to the argument counter
			if(push_count(&argCountStack, 0) < 0)
			{
				return NULL;
			}
			break;			
			
			case OPERATOR_TOKEN:
			if(token->text[0] == ',')
			{
				//Pop the operator stack to the output until a '(' is encountered
				while(operatorStack->top->text[0] != '(')
				{
					if(pop_to_equation(operatorStack, output) < 0)
					{
						//Either the ',' or '(' are misplaces
						return NULL;	
					}
				}
				
				//Increment the argument count if the value at
				//	the top of the mystery stack indicates to do so
				if(*mysteryStack.top == 1)
				{
					(*argCountStack.top)++;
				}
				break;	
			}

			if(token->text[0] == '(')
			{
				//Push open-paren to the operator stack
				push_token(operatorStack, token);
				break;			
			}

			if(token->text[0] == ')')
			{
				//It is an error if the operator stack is empty
				//	when there is a ')' on the token queue
				if(operatorStack->depth <= 0)
				{
					return NULL;
				}

				//Find the matching '(' in the operator stack
				while(operatorStack->top->text[0] != '(')
				{
					//Pop all the operators between the parens into the output queue
					if(pop_to_equation(operatorStack, output) < 0)
					{
						//Mismatched parens will cause
						//	the operator stack to run out before finding the '('
						return NULL;
					}
				}
				//Once found, pop the '(' into the void
				pop_token(operatorStack);
			
				//If the new top of the operator stack is a function,
				//	pop it to the output queue
				if(operatorStack->top->type == FUNCTION_TOKEN)
				{
					if(pop_to_equation(operatorStack, output) < 0)
					{
						return NULL;
					}

					//Pop the argument count, increment it, and 
					//	store it as the output equation term's argument count
					int count = pop_count(&argCountStack);
					if(count < 0)
					{
						return NULL;
					}
					int state = pop_count(&mysteryStack);
					if(state < 0)
					{
						return NULL;
					}
					if(state == 1)
					{
						count++;
					}
					output->top->nArgs = count;
				}

				break;
			}


			//Look into the operator stack
			//	Pop all oprators from the top that have a >= precedence
			while(evaluate_operator_stack(operatorStack, token) == true)
			{
				//Pop the top operator into the output stack
				if(pop_to_equation(operatorStack, output) < 0)
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
		equation_term_s *term = evaluate_token(topOp);
		if(term == NULL)
		{
			return NULL;
		}
		if(push_equation_term(output, term) < 0)
		{
			return NULL;
		}
//		if(enqueue_token(outputQueue, topOp) < 0)
//		{
//			return NULL;
//		}
	}

	return output;
}

