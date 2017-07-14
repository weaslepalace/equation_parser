#include "tokenizer.h"
#include "postfixer.h"

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

	token_queue_s *outputQueue = shunting_yard(tokenQueue);
	fprintf(stderr, "Output Queue: \n");
	test_print_tokens(outputQueue);

	return EXIT_SUCCESS;
}

