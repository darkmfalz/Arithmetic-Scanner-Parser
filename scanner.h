/**********************************************************************
    Tokenizer for Java source.
 **********************************************************************/

#ifndef SCANNER_H
#define SCANNER_H

#include "reader.h"

typedef enum {
    T_EOF, T_SPACE, /* intra-line space (no newline characters) */
    T_NL_SPACE,     /* inter-line space (includes >= 1 newlines) */
    T_ID_DEC,       /* identifier in its own declaration */
                    /* (not returned by scanner) */
    T_IDENTIFIER,   /* other identifier */
    T_OPERATOR, T_KWOPERATOR, T_LITERAL,                                //Modify, Java
    T_LPAREN, T_RPAREN,                                                 //Modify, Java
    T_SEMIC, T_DOT                                                      //Modify, Java
} token_class;

typedef struct {
    token_class tc;
    //This is the location from reader.h -- hence, the location
    //in the complete LinkedList of lines read from the file.
    //It is absolute.
    location_t location;
    //The length indicates how far from the start location the token goes on
    int length;     /* length of token in characters (may span lines) */
} token_t;

void scan(location_t * loc, token_t * tok);
    /* Modify tok to describe next token of input.
        Update loc to refer to location immediately after tok. */

#endif
