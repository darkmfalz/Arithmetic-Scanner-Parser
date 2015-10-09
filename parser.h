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
	s_FACTORTAIL,
	s_SIGN,
	s_INCREMENT

} syntactic_group_class;

typedef struct node {

	int label; //syntactic groups should be t_SEMIC + syntactic_group_class
	struct node * leftChild;
	struct node * rightSibling;

} node_t;

typedef struct s_node { //for stacks

	int label;
	struct s_node * next;

} s_node_t;

typedef struct stack {

	s_node_t head;
	
} stack_t;

void parse();
    /* Scan source, identify structure, and print appropriately. */

#endif
