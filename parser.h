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
	t_EPSILON,
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
	int hasChildren;
	char * data;
	struct node * parent;
	struct node * leftChild;
	struct node * rightSibling;

} node_t;

int returnType(char * value);
void addNodeLabel(int label, node_t * parent);
void addNode(node_t * child, node_t * parent);
char * advanceInput();
//productions
node_t * pExpression(node_t * node, int terminal);
node_t * pExpTail(node_t * node, int terminal);
node_t * pPostIncrement(node_t * node, int terminal);
node_t * pTerm(node_t * node, int terminal);
node_t * pTermTail(node_t * node, int terminal);
node_t * pFactor(node_t * node, int terminal);
node_t * pFactorHead(node_t * node, int terminal);
node_t * pSign(node_t * node, int terminal);
node_t * pIncrement(node_t * node, int terminal);
node_t * pFactorTail(node_t * node, int terminal);

void parse();
    /* Scan source, identify structure, and print appropriately. */

//delete functions
void deleteNode(node_t *node);

#endif
