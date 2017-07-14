/***************************************************************
*                     postfixer.c
***************************************************************/

#include "tokenizer.h"

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
int pop_to_queue(token_stack_s *stack, token_queue_s *queue)
{
	//Pop from the stack
	token_s *topOp = pop_token(stack);
	if(topOp == NULL)
	{
		return -1;
	}

	//Push to the queue
	if(enqueue_token(queue, topOp) < 0)
	{
		return -1;
	}

	return 0;
}


/**
	Shunting-yard implementation on tokens object
*/
token_queue_s *shunting_yard(token_queue_s *tokenQueue)
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
		switch(token->type)
		{
			case VARIABLE_TOKEN:
			case CONSTANT_TOKEN:
			if(enqueue_token(outputQueue, token) < 0)
			{
				return NULL;
			}
			break;

			case FUNCTION_TOKEN:
			//Push functions to the operator stack
			push_token(operatorStack, token);
			break;			
			
			case OPERATOR_TOKEN:
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
					if(pop_to_queue(operatorStack, outputQueue) < 0)
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
					if(pop_to_queue(operatorStack, outputQueue) < 0)
					{
						return NULL;
					}
				}

				break;
			}

			//Vaporize commas			
			if(token->text[0] == ',')
			{
				break;	
			}

			//Look into the operator stack
			//	Pop all oprators from the top that have a >= precedence
			while(evaluate_operator_stack(operatorStack, token) == true)
			{
				//Pop the top operator into the output stack
				if(pop_to_queue(operatorStack, outputQueue) < 0)
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

