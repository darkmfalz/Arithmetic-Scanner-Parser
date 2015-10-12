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
static int isInt;

/********
    A parse error has occurred.  Print error message and halt.
 ********/
static void parse_error()
{    
    fprintf(stderr, "Syntax error");
    fprintf(stderr, " at line %d, column %d\n",
        tok.location.line->line_num, tok.location.column);
    fprintf(stderr, "Problem Character: %c\n", get_character(&(tok.location)));
    exit(1);
}

static void evaluate_error(node_t* node){
    fprintf(stderr, "Evaluation error\n");
    fprintf(stderr, "Problem Node: \n");
    char * indent = " ";
    printNodeError(node, indent, 1, 1);
    exit(1);
}

void addNodeLabel(int label, node_t * parent){

    node_t * child = malloc(sizeof(node_t));
    child->label = label;
    if(label > t_SEMIC)
        child->hasChildren = 0;
    else
        child->hasChildren = 1;
    child->data = NULL;
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
                findNextInput();
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

            printf(";");

            scan(&loc, &tok);
            return ";";

        }

}

void findNextInput(){

    if(tok.tc == T_SPACE || tok.tc == T_NL_SPACE){

        int i;

        for(i = 0; i < tok.length; i++)
            printf("%c", get_character(&(tok.location)));

        scan(&loc, &tok);
        findNextInput();
                
    }

}

