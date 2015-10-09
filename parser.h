/**********************************************************************
    Java parser and pretty-printer.
 **********************************************************************/

#ifndef PARSER_H
#define PARSER_H

typedef enum {

	t_PLUS,
	t_MINUS,
	t_PLUS_UNARY,
	t_MINUS_UNARY,
	t_INCREMENT,
	t_DECREMENT,
	t_STAR,
	t_PCT,
	t_SLASH,
	t_LPAREN,
	t_RPAREN,
	t_BANG,
	t_CARET,
	t_LITERAL,
	t_SEMIC

} terminal_class;

typedef enum {

	s_EXPRESSION,
	s_EXPTAIL,
	s_POSTINCREMENT,
	s_TERM,
	s_TERMTAIL,
	s_FACTOR,
	s_FACTORHEAD,
	s_SIGN,
	s_INCREMENT,
	s_FACTORTAIL

} syntactic_group_class;

typedef struct node {

	int label; //syntactic groups should be t_SEMIC + syntactic_group_class
	struct node * leftChild;
	struct node * rightSibling;

} node_t;

typedef struct tree {

	node_t * root;
	int height;

} tree_t;

typedef struct s_node { //for stacks

	int label;
	struct s_node * next;

} s_node_t;

typedef struct stack {

	s_node_t * head;
	
} stack_t;

void parse();
    /* Scan source, identify structure, and print appropriately. */

//Stack functions
void push(stack_t *stack, int label);
void pop(stack_t *stack);

#endif
