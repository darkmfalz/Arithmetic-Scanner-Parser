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
            in_identifier,
            got_space,
            got_nl_space,
            got_other,
            done,
        /* numbers: */
            got_dot,
            got_0,
            got_0_89,
            got_oct,
            got_dec,
            starting_hex,
            got_hex,
            got_fp_dot,
            starting_exp,
            got_exp_sign,
            got_fp_exp,
        /* operators: */
            got_plus,
            got_minus,
            got_amp,
            got_bar,
            got_equals,
            got_l_than,
            got_l_than_2,
            got_g_than,
            got_g_than_2,
            got_g_than_3,
            got_star,
            got_slash,
            got_op
    } state = start;

/* Standard way to recognize a token: put back lookahead character that
    isn't part of current token: */
#define ACCEPT_REUSE(t) \
    *loc = loc_save;    \
    tok->length--;      \
    tok->tc = t;        \
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
        //Gets the character, but also updates loc
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
                    //?
                    /*CASE_LETTER:
                        state = in_identifier;
                        break;*/
                    case DOT:
                        state = got_dot;
                        break;
                    case DIG_0:
                        state = got_0;
                        break;
                    //?
                    /*CASE_NZ_DIGIT:
                        state = got_dec;
                        break;*/
                    case PLUS:
                        state = got_plus;
                        break;
                    case MINUS:
                        state = got_minus;
                        break;
                    case AMP:
                        state = got_amp;
                        break;
                    case BAR:
                        state = got_bar;
                        break;
                    case EQUALS:
                        state = got_equals;
                        break;
                    case L_THAN:
                        state = got_l_than;
                        break;
                    case G_THAN:
                        state = got_g_than;
                        break;
                    case STAR:
                        state = got_star;
                        break;
                    //?
                    case BANG:
                    //?
                    case PCT:
                    case CARET:
                        state = got_op;
                        break;
                    case TILDE:
                        ACCEPT(T_OPERATOR);
                        break;
                    case LPAREN:
                        ACCEPT(T_LPAREN);
                        break;
                    case RPAREN:
                        ACCEPT(T_RPAREN);
                        break;
                    case LBRAC:
                        ACCEPT(T_LBRAC);
                        break;
                    case COMMA:
                        ACCEPT(T_COMMA);
                        break;
                    case SEMIC:
                        ACCEPT(T_SEMIC);
                        break;
                    case QMARK:
                        ACCEPT(T_QMARK);
                        break;
                    case COLON:
                        ACCEPT(T_COLON);
                        break;
                    case RBRAC:
                        ACCEPT(T_RBRAC);
                        break;
                    case LBRACE:
                        ACCEPT(T_LBRACE);
                        break;
                    case RBRACE:
                        ACCEPT(T_RBRACE);
                        break;
                    //Edit to take division, instead of just "slash"
                    case SLASH:
                        state = got_slash;
                        break;
                    case END:
                        ACCEPT_REUSE(T_EOF);
                        break;
                    //?
                    case B_SLASH:
                    case OTHER:
                        /* This will be an error.  Eat as many bogus
                            characters as possible. */
                        state = got_other;
                        break;
                }
                break;
            //Let's decipher this!
            //CASE_LETTER brings us here.
            //Perhaps this is designed to identify the letter?
            case in_identifier:
                switch (char_classes[c]) {
                    //?
                    CASE_LETTER:
                    //?
                    CASE_DEC_DIGIT:
                        break;
                    default:
                        ACCEPT_REUSE(T_IDENTIFIER);
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
            case got_plus:
                switch (char_classes[c]) {
                    case PLUS:
                        ACCEPT(T_OPERATOR);       /* ++ */
                        break;
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* += */
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* + */
                        break;
                }
                break;
            case got_minus:
                switch (char_classes[c]) {
                    case MINUS:
                        ACCEPT(T_OPERATOR);       /* -- */
                        break;
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* -= */
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* - */
                        break;
                }
                break;
            case got_amp:
                switch (char_classes[c]) {
                    case AMP:
                        ACCEPT(T_OPERATOR);       /* && */
                        break;
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* &= */
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* & */
                        break;
                }
                break;
            case got_bar:
                switch (char_classes[c]) {
                    case BAR:
                        ACCEPT(T_OPERATOR);       /* || */
                        break;
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* |= */
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* | */
                        break;
                }
                break;
            case got_equals:
                switch (char_classes[c]) {
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* == */
                        break;
                    default:
                        ACCEPT_REUSE(T_EQUALS);   /* = */
                        break;
                }
                break;
            case got_l_than:
                switch (char_classes[c]) {
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* <= */
                        break;
                    case L_THAN:
                        state = got_l_than_2;
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* < */
                        break;
                }
                break;
            case got_l_than_2:
                switch (char_classes[c]) {
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* <<= */
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* << */
                        break;
                }
                break;
            case got_g_than:
                switch (char_classes[c]) {
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* >= */
                        break;
                    case G_THAN:
                        state = got_g_than_2;
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* > */
                        break;
                }
                break;
            case got_g_than_2:
                switch (char_classes[c]) {
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* >>= */
                        break;
                    case G_THAN:
                        state = got_g_than_3;
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* >> */
                        break;
                }
                break;
            case got_g_than_3:
                switch (char_classes[c]) {
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* >>>= */
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* >>> */
                        break;
                }
                break;
            case got_star:
                switch (char_classes[c]) {
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* *= */
                        break;
                    default:
                        ACCEPT_REUSE(T_STAR);     /* bare op */
                        break;
                }
                break;

            case got_op:
                switch (char_classes[c]) {
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* op= */
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* bare op */
                        break;
                }
                break;

            /* numeric literals: */
            case got_dot:
                switch (char_classes[c]) {
                    CASE_DEC_DIGIT:
                        state = got_fp_dot;
                        break;
                    default:
                        ACCEPT_REUSE(T_DOT);      /* dot */
                        break;
                }
                break;
            case got_0:
                switch (char_classes[c]) {
                    case LET_L:
                        ACCEPT(T_LITERAL);        /* 0L */
                        break;
                    case LET_E:
                        state = starting_exp;
                        break;
                    CASE_LET_DF:
                        ACCEPT(T_LITERAL);        /* 0D or 0F */
                        break;
                    case LET_X:
                        state = starting_hex;
                        break;
                    CASE_OCT_DIGIT:
                        state = got_oct;
                        break;
                    case DIG_89:
                        state = got_0_89;
                        break;
                    case DOT:
                        state = got_fp_dot;
                        break;
                    default:
                        ACCEPT_REUSE(T_LITERAL);  /* 0 */
                        break;
                }
                break;
            case got_0_89:
                switch (char_classes[c]) {
                    CASE_DEC_DIGIT:
                        break;  /* stay put */
                    case DOT:
                        state = got_fp_dot;
                        break;
                    case LET_E:
                        state = starting_exp;
                        break;
                    CASE_LET_DF:
                        ACCEPT(T_LITERAL);        /* FP w/ suffix */
                        break;
                    default:
                        fprintf(stderr, "Invalid decimal literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case got_oct:
                switch (char_classes[c]) {
                    CASE_OCT_DIGIT:
                        break;  /* stay put */
                    case LET_L:
                    CASE_LET_DF:
                        ACCEPT(T_LITERAL);        /* int or FP w/ suffix */
                        break;
                    case LET_E:
                        state = starting_exp;
                        break;
                    case DOT:
                        state = got_fp_dot;
                        break;
                    default:
                        ACCEPT_REUSE(T_LITERAL);  /* octal integer */
                        break;
                }
                break;
            case got_dec:
                switch (char_classes[c]) {
                    CASE_DEC_DIGIT:
                        break;  /* stay put */
                    case DOT:
                        state = got_fp_dot;
                        break;
                    case LET_E:
                        state = starting_exp;
                        break;
                    default:
                        ACCEPT_REUSE(T_LITERAL);  /* decimal integer */
                        break;
                }
                break;
            case starting_hex:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = got_hex;
                        break;
                    default:
                        fprintf(stderr, "Invalid hexadecimal literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case got_hex:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        break;  /* stay put */
                    case LET_L:
                        ACCEPT(T_LITERAL);        /* hex w/ suffix */
                        break;
                    default:
                        ACCEPT_REUSE(T_LITERAL);
                        break;                  /* hexadecimal integer */
                }
                break;
            case got_fp_dot:
                switch (char_classes[c]) {
                    CASE_DEC_DIGIT:
                        break;  /* stay put */
                    CASE_LET_DF:
                        ACCEPT(T_LITERAL);        /* fp w/ suffix */
                        break;
                    case LET_E:
                        state = starting_exp;
                        break;
                    default:
                        ACCEPT_REUSE(T_LITERAL);  /* fp */
                        break;
                }
                break;
            case starting_exp:
                switch (char_classes[c]) {
                    CASE_DEC_DIGIT:
                        state = got_fp_exp;
                        break;
                    CASE_SIGN:
                        state = got_exp_sign;
                        break;
                    default:
                        fprintf(stderr, "Invalid floating point literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case got_exp_sign:
                switch (char_classes[c]) {
                    CASE_DEC_DIGIT:
                        state = got_fp_exp;
                        break;
                    default:
                        fprintf(stderr, "Invalid floating point literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case got_fp_exp:
                switch (char_classes[c]) {
                    CASE_DEC_DIGIT:
                        break;  /* stay put */
                    CASE_LET_DF:
                        ACCEPT(T_LITERAL);        /* fp w/ suffix */
                        break;
                    default:
                        ACCEPT_REUSE(T_LITERAL);  /* fp */
                        break;
                }
                break;

            /* SLASH -- EDIT TO DO DIVISION, LOOK AT OPERATORS: */
            case got_slash:
                switch (char_classes[c]) {
                    case STAR:
                        state = in_old_comment;
                        break;
                    case SLASH:
                        state = in_new_comment;
                        break;
                    case EQUALS:
                        ACCEPT(T_OPERATOR);       /* /= */
                        break;
                    default:
                        ACCEPT_REUSE(T_OPERATOR); /* / */
                        break;
                }
                break;
        }
    }
}
