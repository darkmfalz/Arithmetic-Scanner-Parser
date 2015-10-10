/**********************************************************************
    Tokenizer for Java source.
 **********************************************************************/

#ifndef SCANNER_H
#define SCANNER_H

#include "reader.h"
#include "parser.h"

typedef enum {
    T_EOF, T_SPACE, /* intra-line space (no newline characters) */
    T_NL_SPACE,     /* inter-line space (includes >= 1 newlines) */
    T_UNARY, T_INCREMENT,       //My own personal token types
    T_OPERATOR, T_LITERAL,                                             
    T_LPAREN, T_RPAREN,                                                
    T_SEMIC, T_DOT                                                      
} token_class;

typedef struct {
    token_class tc;
    //This is the location from reader.h -- hence, the location
    //in the complete LinkedList of lines read from the file.
    //It is absolute.
    terminal_class terminal;
    location_t location;
    //The length indicates how far from the start location the token goes on
    int length;     /* length of token in characters (may span lines) */
} token_t;

void scan(location_t * loc, token_t * tok);
    /* Modify tok to describe next token of input.
        Update loc to refer to location immediately after tok. */

#endif
