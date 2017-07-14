/***************************************************************
*                     tokenizer.h
***************************************************************/

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdbool.h>

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
	token Object
*/
typedef struct {
	enum {
		VARIABLE_TOKEN,
		FUNCTION_TOKEN,
		CONSTANT_TOKEN,
		OPERATOR_TOKEN,
		COUNT_TOKEN,
		UNDEFINED_TOKEN
	} type;
	int length;
	char *text;
}token_s;
#ifdef _TOKENIZER_SOURCE
token_s token_prototype = {
	.type = UNDEFINED_TOKEN,
	.text = "", .length = 0
};
#endif //_TOKENIZER_SOURCE

typedef enum {
	INVALID_TOKEN,
	VALID_TOKEN,
	NEGATIVE_TOKEN
} token_validator_e;

token_s *new_token(void);
void free_token(token_s *token);
char *new_token_text(char *expr, int length);
token_s **resize_token_array(token_s **array, int size);
void free_token_array(token_s **array, int size);


/**
	token_queue_s Object
*/
typedef struct {
	token_s **token;
	int head;
	int tail;
} token_queue_s;
token_queue_s *new_token_queue(void);
int enqueue_token(token_queue_s *queue, token_s *token);
token_s *dequeue_token(token_queue_s *queue);
void free_token_queue(token_queue_s *queue);


/**	
	token_stack_s Object
*/
typedef struct {
	token_s **token;
	token_s *top;
	int depth;	
} token_stack_s;
token_stack_s *new_token_stack(void);
int push_token(token_stack_s *stack, token_s* token);
token_s *pop_token(token_stack_s *stack);
void free_token_stack(token_stack_s *stack);

token_s *parse_name(char **cursor);
token_s *parse_number(char **cursor);
token_s *parse_operator(char **cursor);
token_s *parse_token(char **cursor, token_validator_e isNeg);
token_queue_s *tokenize_equation(char *expr);



#endif //TOKENIZER_H
