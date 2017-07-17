/***************************************************************
*                      operators.h
***************************************************************/

#ifndef OPERATORS_H
#define OPERATORS_H

#include <stdbool.h>

float execute_add(int nArgs, float *args); 
float execute_sub(int nArgs, float *args);
float execute_mul(int nArgs, float *args);
float execute_div(int nArgs, float *args);
float execute_mod(int nArgs, float *args);
float execute_exp(int nArgs, float *args);
float execute_avg(int nArgs, float *args);
float execute_min(int nArgs, float *args);
float execute_max(int nArgs, float *args);


/**
	Operator Helpers
*/
typedef enum {
	RIGHT_ASSOC,
	LEFT_ASSOC,
	NO_ASSOC
}op_assoc_e;
int operator_prec(char *op);
bool operator_check(char c);
op_assoc_e operator_assoc(char *op);

/**
	Reference Linkers
*/
void *reference_operator(char *op);
void *reference_function(char *func);

#endif //OPERATORS_H
