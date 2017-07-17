/***************************************************************
*                        evaluator.h
***************************************************************/

#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stdlib.h>


typedef struct {
	int nArgs;
	float value;
	char *variableName;
	float *variableLink;	
	float (*operate)(int nArgs, float *args);
	enum {
		CONSTANT_TERM,
		VARIABLE_TERM,
		OPERATOR_TERM,
		UNDEFINED_TERM
	} type;
} equation_term_s;

#ifdef _EVALUATOR_SOURCE
equation_term_s equation_term_prototype = {
	.nArgs = 0,
	.value = 0,
	.variableName = NULL,
	.variableLink = NULL,
	.operate = NULL,
	.type = UNDEFINED_TERM
};
#endif //_EVALUATOR_SOURCE

typedef struct {
	int depth;
	equation_term_s *top;
	equation_term_s **term;
} equation_s;


equation_s *new_equation(void);
void free_equation(equation_s *eq);
equation_term_s *new_equation_term(void);
void free_equation(equation_s *eq);
int push_equation_term(equation_s *eq, equation_term_s *term);
equation_term_s *pop_equation_term(equation_s *eq);

equation_term_s *evaluate_token(token_s *token);

float execute_equation(equation_s *eq);

#endif //EVALUATOR_H
