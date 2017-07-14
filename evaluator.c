/***************************************************************
*                      evaluator.c
***************************************************************/


#include "tokenizer.h"
#include "postfixer.h"


//typedef struct {
//	int nArgs;
//	float ** args;
//	float execute;
//} equation_s;

typedef struct {
	int nArgs;
	float constant;
	float *variable;
	float (*operate)(int nArgs, float *args);
	enum {
		CONSTANT_TERM,
		VARIABLE_TERM,
		OPERATOR_TERM,
		UNDEFINED_TERN
	} type;
} equation_term_s;

equation_term_s equation_term_prototype = {
	.nArgs = 0,
	.value = 0,
	.variable = NULL,
	.operator = NULL,
	.type = UNDEFINED_TERM
};



equation_s *execute_rpn(equation_s *eq)
{
	// 324*15-23^^/+
	// Push 3 to stack
	// Push 2 to stack
	// Push 4 to stack
	// '*' takes two arguments
	//    Pop twice and multiply
	//    Push result(2*4 = 6) to stack
	// Push 1 to stack
	// Push 5 to stack
	// '-' takes two arguments
	//    Pop twice and subtrack
	//    Push result ((1 - 5) = -4) to stack
	// Push 2 to stack
	// Push 3 to stack
	// '^' takes two arguments
	//    Pop twice and exponentiate
	//    Push result (2^3 = 8) to stack
	// '^'  takes two arguments
	// 	  Pop twice and exponentiate
	// 	  Push result (-4 ^ 8 = 65536) to the stack
	// '/' takes two arguments
	// 	  Pop twice and exponentiate
	// 	  Push result (8 / (65536) = 1/8192) to the stack
	// '+' takes two arguemnts
	//    Pop twice and add
	//    Push result (3 + (1/8192)) = ~3) to the stack
	//	Return the value at the top of the stack
	
	token_stack_s *resultStack = new_token_stack();
	if(resultStack == NULL)
	{
		return NULL;
	}

	for(token_s *token = dequeue_token(rpnQueue);
		token != NULL;
		token = dequeue_token(tokenQueue))
	{
		if(resultStack->type == NUMBER
	}	


}
