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

static token_t     tok;
static location_t  loc;

/********
    A parse error has occurred.  Print error message and halt.
 ********/
static void parse_error()
{
    fprintf(stderr, "%c\n", get_character(&(tok.location)));
    fprintf(stderr, "Syntax error");
    fprintf(stderr, " at line %d, column %d\n",
        tok.location.line->line_num, tok.location.column);
    exit(1);
}

int returnType(char * value){

    switch(value[0]){
        case '+':
            return t_PLUS;
            break;
        case '-':
            return t_MINUS;
            break;
        case '(':
            switch(value[1]){
                case '+':
                    switch(value[2]){
                        case '+':
                            return t_INCREMENT;
                            break;
                        case ')':
                            return t_PLUS_UNARY;
                            break;
                        default:
                            parse_error();
                            break;
                    }
                    break;
                case '-':
                    switch(value[2]){
                        case '-':
                            return t_DECREMENT;
                            break;
                        case ')':
                            return t_MINUS_UNARY;
                            break;
                        default:
                            parse_error();
                            break;
                    }
                    break;
                default:
                    return t_LPAREN;
                    break;
            }
            break;
        case '*':
            return t_STAR;
            break;
        case '%':
            return t_PCT;
            break;
        case '/':
            return t_SLASH;
            break;
        case ')':
            return t_RPAREN;
            break;
        case '!':
            return t_BANG;
            break;
        case '^':
            return t_CARET;
            break;
        case ';':
            return t_SEMIC;
            break;
        default:
            if(tok.tc == T_LITERAL)
                return t_LITERAL;
            else
                parse_error();
            break;
    }

    parse_error();

    return -1;

}

void addNodeLabel(int label, node_t * parent){

    node_t * child = malloc(sizeof(node_t));
    child->label = label;
    if(label > t_SEMIC)
        child->hasChildren = 0;
    else
        child->hasChildren = 1;
    child->parent = NULL;
    child->leftChild = NULL;
    child->rightSibling = NULL;

    parent->hasChildren = 1;
    child->parent = parent;

    if(parent->leftChild == NULL)
        parent->leftChild = child;
    else{

        node_t * current = parent->leftChild;

        while(current->rightSibling != NULL)
            current = current->rightSibling;

        current->rightSibling = child;

    }


}

void addNode(node_t* child, node_t * parent){

    if(parent->leftChild == NULL)
        parent->leftChild = child;
    else{

        node_t * current = parent->leftChild;

        while(current->rightSibling != NULL)
            current = current->rightSibling;

        current->rightSibling = child;

    }

}

char * advanceInput(){

    if(tok.tc != T_SEMIC){

            if(tok.tc != T_EOF && tok.tc != T_SPACE && tok.tc != T_NL_SPACE){
            
                char * current = malloc(sizeof(char)*tok.length + 1);

                int i;

                location_t locTemp = tok.location;

                for(i = 0; i < tok.length; i++)
                    current[i] = get_character(&(tok.location));

                tok.location = locTemp;

                current[i] = '\0';

                printf("%s", current);

                scan(&loc, &tok);
                return current;

            }
            else{

                if(tok.tc == T_EOF)
                    return "END";
                else{
                    
                    int i;

                    location_t locTemp = tok.location;
                
                    for(i = 0; i < tok.length; i++)
                        printf("%c", get_character(&(tok.location)));

                    tok.location = locTemp;

                    scan(&loc, &tok);
                    return advanceInput();
                
                }

            }
        
        }
        else{

            printf(";\n==");

            scan(&loc, &tok);
            return ";";

        }

}

/*node_t * pExpression(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_PLUS:
            node_t node1 = pTerm(node, terminal);
            node_t node2 = pExpTail(node, terminal);
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        case t_MINUS:
            node_t node1 = pTerm(node, terminal);
            node_t node2 = pExpTail(node, terminal);
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        case t_PLUS_UNARY:
            node_t node1 = pTerm(node, terminal);
            node_t node2 = pExpTail(node, terminal);
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        case t_MINUS_UNARY:
            node_t node1 = pTerm(node, terminal);
            node_t node2 = pExpTail(node, terminal);
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        case t_INCREMENT:
            node_t node1 = pTerm(node, terminal);
            node_t node2 = pExpTail(node, terminal);
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        case t_DECREMENT:
            node_t node1 = pTerm(node, terminal);
            node_t node2 = pExpTail(node, terminal);
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        case t_LPAREN:
            node_t node1 = pTerm(node, terminal);
            node_t node2 = pExpTail(node, terminal);
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        case t_LITERAL:
            node_t node1 = pTerm(node, terminal);
            node_t node2 = pExpTail(node, terminal);
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        default:
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pExpTail(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_PLUS:
        case t_MINUS:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_SEMIC:
        default:

    }

    return returnNode;

}
node_t * pPostIncrement(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_INCREMENT:
        case t_DECREMENT:
        default:

    }

    return returnNode;

}
node_t * pTerm(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:
        default:

    }

    return returnNode;

}
node_t * pTermTail(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_STAR:
        case t_PCT:
        case t_SLASH:
        case t_SEMIC:
        default:

    }

    return returnNode;

}
node_t * pFactor(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:
        default:

    }

    return returnNode;

}
node_t * pFactorHead(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:
        default:

    }

    return returnNode;

}
node_t * pSign(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:
        default:

    }

    return returnNode;

}
node_t * pIncrement(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:
        case t_SEMIC:
        default:

    }

    return returnNode;

}
node_t * pFactorTail(node_t * node, int terminal){

    node_t * returnNode = malloc(sizeof(node_t));

    switch(terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_STAR:
        case t_PCT:
        case t_SLASH:
        case t_BANG:
        case t_CARET:
        case t_LITERAL:
        default:

    }

    return returnNode;

}*/

/********
    Scan source, identify structure, and print appropriately.
 ********/
void parse(){

    //Calls from reader.c
    //sets the address of loc to be the first character
    //of the first line

    set_to_beginning(&loc);

    //Creating the parse tree
    node_t * root = malloc(sizeof(node_t));
    root->label = s_EXPRESSION + t_SEMIC + 1;
    root->hasChildren = 0;
    root->parent = NULL;
    root->leftChild = NULL;
    root->rightSibling = NULL;
    root->data = NULL;

    //These macros will come in handy when we have to re-initialize
    //the stack and tree between statements.       
    #define INIT_TREE()                                         \
        root = malloc(sizeof(node_t));                          \
        root->label = s_EXPRESSION + t_SEMIC + 1;               \
        root->hasChildren = 0;                                  \   
        root->parent = NULL;                                    \
        root->leftChild = NULL;                                 \
        root->rightSibling = NULL;                              \
        root->data = NULL;        

    scan(&loc, &tok);
    while(tok.tc != T_EOF){
        
        //recurse for statement HERE;
        advanceInput();
        
    }

}

void deleteNode(node_t *node){

    //Iterates through all the the branches
    //and then clears the lines from memory.
    if(node) {

        if(node->leftChild)
            deleteNode(node->leftChild);
        if(node->rightSibling)
            deleteNode(node->rightSibling);
        free(node);

    }

}