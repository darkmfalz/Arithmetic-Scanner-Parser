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
	s_FACTORTAIL,
	s_INCTAIL

} syntactic_group_class;

typedef struct node {

	int label; //syntactic groups should be t_SEMIC + syntactic_group_class
	int hasChildren;
	char * data; //this is for literals
	struct node * parent;
	struct node * leftChild;
	struct node * rightSibling;

} node_t;

void addNodeLabel(int label, node_t * parent);
void addNode(node_t * child, node_t * parent);
char * advanceInput();
void findNextInput();
//productions
node_t * pExpression();
node_t * pExpTail();
node_t * pPostIncrement();
node_t * pTerm();
node_t * pTermTail();
node_t * pFactor();
node_t * pFactorHead();
node_t * pSign();
node_t * pIncrement();
node_t * pFactorTail();
node_t * pIncTail();

float evaluate(node_t *node, float start);

void parse();
    /* Scan source, identify structure, and print appropriately. */

int adeebRound(float target);
float factorial(float target);
float exponent(float base, float index);
float _exponent(float base, float index, float precision);
float adeebSqrt(float target);

//print functions
void printNode(node_t *node, char * indent, int length, int last);
void printLabel(node_t * node);
void printNodeError(node_t *node, char * indent, int length, int last);
void printLabelError(node_t * node);

//delete functions
void deleteNode(node_t *node);

#endif
