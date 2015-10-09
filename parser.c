/**********************************************************************
    Java parser and pretty-printer.

    Uses a recursive-descent parser that accepts a superset of the
    language.

    At the moment it's an unreasonably large superset: basically any
    string of tokens with balanced curvy braces is accepted inside a
    class definition.  You have to fix that by fleshing out the routines
    that parse a compound_stmt.  You also have to
        - find all declared identifiers, so you can print them red
        - indent things appropriately
        - fix inter-token spaces

 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include "scanner.h"
#include "parser.h"

static (*parseTable[t_SEMIC+1][s_INCREMENT+1])(); //Fill input parameters
static node_t root;

static stack_t stack;

static token_t     tok;
static location_t  loc;

static int indent_level = 0;
#define    INDENT_WIDTH   4

//stack MACROS
#define POP()  \
    stack->head = stack->head->next;
#define PUSH(t)                             \   //t is the label of the s_node
    s_node_t oldHead = stack->head;         \
    stack->head = malloc(sizeof(stack_t));  \
    stack->head->label = t;                 \
    stack->head->next = oldHead;

/********
    A parse error has occurred.  Print error message and halt.
 ********/
static void parse_error()
{
    fprintf(stderr, "Syntax error");
    fprintf(stderr, " at line %d, column %d\n",
        tok.location.line->line_num, tok.location.column);
    exit(1);
}

/********
    Scan source, identify structure, and print appropriately.
 ********/
void parse()
{
    //Calls from reader.c
    //sets the address of loc to be the first character
    //of the first line
    set_to_beginning(&loc);

    tok.tc = T_LITERAL;
    
    while(tok.tc != T_EOF){

        scan(&loc, &tok);
        
        if(tok.tc != T_SEMIC){

            int i;
        
            //Do stuff, then print;
            for(i = 0; i < tok.length; i++)
                printf("%c", get_character(&(tok.location)));
        
        }
        else
            printf(";\n==");
        
    }

}