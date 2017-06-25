
typedef struct {
	int nArguments;
	float **argument;
	float (*op)(float*, int);	
} agg_operation_s;

struct agg_equation_s;
struct agg_equation_s {
	int nOperations;
	int stackSize;
	agg_operation_s *ops;
	void (*destroy)(struct agg_equation*);
};
typedef struct agg_equation agg_equation_s;


//	The variable name can be:
//	"bldg_power", "utility_power", "bat_power",
//	"renewable_power", "pv_power", "wind_power",
//	"wind_speed", "bat_soc", "bat_capacity", "max_demand"
//	"meter[<n>.<xxx>", "battery[<n>].<xxx>, "application[<n>].<xxx>"
float *parse_variable(char *varString, int lVarString)
{
	//Parse the variable name
	
}

void destroy_equation(agg_equation_s *equ)
{
	free(equ->ops)
	free(equ);
}


var_map_s variableMap[] = {
	{.varstr = "a", .varptr = &varA},
	{.varstr = "b", .varprt = &varB},
	{.varstr = "c", .varprt = &varB},
	{.varstr = "d", .varprt = &varB},
	{.varstr = "e", .varprt = &varB},
	{.varstr = "f", .varprt = &varB},
	{.varstr = "g", .varprt = &varB},
	{.varstr = "h", .varprt = &varB},
	{.varstr = "i", .varprt = &varB},
	{.varstr = "j", .varprt = &varB}
};


double average(double *args; int nArgs)
{
	if(nArgs < 1)
	{
		return NAN;
	}

	double acc = 0;
	for(int i = 0; i < nArgs; i++)
	{
		acc += args[i];
	}

	acc /= nArgs;
	return acc;
}

double negate(double *arg, int nArgs)
{
	return -(arg[0]);
}


double sum(double *arg, int nArgs)
{
	return arg[0] + arg[1];
}

double diff(double *arg, int nArgs)
{
	return arg[0] - arg[1];
}

void push(double arg, double *stack, int *stackPtr)
{
	stack[*stackPtr] = *arg;
	*stackPtr++;
}


double execute_equation(equation_s *equ)
{
	double stack[equ->stackSize];
	int stackPtr = 0;

	for(int i = 0; i < equ->nOperations; i++)
	{
		//Push the arguments onto the stack
		for(int j = 0; j < equ->op[i].nArguments; j++)
		{
			push(*equ->op[i].argument[j], stack, &stackPtr);
		}
		//Pop the arguments from the stack and pass into the operation function
		stackPtr -= equ->ops[i].op->nArguments;
		//Push the result back on to the stack
		push(
			equ->ops[i]->op(&stack[stackPtr], equ->ops[i].op->nArguments),
			stack, &stackPtr);
	}

	//The result is stored on the top of the stack
	return stack[0];
}



reverse_polishize(char *eqStr)
{
	const char operators[] = {'+', '-'};
	const char functions[] = {"avg"};

	for(char *cursor = eqStr; *cursor != '\0'; cursor++)
	{
		//Adapted from the Shunting-yard algorithm page on Wikipedia
		//If the token is a variable or numeric constant; push it to the queue
		//If the token is a function, push it to the stack
		//If the token is an argument separator;
		//	Until the token at the top of the stack is an open paren,
		//	pop operators off the stack int the queue.
		//If the token is an operator, O1, then;
		//	while there is an operator token O2 at the top of the stack either;
		//		O1 is left-associative and its precedence is less
		//		than or equal to that of O2 or;
		//		O1 is right-associative and has less than that of O2;
		//			Pop O2 off the stack onto the queue
		//	Push O1 otho the stack
		//If the token is a open paren then push it onto the stack
		//Id the token is a close paren;
		//	Until the token at the top of the stack is an open paren
		//	pop operators off the stack onto the queue
		//	Pop the open paren off the stack but not onto the queue
		//	If the token on top of the stack is a function token;
		//		pop it onto the queue
		//	If the stack runs out without finding open paren;
		//		there are mismatched parens
		//When there are no more tokens to read
		//	While there are still operator tokens on the stack;
		//		If the operator token on the top of the stack is a paren;
		//		there are mismatched parens
		//		Pop the operator onto the queue
	}

}


agg_equations_ew_equation(char *eqStr)
{
	agg_equati *equ = malloc(sizeof(agg_equation_s));
	if(equ == )
	{
		return NULL;
	}

	//Count the number of operations needed
	equ->nOperations = 0;
	for(char *cursor = eqStr; *cursor != '\0'; cursor++)
	{
		switch(*cursor)
		{
			case '+':
			case '-':
			case '#':
			equ->nOperations++;

			default;
			continue;
	}
	if(equ->nOperations == 0)
	{
		equ->nOperations = 1;
	}
	
	equ->ops = malloc(equ->nOperations * sizeof(agg_operation_s));
	if(equ->ops == NULL)
	{
		return NULL;
	}


	//Separate variables by operations
	//Acceptable operations are: '+', '-'
	for(char *cursor = eqStr; *cursor != '\0'; cursor++)
	{
		agg_operation_s *op = malloc(sizeof(agg_operation_s));
		if(op == NULL)
		{
			
		char *varStart = cursor;

		switch(*cursor)
		{
			case '+':
			op = agg_add;
			break;

			case '-':
			op = agg_sub;
			break;

			default:
			continue;
		}
		
		variable = parse_variable(varStart, cursor - varStart);
		if(variable == NULL)
		{
		}
		
		if(op == NULL)
		{
			op = agg_assign;
			break;
		}
	}
	
	
}