node_t * pExpression(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_EXPRESSION + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:{
            node_t *node1 = pTerm();
            node_t *node2 = pExpTail();
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        }
        default:
            printf("erin");
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pExpTail(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_EXPTAIL + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:{
            advanceInput();
            addNodeLabel(t_PLUS, returnNode);
            node_t *node2 = pTerm();
            node_t *node3 = pExpTail();
            addNode(node2, returnNode);
            addNode(node3, returnNode);
            break;
        }
        case t_MINUS:{
            advanceInput();
            addNodeLabel(t_MINUS, returnNode);
            node_t *node2 = pTerm();
            node_t *node3 = pExpTail();
            addNode(node2, returnNode);
            addNode(node3, returnNode);
            break;
        }
        case t_INCREMENT:
        case t_DECREMENT:{
            node_t *node1 = pPostIncrement();
            node_t *node2 = pExpTail();
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        }
        case t_RPAREN:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        case t_SEMIC:{
            advanceInput();
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        default:
            printf("nina");
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pPostIncrement(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_POSTINCREMENT + t_SEMIC + 1;

    switch(tok.terminal){

        case t_INCREMENT:
        case t_DECREMENT:{
            node_t *node1 = pIncrement();
            node_t *node2 = pIncTail();
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        }
        default:

            printf("jane");
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pTerm(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_TERM + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:{
            node_t *node1 = pFactor();
            node_t *node2 = pTermTail();
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        }
        default:
            printf("alisa");
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pTermTail(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_TERMTAIL + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:
        case t_MINUS:
        case t_INCREMENT:
        case t_DECREMENT:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        case t_STAR:{
            advanceInput();
            addNodeLabel(t_STAR, returnNode);
            node_t *node2 = pFactor();
            node_t *node3 = pTermTail();
            addNode(node2, returnNode);
            addNode(node3, returnNode);
            break;
        }
        case t_PCT:{
            advanceInput();
            addNodeLabel(t_PCT, returnNode);
            node_t *node2 = pFactor();
            node_t *node3 = pTermTail();
            addNode(node2, returnNode);
            addNode(node3, returnNode);
            break;
        }
        case t_SLASH:{
            advanceInput();
            addNodeLabel(t_SLASH, returnNode);
            node_t *node2 = pFactor();
            node_t *node3 = pTermTail();
            addNode(node2, returnNode);
            addNode(node3, returnNode);
            break;
        }
        case t_RPAREN:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        case t_SEMIC:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        default:
            printf("colleen");
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pFactor(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_FACTOR + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:{
            node_t *node1 = pFactorHead();
            node_t *node2 = pFactorTail();
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            break;
        }
        default:
            printf("sam");
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pFactorHead(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_FACTORHEAD + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:{
            node_t *node1 = pSign();
            node_t *node2 = pIncrement();
            node_t *node3 = pFactorHead();
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            addNode(node3, returnNode);
            break;
        }
        case t_INCREMENT:
        case t_DECREMENT:{
            node_t *node1 = pIncrement();
            node_t *node2 = pSign();
            node_t *node3 = pFactorHead();
            addNode(node1, returnNode);
            addNode(node2, returnNode);
            addNode(node3, returnNode);
            break;
        }
        case t_LPAREN:{
            advanceInput();
            addNodeLabel(t_LPAREN, returnNode);
            node_t *node2 = pExpression();
            if(tok.terminal == t_RPAREN){
                advanceInput();
                addNode(node2, returnNode);
                addNodeLabel(t_RPAREN, returnNode);
            }
            else
                parse_error();
            break;
        }
        case t_LITERAL:{
            addNodeLabel(t_LITERAL, returnNode);
            returnNode->leftChild->data = advanceInput();
            break;
        }
        default:
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pSign(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_SIGN + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:{
            advanceInput();
            addNodeLabel(t_PLUS, returnNode);
            break;
        }
        case t_MINUS:{
            advanceInput();
            addNodeLabel(t_MINUS, returnNode);
            break;
        }
        case t_PLUS_UNARY:{
            advanceInput();
            addNodeLabel(t_PLUS_UNARY, returnNode);
            break;
        }
        case t_MINUS_UNARY:{
            advanceInput();
            addNodeLabel(t_MINUS_UNARY, returnNode);
            break;
        }
        case t_INCREMENT:
        case t_DECREMENT:
        case t_LPAREN:
        case t_LITERAL:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        default:
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pIncrement(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_INCREMENT + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        case t_INCREMENT:{
            advanceInput();
            addNodeLabel(t_INCREMENT, returnNode);
            break;
        }
        case t_DECREMENT:{
            advanceInput();
            addNodeLabel(t_DECREMENT, returnNode);
            break;
        }
        case t_LPAREN:
        case t_RPAREN:
        case t_LITERAL:
        case t_SEMIC:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        default:
            printf("ava");
            parse_error();
            break;

    }

    return returnNode;

}
node_t * pFactorTail(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_FACTORTAIL + t_SEMIC + 1;

    switch(tok.terminal){

        case t_PLUS:
        case t_MINUS:
        case t_PLUS_UNARY:
        case t_MINUS_UNARY:
        case t_INCREMENT:
        case t_DECREMENT:
        case t_STAR:
        case t_PCT:
        case t_SLASH:
        case t_RPAREN:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        case t_BANG:{
            advanceInput();
            addNodeLabel(t_BANG, returnNode);
            break;
        }
        case t_CARET:{
            advanceInput();
            addNodeLabel(t_CARET, returnNode);
            node_t * node2 = pFactor();
            addNode(node2, returnNode);
            break;
        }
        case t_SEMIC:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        default:
            printf("rachel");
            parse_error();
            break;

    }

    return returnNode;

}

node_t * pIncTail(){

    node_t * returnNode = malloc(sizeof(node_t));
    returnNode->data = NULL;
    returnNode->parent = NULL;
    returnNode->leftChild = NULL;
    returnNode->rightSibling = NULL;
    returnNode->label = s_INCTAIL + t_SEMIC + 1;

    switch(tok.terminal){

        case t_STAR:
        case t_SLASH:
        case t_PCT:{
            node_t *node1 = pTermTail();
            addNode(node1, returnNode);
            break;
        }
        case t_BANG:
        case t_CARET:{
            node_t *node1 = pFactorTail();
            addNode(node1, returnNode);
            break;
        }
        case t_PLUS:
        case t_MINUS:
        case t_RPAREN:
        case t_SEMIC:{
            addNodeLabel(t_EPSILON, returnNode);
            break;
        }
        default:
            printf("jane");
            parse_error();
            break;

    }

    return returnNode;

}

float evaluate(node_t *node, float start){

    float returnValue;

    switch(node->label){

        case t_PLUS:
            returnValue = (float)t_PLUS;
            break;
        case t_MINUS:
            returnValue = (float)t_MINUS;
            break;
        case t_PLUS_UNARY:
            returnValue = (float)t_PLUS_UNARY;
            break;
        case t_MINUS_UNARY:
            returnValue = (float)t_MINUS_UNARY;
            break;
        case t_INCREMENT:
            returnValue = (float)t_INCREMENT;
            break;
        case t_DECREMENT:
            returnValue = (float)t_DECREMENT;
            break;
        case t_STAR:
            returnValue = (float)t_STAR;
            break;
        case t_PCT:
            returnValue = (float)t_PCT;
            break;
        case t_SLASH:
            returnValue = (float)t_SLASH;
            break;
        case t_LPAREN:
            returnValue = (float)t_LPAREN;
            break;
        case t_RPAREN:
            returnValue = (float)t_RPAREN;
            break;
        case t_BANG:
            returnValue = (float)t_BANG;
            break;
        case t_CARET:
            returnValue = (float)t_CARET;
            break;
        case t_LITERAL:
            returnValue = atof(node->data);
            if(returnValue == (float)((int) returnValue))
                isInt = 1;
            else
                isInt = 0;
            break;
        case t_EPSILON:
            returnValue = (float)t_EPSILON;
            break;
        case t_SEMIC:
            returnValue = (float)t_SEMIC;
            break;
        case (s_EXPRESSION + t_SEMIC + 1):{
            if(node->leftChild != NULL && node->leftChild->rightSibling != NULL){
                float f1 = evaluate(node->leftChild, start);
                float f2 = evaluate(node->leftChild->rightSibling, f1);
                returnValue = f1 + f2;
            }
            else
                evaluate_error(node);
            break;
        }
        case (s_EXPTAIL + t_SEMIC + 1):{
            if(node->leftChild != NULL){

                float f1 = evaluate(node->leftChild, start);

                //If it's not a postincrement expression -- i.e.:
                // + <T> <ET> OR - <T> <ET>
                if(node->leftChild->rightSibling != NULL && node->leftChild->rightSibling->rightSibling != NULL){
                        
                    float f2 = evaluate(node->leftChild->rightSibling, start);
                    float f3;

                    if(f1 == (float)t_MINUS){

                        f3 = evaluate(node->leftChild->rightSibling->rightSibling, f1 - f2);
                        returnValue = -1.0*f2 + f3;

                    }
                    else if(f1 == (float)t_PLUS){

                        f3 = evaluate(node->leftChild->rightSibling->rightSibling, f1 + f2);
                        returnValue = f2 + f3;

                    }
                    else
                        evaluate_error(node);

                }
                //otherwise, it IS a postincrement expression...
                else if(node->leftChild->rightSibling != NULL){

                    float f2 = evaluate(node->leftChild->rightSibling, start);

                    returnValue = f1 + f2;

                }
                //or else it's an epsilon or error
                else{
                    if(f1 == (float)t_EPSILON)
                        returnValue = 0;
                    else
                        evaluate_error(node);
                }
             
            }
            else
                evaluate_error(node);
            
            break;
        }
        case (s_POSTINCREMENT + t_SEMIC + 1):{
            if(node->leftChild != NULL && node->leftChild->rightSibling != NULL){

                float f1 = evaluate(node->leftChild, start);
                float f2 = evaluate(node->leftChild->rightSibling, start);

                if(node->parent->parent->rightSibling != NULL && node->parent->parent->rightSibling->label == t_RPAREN && node->leftChild->rightSibling->leftChild != NULL && node->leftChild->rightSibling->leftChild->label == t_EPSILON)
                    returnValue = f1;
                else
                    returnValue = f2;

            }
            else
                evaluate_error(node);
            break;
        }
        case (s_TERM + t_SEMIC + 1):{
            if(node->leftChild != NULL && node->leftChild->rightSibling != NULL){
                float f1 = evaluate(node->leftChild, start);
                float f2 = evaluate(node->leftChild->rightSibling, f1);
                returnValue = f1 + f2;
            }
            else
                evaluate_error(node);
            break;
        }
        case (s_TERMTAIL + t_SEMIC + 1):{
            if(node->leftChild != NULL){

                float f1 = evaluate(node->leftChild, start);

                if(node->leftChild->rightSibling != NULL && node->leftChild->rightSibling->rightSibling != NULL){

                    float f2 = evaluate(node->leftChild->rightSibling, start);
                    float f3;

                    if(f1 == (float)t_STAR){

                        f3 = evaluate(node->leftChild->rightSibling->rightSibling, start*f2);
                        returnValue = start*f2 - start + f3;

                    }
                    else if(f1 == (float)t_SLASH){

                        f3 = evaluate(node->leftChild->rightSibling->rightSibling, start/f2);
                        returnValue = start/f2 - start + f3;

                    }
                    else if(f1 == (float)t_PCT){

                        f3 = evaluate(node->leftChild->rightSibling->rightSibling, (float)(adeebRound(start) % adeebRound(f2)));
                        if(start != 0.0)
                            returnValue = ((float)(adeebRound(start) % adeebRound(f2))) - start + f3;
                        else
                            returnValue = 0.0;

                    }
                    else
                        evaluate_error(node);

                }
                //or else it's an epsilon or error
                else{
                    if(f1 == (float)t_EPSILON)
                        returnValue = 0.0;
                    else
                        evaluate_error(node);
                }
            }
            else
                evaluate_error(node);
            break;
        }
        case (s_FACTOR + t_SEMIC + 1):{
            if(node->leftChild != NULL && node->leftChild->rightSibling != NULL){
                float f1 = evaluate(node->leftChild, start);
                float f2 = evaluate(node->leftChild->rightSibling, f1);
                returnValue = f1 + f2;
            }
            else
                evaluate_error(node);
            break;
        }
        case (s_FACTORHEAD + t_SEMIC + 1):{
            if(node->leftChild != NULL){

                if(node->leftChild->rightSibling != NULL && node->leftChild->rightSibling->rightSibling != NULL){

                    if(node->leftChild->label == s_INCREMENT + t_SEMIC + 1){

                        float f1 = evaluate(node->leftChild, start);
                        float f2 = evaluate(node->leftChild->rightSibling, start);
                        float f3 = evaluate(node->leftChild->rightSibling->rightSibling, start);

                        returnValue = f1 + (f2 * f3);

                    }
                    else if(node->leftChild->label == s_SIGN + t_SEMIC + 1){

                        float f1 = evaluate(node->leftChild, start);
                        float f2 = evaluate(node->leftChild->rightSibling, start);
                        float f3 = evaluate(node->leftChild->rightSibling->rightSibling, start);

                        returnValue = f1*(f2 + f3);

                    }
                    else if(node->leftChild->label == t_LPAREN){

                        if(node->leftChild->rightSibling->label == (s_EXPRESSION + t_SEMIC + 1))
                            returnValue = evaluate(node->leftChild->rightSibling, start);
                        else
                            evaluate_error(node);

                    }
                    else
                        evaluate_error(node);

                }
                else if(node->leftChild->rightSibling == NULL){

                    if(node->leftChild->label == t_LITERAL)
                        returnValue = evaluate(node->leftChild, start);
                    else
                        evaluate_error(node);

                }
                else
                    evaluate_error(node);

            }
            else
                evaluate_error(node);
            break;
        }
        case (s_SIGN + t_SEMIC + 1):{
            if(node->leftChild != NULL){

                if(node->leftChild->label == t_PLUS || node->leftChild->label == t_PLUS_UNARY || node->leftChild->label == t_EPSILON)
                    returnValue = 1.0;
                else if(node->leftChild->label == t_MINUS || node->leftChild->label == t_MINUS_UNARY)
                    returnValue = -1.0;
                else
                    evaluate_error(node);

            }
            else
                evaluate_error(node);

            break;
        }
        case (s_INCREMENT + t_SEMIC + 1):{
            if(node->leftChild != NULL){

                if(node->leftChild->label == t_INCREMENT)
                    returnValue = 1.0;
                else if(node->leftChild->label == t_DECREMENT)
                    returnValue = -1.0;
                else if(node->leftChild->label == t_EPSILON)
                    returnValue = 0.0;
                else
                    evaluate_error(node);

            }
            else
                evaluate_error(node);

            break;
        }
        case (s_FACTORTAIL + t_SEMIC + 1):{
            if(node->leftChild != NULL){

                if(node->leftChild->label == t_BANG)
                    returnValue = factorial(start) - start;
                else if(node->leftChild->label == t_CARET){

                    if(node->leftChild->rightSibling != NULL && node->leftChild->rightSibling->label == (s_FACTOR + t_SEMIC + 1)){

                        float f1 = evaluate(node->leftChild->rightSibling, start);
                        returnValue = exponent(start, f1) - start;

                    }
                    else
                        evaluate_error(node);

                }
                else if(node->leftChild->label == t_EPSILON)
                    returnValue = 0.0;

            }
            else
                evaluate_error(node);
            break;
        }
        case (s_INCTAIL + t_SEMIC + 1):{
            if(node->leftChild != NULL){
                if(node->leftChild->label == t_EPSILON)
                    returnValue = 0;
                else
                    returnValue = evaluate(node->leftChild, start);
            }
            else
                evaluate_error(node);
            break;
        }
        default:
            evaluate_error(node);
            returnValue =  0.0;
            break;

    }

    return returnValue;

}

/********
    Scan source, identify structure, and print appropriately.
 ********/
void parse(){

    //Calls from reader.c
    //sets the address of loc to be the first character
    //of the first line

    set_to_beginning(&loc);
    isInt = 0;

    scan(&loc, &tok);
    while(tok.tc != T_EOF){
        
        //recurse for statement HERE;
        if(tok.tc != T_EOF && tok.tc != T_SPACE && tok.tc != T_NL_SPACE){
            
            node_t * root = pExpression();
            printf("\n");
            //printNode(root, " ", 1, 1);
            float eval = evaluate(root, 1);
            if(isInt){

                if(eval != (float)((int) eval))
                    printf("==%d (or %f)", (int) eval, eval);
                else
                    printf("==%d", (int) eval);

            }
            else
                printf("==%f", eval);
            isInt = 0;

        }
        else
            //advanceInput();
            findNextInput();
        
    }

}

int adeebRound(float target){

    float tenthsPlace = (target*10.0) - (float) (((int)target)*10);

    int returnValue = (int) target;

    if(tenthsPlace >= 5.0 && target > (float)((int)target))
        returnValue++;

    return returnValue;

}

float factorial(float target){

    float result = 0.0;

    if(target == (float)((int) target) && target >= 0.0){

        if(target == 0.0 || target == 1.0)
            result = 1.0;
        else{

            result = target;

            float i;

            for(i = 1.0; i < target; i++)
                result = result * i;

        }

    }
    else if(target > - 1.0){
        //implementation of the Stirling's Approximation
        //to give a "general" factorial.
        //in fact, this "factorial" function is just a domain-changed
        //Euler-Gamma function!
        //The formula was drawn from Wikipedia: https://en.wikipedia.org/wiki/Stirling%27s_approximation#Versions_suitable_for_calculators
        //This simplification was created by Gergo Nemes.

        float z = target + 1.0;
        float pi = 3.1415926535897932385;
        float e = 2.7182818284590452354;

        result = adeebSqrt(2*pi/z)*exponent(1/e*(z + 1/(12*z - 1/(10*z))), z);

    }
    return result;

}

float exponent(float base, float index){
    //Implemented based on code in this stack overflow thread:
    //http://stackoverflow.com/questions/3518973/floating-point-exponentiation-without-power-function
    return _exponent(base, index, 0.000001); 

}

float _exponent(float base, float index, float precision){
    //Implemented based on code in this stack overflow thread:
    //http://stackoverflow.com/questions/3518973/floating-point-exponentiation-without-power-function
    if(index < 0.0)
        return 1.0/_exponent(base, -1.0*index, precision);
    if(index == 0.0)
        return 1.0;
    else if(index >= 10)
        return _exponent(base, index/2.0, precision/2.0) * _exponent(base, index/2.0, precision/2.0);
    else if(index >= 1.0){

        float result = 1.0;

        while(index >= 1.0){

            result = result * base;
            index--;

        }

        return result * _exponent(base, index, precision);

    }
    else if(precision >= 1.0)
        return adeebSqrt(base);

    return adeebSqrt(_exponent(base, index*2.0, precision*2.0));

}

float adeebSqrt(float target){
    //Implement Newton's Method of root finding
    //for a function f(x) = x^2 - a, where a is the target
    //the found root will be the root of the target

    float precision = 0.000001;
    float current;

    if(target > 1.0)
        current = (target - 1.0)*0.5 + 1.0;
    else
        current = target;

    float bit = (current*current - target)/(2*current);
    while(bit > precision){

        current = current - bit;
        bit = (current*current - target)/(2*current);

    }

    return current;

}

void printNode(node_t *node, char * indent, int length, int last){
    //Based on C# Example Code
    //from this stackoverflow thread:
    //http://stackoverflow.com/questions/1649027/how-do-i-print-out-a-tree-structure

    int i;
    char * newIndent = malloc(sizeof(char)*(length + 4));

    printf("%s", indent);

    if(last){

        printf("\\--");
        
        for(i = 0; i < length; i++){
            if(indent[i] == '\0')
                break;
            else
                newIndent[i] = indent[i];
        }
        newIndent[length] = ' ';
        newIndent[length+1] = ' ';
        newIndent[length+2] = ' ';
        newIndent[length+3] = '\0';

        length = length + 3;

    }
    else{

        printf("|--");

        for(i = 0; i < length; i++){
            if(indent[i] == '\0')
                break;
            else
                newIndent[i] = indent[i];
        }
        newIndent[length] = '|';
        newIndent[length+1] = ' ';
        newIndent[length+2] = ' ';
        newIndent[length+3] = '\0';

        length = length + 3;

    }
    printLabel(node);

    if(node->leftChild == NULL)
        return;
    node_t * currNode = node->leftChild;
    while(currNode != NULL){

        if(currNode->rightSibling == NULL){

            printNode(currNode, newIndent, length, 1);
            break;

        }
        else{

            printNode(currNode, newIndent, length, 0);
            currNode = currNode->rightSibling;

        }

    }


}

void printLabel(node_t * node){

    switch(node->label){

        case t_PLUS:
            printf("{+}\n");
            break;
        case t_MINUS:
            printf("{-}\n");
            break;
        case t_PLUS_UNARY:
            printf("{(+)}\n");
            break;
        case t_MINUS_UNARY:
            printf("{(-)}\n");
            break;
        case t_INCREMENT:
            printf("{(++)}\n");
            break;
        case t_DECREMENT:
            printf("{(--)}\n");
            break;
        case t_STAR:
            printf("{*}\n");
            break;
        case t_PCT:
            printf("{%%}\n");
            break;
        case t_SLASH:
            printf("{/}\n");
            break;
        case t_LPAREN:
            printf("{(}\n");
            break;
        case t_RPAREN:
            printf("{)}\n");
            break;
        case t_BANG:
            printf("{!}\n");
            break;
        case t_CARET:
            printf("{^}\n");
            break;
        case t_LITERAL:
            printf("{%s}\n", node->data);
            break;
        case t_EPSILON:
            printf("{e}\n");
            break;
        case t_SEMIC:
            printf("{;}\n");
            break;
        case (s_EXPRESSION + t_SEMIC + 1):
            printf("{<E>}\n");
            break;
        case (s_EXPTAIL + t_SEMIC + 1):
            printf("{<ET>}\n");
            break;
        case (s_POSTINCREMENT + t_SEMIC + 1):
            printf("{<PI>}\n");
            break;
        case (s_TERM + t_SEMIC + 1):
            printf("{<T>}\n");
            break;
        case (s_TERMTAIL + t_SEMIC + 1):
            printf("{<TT>}\n");
            break;
        case (s_FACTOR + t_SEMIC + 1):
            printf("{<F>}\n");
            break;
        case (s_FACTORHEAD + t_SEMIC + 1):
            printf("{<FH>}\n");
            break;
        case (s_SIGN + t_SEMIC + 1):
            printf("{<S>}\n");
            break;
        case (s_INCREMENT + t_SEMIC + 1):
            printf("{<I>}\n");
            break;
        case (s_FACTORTAIL + t_SEMIC + 1):
            printf("{<FT>}\n");
            break;
        case (s_INCTAIL + t_SEMIC + 1):
            printf("{<IT>}\n");
            break;
        default:
            break;

    }

}

void printNodeError(node_t *node, char * indent, int length, int last){
    //Based on C# Example Code
    //from this stackoverflow thread:
    //http://stackoverflow.com/questions/1649027/how-do-i-print-out-a-tree-structure

    int i;
    char * newIndent = malloc(sizeof(char)*(length + 4));

    fprintf(stderr, "%s", indent);

    if(last){

        fprintf(stderr, "\\--");
        
        for(i = 0; i < length; i++){
            if(indent[i] == '\0')
                break;
            else
                newIndent[i] = indent[i];
        }
        newIndent[length] = ' ';
        newIndent[length+1] = ' ';
        newIndent[length+2] = ' ';
        newIndent[length+3] = '\0';

        length = length + 3;

    }
    else{

        fprintf(stderr, "|--");

        for(i = 0; i < length; i++){
            if(indent[i] == '\0')
                break;
            else
                newIndent[i] = indent[i];
        }
        newIndent[length] = '|';
        newIndent[length+1] = ' ';
        newIndent[length+2] = ' ';
        newIndent[length+3] = '\0';

        length = length + 3;

    }
    printLabelError(node);

    node_t * currNode = node->leftChild;
    while(currNode != NULL){

        if(currNode->rightSibling == NULL){

            printNode(currNode, newIndent, length, 1);
            break;

        }
        else{

            printNode(currNode, newIndent, length, 0);
            currNode = currNode->rightSibling;

        }

    }


}

void printLabelError(node_t * node){

    switch(node->label){

        case t_PLUS:
            fprintf(stderr, "{+}\n");
            break;
        case t_MINUS:
            fprintf(stderr, "{-}\n");
            break;
        case t_PLUS_UNARY:
            fprintf(stderr, "{(+)}\n");
            break;
        case t_MINUS_UNARY:
            fprintf(stderr, "{(-)}\n");
            break;
        case t_INCREMENT:
            fprintf(stderr, "{(++)}\n");
            break;
        case t_DECREMENT:
            fprintf(stderr, "{(--)}\n");
            break;
        case t_STAR:
            fprintf(stderr, "{*}\n");
            break;
        case t_PCT:
            fprintf(stderr, "{%%}\n");
            break;
        case t_SLASH:
            fprintf(stderr, "{/}\n");
            break;
        case t_LPAREN:
            fprintf(stderr, "{(}\n");
            break;
        case t_RPAREN:
            fprintf(stderr, "{)}\n");
            break;
        case t_BANG:
            fprintf(stderr, "{!}\n");
            break;
        case t_CARET:
            fprintf(stderr, "{^}\n");
            break;
        case t_LITERAL:
            fprintf(stderr, "{%s}\n", node->data);
            break;
        case t_EPSILON:
            fprintf(stderr, "{e}\n");
            break;
        case t_SEMIC:
            fprintf(stderr, "{;}\n");
            break;
        case (s_EXPRESSION + t_SEMIC + 1):
            fprintf(stderr, "{<E>}\n");
            break;
        case (s_EXPTAIL + t_SEMIC + 1):
            fprintf(stderr, "{<ET>}\n");
            break;
        case (s_POSTINCREMENT + t_SEMIC + 1):
            fprintf(stderr, "{<PI>}\n");
            break;
        case (s_TERM + t_SEMIC + 1):
            fprintf(stderr, "{<T>}\n");
            break;
        case (s_TERMTAIL + t_SEMIC + 1):
            fprintf(stderr, "{<TT>}\n");
            break;
        case (s_FACTOR + t_SEMIC + 1):
            fprintf(stderr, "{<F>}\n");
            break;
        case (s_FACTORHEAD + t_SEMIC + 1):
            fprintf(stderr, "{<FH>}\n");
            break;
        case (s_SIGN + t_SEMIC + 1):
            fprintf(stderr, "{<S>}\n");
            break;
        case (s_INCREMENT + t_SEMIC + 1):
            fprintf(stderr, "{<I>}\n");
            break;
        case (s_FACTORTAIL + t_SEMIC + 1):
            fprintf(stderr, "{<FT>}\n");
            break;
        case (s_INCTAIL + t_SEMIC + 1):
            fprintf(stderr, "{<IT>}\n");
            break;
        default:
            break;

    }

}

void deleteNode(node_t *node){

    //Iterates through all the the branches
    //and then clears the nodes from memory.
    if(node != NULL) {

        printf("CALL: %d ", node->label);
        if(node->leftChild != NULL)
            deleteNode(node->leftChild);
        if(node->rightSibling != NULL)
            deleteNode(node->rightSibling);
        printf("FREE: %d \n", node->label);
        free(node);
        node = NULL;

    }

}