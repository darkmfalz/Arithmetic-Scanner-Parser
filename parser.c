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

static void (*parseTable[t_SEMIC+1][s_INCREMENT+1])(); //Fill input parameters

static token_t     tok;
static location_t  loc;

static int indent_level = 0;
#define    INDENT_WIDTH   4

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
    
    stack_t *stack = malloc(sizeof(stack_t));
    push(stack, s_EXPRESSION);

    tree_t *parseTree = malloc(sizeof(tree_t));
    parseTree->root = malloc(sizeof(node_t));
    parseTree->root->label = s_EXPRESSION;
    parseTree->height++;

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

void push(stack_t *stack, int label){
    
    s_node_t* oldHead = stack->head;         
    stack->head = malloc(sizeof(s_node_t));  
    stack->head->label = label;                
    stack->head->next = oldHead;
    
}

void pop(stack_t *stack){

    s_node_t* oldHead = stack->head;     
    stack->head = stack->head->next;
    free(oldHead);
    
}