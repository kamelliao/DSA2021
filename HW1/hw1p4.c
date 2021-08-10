/*
    Reference: @AnthonyDiGirolamo https://gist.github.com/AnthonyDiGirolamo/1179218/2faf71295ee853facac6dd4f2984672bd9dcb9ad
*/

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#define LINE 1000000
#define MAX 100000000

double num_stack[MAX];
char op_stack[MAX];
int n_num_stack = 0;
int n_op_stack = 0;


// functions for stack
void push_num(double num);
void push_op(char op);
double pop_num();
char pop_op();

// functions for algorithm
double arithmetic(double num1, double num2, char op);
int precedence(char op);
void eval(char* expression);

int main(void)
{
    char expression[LINE];
    while (fgets(expression, LINE, stdin) != NULL)
    {
        eval(expression);
        printf("%.15f\n", pop_num());
    }
    
    return 0;
}

// functions for stack
void push_num(double num)
{
    num_stack[n_num_stack++] = num;
}

void push_op(char op)
{
    op_stack[n_op_stack++] = op;
}

double pop_num()
{
    return num_stack[--n_num_stack];
}

char pop_op()
{
    return op_stack[--n_op_stack];
}

// functions for algorithm
double arithmetic(double num1, double num2, char op)
{
    switch (op)
    {
    case '+':
        return num1 + num2;
    case '-':
        return num1 - num2;
    case '*':
        return num1 * num2;
    case '/':
        return num1 / num2;
    }
    return 0;
}

int precedence(char op)
{
    switch (op)
    {
    case '+':
        return 1;
    case '-':
        return 1;
    case '*':
        return 3;
    case '/':
        return 3;
    case '(':
        return 0;
    case '\n':
        return 0;
    }
    return 0;
}

void eval(char* expression)
{
    char* num_start = NULL;
    double num1, num2;
    char new_op, op;
    for (char* curr = expression; *curr != '\0'; curr++)
    {
        if (isdigit(*curr))
        {
            // parse number
            if (!num_start)
            {
                num_start = curr;
            }
        }
        else
        {
            new_op = *curr;
            *curr = '\0';

            if (new_op == '(')
            {
                push_op(new_op);
            }
            else if (new_op == '\n')
            {
                if (num_start)
                {
                    push_num(atof(num_start));
                    num_start = NULL;
                }
            }
            else if (new_op == ')')
            {
                if (num_start)
                {
                    push_num(atof(num_start));
                    num_start = NULL;
                }

                while (op_stack[n_op_stack-1] != '(')
                {
                    op = pop_op();
                    num2 = pop_num();
                    num1 = pop_num();
                    push_num(arithmetic(num1, num2, op));
                }
                pop_op();  // pop '('
            }
            else
            {
                if (num_start)
                {
                    push_num(atof(num_start));
                    num_start = NULL;
                }

                if (n_op_stack > 0)
                {
                    while (precedence(new_op) <= precedence(op_stack[n_op_stack-1]))
                    {
                        op = pop_op();
                        num2 = pop_num();
                        num1 = pop_num();
                        push_num(arithmetic(num1, num2, op));
                    }
                }
                push_op(new_op);
            }
        }
    }

    while (n_op_stack > 0)
    {
        op = pop_op();
        num2 = pop_num();
        num1 = pop_num();
        push_num(arithmetic(num1, num2, op));
    }
}