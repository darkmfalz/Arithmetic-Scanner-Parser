/**********************************************************************
    Tokenizer for Java source.

    Allows unicode escapes only within strings and comments.  Otherwise,
    accepts all and only those tokens defined by the Java language
    specification.  (Note that this is significantly more than you were
    required to do for assignment 3; this solution would have received a
    lot of extra credit.)

    Tokens are classified as listed in scanner.h.

 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include "char_classes.h"
#include "parser.h"
#include "scanner.h"

static void print_location (token_t *tok)
{
    fprintf(stderr, " at line %d, column %d\n",
        tok->location.line->line_num, tok->location.column);
}

/********
    Modify tok to describe next token of input.
    Update loc to refer to location immediately after tok.
 ********/
//This method reads in a new token -- it starts at the
//current location, then adds new characters so long as
//the token is unfinished.

//Some thoughts: we don't need all these character classes
//Some other thoughts: Understand how scan works in the
//context of the entire program.
void scan(location_t * loc, token_t * tok)
{
    //an enumeration of the different possible states
    //also initializes "state" to "start," which is 0
    enum {
            start,
            got_space,
            got_nl_space,
            got_other,
            done,
        /* numbers: */
            got_dot,
            got_dec,
            got_fp_dot,
        /* operators: */
            got_plus,
            got_minus,
            got_star,
            got_slash,
            got_pct,
            got_lparen,
            got_incr,
            got_decr,
            got_incr2,
            got_decr2,
            got_caret,
            got_bang
    } state = start;

/* Standard way to recognize a token: put back lookahead character that
    isn't part of current token: */
#define ACCEPT_REUSE(t) \
    *loc = loc_save;    \
    tok->length--;      \
    tok->tc = t;        \
    state = done;

#define ACCEPT_SHIFT(t, i)          \
    move_location_back(loc, i);    \
    tok->length = tok->length - i;  \
    tok->tc = t;                    \
    state = done;

/* Shortcut to eliminate final states with no out transitions: go
    ahead and accept token in previous state, but don't put back the
    lookahead: it's actually part of the token: */
#define ACCEPT(t) \
    tok->tc = t;  \
    state = done;

//NOTE: the following code is NOT part of the ACCEPT(t)
    tok->location = *loc;
    tok->length = 0;

    while (state != done) {
        //Points to loc -- but if we update loc, we update loc_save
        location_t loc_save = *loc;
        //Gets the character, but also updates loc by incrementing it
        int c = get_character(loc);
        //I guess... we might be adding this character to the token?
        tok->length++;
        //WAIT A SECOND -- THIS WHOLE SWITCH-CASE IS A DFA. :O
        switch (state) {
            //If we're in the start state...
            case start:
                //... find out which character class the character is in
                switch (char_classes[c]) {
                    case WHITE:
                        state = got_space;
                        break;
                    case EOLN:
                        state = got_nl_space;
                        break;
                    case DOT:
                        state = got_dot;
                        break;
                    case DIG:
                        state = got_dec;
                        break;
                    case PLUS:
                        state = got_plus;
                        break;
                    case MINUS:
                        state = got_minus;
                        break;
                    case STAR:
                        state = got_star;
                        break;
                    case PCT:
                        state = got_pct;
                        break;
                    case SLASH:
                        state = got_slash;
                        break;
                    case CARET:
                        state = got_caret;
                        break;
                    case BANG:
                        state = got_bang;
                        break;
                    case LPAREN:
                        state = got_lparen;
                        break;
                    case RPAREN:
                        tok->terminal = t_RPAREN;
                        ACCEPT(T_RPAREN);
                        break;
                    case SEMIC:
                        tok->terminal = t_SEMIC;
                        ACCEPT(T_SEMIC);
                        break;
                    case END:
                        ACCEPT_REUSE(T_EOF);
                        break;
                    case OTHER:
                        /* This will be an error.  Eat as many bogus
                            characters as possible. */
                        state = got_other;
                        break;
                    default:
                        state = got_other;
                        break;
                }
                break;
            case got_space:
                switch (char_classes[c]) {
                    case WHITE:
                        break;  /* stay put */
                    case EOLN:
                        state = got_nl_space;
                        break;
                    default:
                        ACCEPT_REUSE(T_SPACE);
                        break;
                }
                break;
            case got_nl_space:
                switch (char_classes[c]) {
                    case WHITE:
                    case EOLN:
                        break;  /* stay put */
                    default:
                        ACCEPT_REUSE(T_NL_SPACE);
                        break;
                }
                break;
            case got_other:
                switch (char_classes[c]) {
                    case OTHER:
                    case WHITE:
                    case EOLN:
                        break;  /* stay put */
                    default:
                        fprintf(stderr, "Invalid token");
                        print_location(tok);
                        ACCEPT_REUSE(T_SPACE);    /* most likely recovery? */
                        break;
                }
                break;
            //This, theoretically, should be unreachable -- state would have to change
            //outside of a switch statement, which, by current design is, dare I say,
            //impossible. But, this being a computer program prone to bugs, let's just
            //say "improbable."
            case done:
                fprintf(stderr, "scan: unexpected done in switch\n");
                exit(-1);
                break;

            /* operators: */
            //What about positive and negative? We want the scanner
            //to parse the positive and negative signs for me
            case got_plus:
                tok->terminal = t_PLUS;
                ACCEPT_REUSE(T_OPERATOR);       //  +
                break;
            case got_minus:
                tok->terminal = t_MINUS;
                ACCEPT_REUSE(T_OPERATOR);       //  -
                break;
            case got_star:
                tok->terminal = t_STAR;
                ACCEPT_REUSE(T_OPERATOR);       //  * 
                break;
            case got_slash:
                tok->terminal = t_SLASH;
                ACCEPT_REUSE(T_OPERATOR);       //  /
                break;
            case got_pct:
                tok->terminal = t_PCT;
                ACCEPT_REUSE(T_OPERATOR);       //  %
                break;
            case got_caret:
                tok->terminal = t_CARET;
                ACCEPT_REUSE(T_OPERATOR);
                break;
            case got_bang:
                tok->terminal = t_BANG;
                ACCEPT_REUSE(T_OPERATOR);
                break;
            case got_lparen:
                switch(char_classes[c]){
                    case PLUS:
                        state = got_incr;
                        break;
                    case MINUS:
                        state = got_decr;
                        break;
                    default:
                        tok->terminal = t_LPAREN;
                        ACCEPT_REUSE(T_LPAREN);
                        break;
                }
                break;
            case got_incr:
                switch(char_classes[c]){
                    case PLUS:
                        state = got_incr2;
                        break;
                    case RPAREN:
                        tok->terminal = t_PLUS_UNARY;
                        ACCEPT(T_UNARY);
                        break;
                    default:
                        tok->terminal = t_LPAREN;
                        ACCEPT_SHIFT(T_LPAREN, 2);
                        break;
                }
                break;
            case got_decr:
                switch(char_classes[c]){
                    case MINUS:
                        state = got_decr2;
                        break;
                    case RPAREN:
                        tok->terminal = t_MINUS_UNARY;
                        ACCEPT(T_UNARY);
                        break;
                    default:
                        tok->terminal = t_LPAREN;
                        ACCEPT_SHIFT(T_LPAREN, 2);
                        break;
                }
                break;
            case got_incr2:
                switch(char_classes[c]){
                    case RPAREN:
                        tok->terminal = t_INCREMENT;
                        ACCEPT(T_INCREMENT);
                        break;
                    default:
                        tok->terminal = t_LPAREN;
                        ACCEPT_SHIFT(T_LPAREN, 3);
                        break;
                }
                break;
            case got_decr2:
                switch(char_classes[c]){
                    case RPAREN:
                        tok->terminal = t_DECREMENT;
                        ACCEPT(T_INCREMENT);
                        break;
                    default:
                        tok->terminal = t_LPAREN;
                        ACCEPT_SHIFT(T_LPAREN, 3);
                        break;
                }
                break;
            /* numeric literals: */
            //We assume that a dot means that it's a decimal
            case got_dot:
                state = got_fp_dot;
                break;
            case got_dec:
                switch (char_classes[c]) {
                    case DIG:
                        break;  /* stay put */
                    case DOT:
                        state = got_fp_dot;
                        break;
                    /*case LET_E:
                        state = starting_exp;
                        break;*/
                    default:
                        tok->terminal = t_LITERAL;
                        ACCEPT_REUSE(T_LITERAL);  /* decimal integer */
                        break;
                }
                break;
            case got_fp_dot:
                switch (char_classes[c]) {
                    case DIG:
                        break;  /* stay put */
                    default:
                        tok->terminal = t_LITERAL;
                        ACCEPT_REUSE(T_LITERAL);  /* fp */
                        break;
                }
                break;
        }
    }
}
