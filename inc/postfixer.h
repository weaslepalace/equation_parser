/***************************************************************
*                     postfixer.h
***************************************************************/

#ifndef POSTFIXER_H
#define POSTFIXER_H

#include "evaluator.h"

equation_s *shunting_yard(token_queue_s *tokenQueue);

#endif //POSTFIXER_H
