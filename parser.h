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

	char * label;
	struct node * leftChild;
	struct node * rightSibling;

} node_t;

typedef struct tree {

	node_t root;

} tree_t;

void parse();
    /* Scan source, identify structure, and print appropriately. */

#endif
