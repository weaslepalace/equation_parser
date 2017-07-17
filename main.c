#include "tokenizer.h"
#include "operators.h"
#include "postfixer.h"
#include "evaluator.h"

#include <stdio.h>
#include <stdlib.h>

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

int test_print_equation(equation_s *eq)
{
	if(eq == NULL)
	{
		return -1;
	}

	for(int i = 0; i < eq->depth; i++)
	{
		switch(eq->term[i]->type)
		{
			case CONSTANT_TERM:
			fprintf(stderr, "%f ", eq->term[i]->value);
			break;

			case VARIABLE_TERM:
			fprintf(stderr, "%s ", eq->term[i]->variableName);
			break;

			case OPERATOR_TERM:
			fprintf(stderr, "%s ", 
				(eq->term[i]->operate == execute_add) ? "+" :
				(eq->term[i]->operate == execute_sub) ? "-" :
				(eq->term[i]->operate == execute_mul) ? "*" :
				(eq->term[i]->operate == execute_div) ? "/" :
				(eq->term[i]->operate == execute_mod) ? "%" :
				(eq->term[i]->operate == execute_exp) ? "^" :
				(eq->term[i]->operate == execute_avg) ? "avg" :
				(eq->term[i]->operate == execute_min) ? "min" :
				(eq->term[i]->operate == execute_max) ? "max" : "null");
			break;

			default:
			fprintf(stderr, "Undef ");
		}
	}
	fprintf(stderr, "\n");

	return 0;
}


int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage:%s <expression>\n", argv[0]);
		return EXIT_FAILURE;
	}
                                                              
	token_queue_s *tokenQueue = tokenize_equation(argv[1]);
	if(tokenQueue == NULL)
	{
		return EXIT_FAILURE;
	}
	fprintf(stderr, "Token Queue: \n");
	test_print_tokens(tokenQueue);

	equation_s *equation = shunting_yard(tokenQueue);
	fprintf(stderr, "Output Equation: \n");
	test_print_equation(equation);

	return EXIT_SUCCESS;
}

