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
void scan(location_t * loc, token_t * tok)
{
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
        /* character literals: */
            got_s_quote,
            got_sq_char,
            ch_esc,
            ch_oct_esc_1,
            ch_oct_esc_2,
            ch_uni_esc_1,
            ch_uni_esc_2,
            ch_uni_esc_3,
            ch_uni_esc_4,
        /* string literals: */
            in_string,
            str_esc,
            str_oct_esc_1,
            str_oct_esc_2,
            str_uni_esc_1,
            str_uni_esc_2,
            str_uni_esc_3,
            str_uni_esc_4,
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
            got_op,
        /* comments: */
            got_slash,
            in_old_comment,
            ending_comment,
            in_new_comment
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

    tok->location = *loc;
    tok->length = 0;

    while (state != done) {
        location_t loc_save = *loc;
        int c = get_character(loc);
        tok->length++;
        switch (state) {
            case start:
                switch (char_classes[c]) {
                    case WHITE:
                        state = got_space;
                        break;
                    case EOLN:
                        state = got_nl_space;
                        break;
                    CASE_LETTER:
                        state = in_identifier;
                        break;
                    case DOT:
                        state = got_dot;
                        break;
                    case DIG_0:
                        state = got_0;
                        break;
                    CASE_NZ_DIGIT:
                        state = got_dec;
                        break;
                    case S_QUOTE:
                        state = got_s_quote;
                        break;
                    case D_QUOTE:
                        state = in_string;
                        break;
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
                    case BANG:
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
                    case SLASH:
                        state = got_slash;
                        break;
                    case END:
                        ACCEPT_REUSE(T_EOF);
                        break;
                    case B_SLASH:
                    case OTHER:
                        /* This will be an error.  Eat as many bogus
                            characters as possible. */
                        state = got_other;
                        break;
                }
                break;
            case in_identifier:
                switch (char_classes[c]) {
                    CASE_LETTER:
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

            /* character literals: */
            case got_s_quote:
                switch (char_classes[c]) {
                    case B_SLASH:
                        state = ch_esc;
                        break;
                    case S_QUOTE:
                        fprintf(stderr, "Invalid character literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);  /* punt */
                        break;
                    case EOLN:
                        fprintf(stderr, "Invalid character literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                    default:
                        state = got_sq_char;
                        break;
                }
                break;
            case got_sq_char:
                switch (char_classes[c]) {
                    case S_QUOTE:
                        ACCEPT(T_LITERAL);        /* character literal */
                        break;
                    default:
                        fprintf(stderr, "Invalid character literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case ch_esc:
                switch (char_classes[c]) {
                    case LET_b:
                    case LET_t:
                    case LET_n:
                    case LET_f:
                    case LET_r:
                    case S_QUOTE:
                    case D_QUOTE:
                    case B_SLASH:
                        state = got_sq_char;
                        break;
                    case DIG_0:
                    case DIG_1to3:
                        state = ch_oct_esc_1;
                        break;
                    case DIG_4to7:
                        state = ch_oct_esc_2;
                        break;
                    case LET_u:
                        state = ch_uni_esc_1;
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        state = got_sq_char;    /* most likely recovery? */
                        break;
                }
                break;
            case ch_oct_esc_1:
                switch (char_classes[c]) {
                    CASE_OCT_DIGIT:
                        state = ch_oct_esc_2;
                        break;
                    case S_QUOTE:
                        ACCEPT(T_LITERAL);        /* character literal */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case ch_oct_esc_2:
                switch (char_classes[c]) {
                    CASE_OCT_DIGIT:
                        state = got_sq_char;
                        break;
                    case S_QUOTE:
                        ACCEPT(T_LITERAL);        /* character literal */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case ch_uni_esc_1:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = ch_uni_esc_2;
                        break;
                    case S_QUOTE:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);        /* most likely recovery? */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case ch_uni_esc_2:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = ch_uni_esc_3;
                        break;
                    case S_QUOTE:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);        /* most likely recovery? */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case ch_uni_esc_3:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = ch_uni_esc_4;
                        break;
                    case S_QUOTE:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);        /* most likely recovery? */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;
            case ch_uni_esc_4:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = got_sq_char;
                        break;
                    case S_QUOTE:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);        /* most likely recovery? */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in character literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                }
                break;

            /* string literals: */
            case in_string:
                switch (char_classes[c]) {
                    case B_SLASH:
                        state = str_esc;
                        break;
                    case D_QUOTE:
                        ACCEPT(T_LITERAL);        /* string literal */
                        break;
                    case EOLN:
                        fprintf(stderr, "End of line in string literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                    default:
                        break;  /* stay put */
                }
                break;
            case str_esc:
                switch (char_classes[c]) {
                    case LET_b:
                    case LET_t:
                    case LET_n:
                    case LET_f:
                    case LET_r:
                    case S_QUOTE:
                    case D_QUOTE:
                    case B_SLASH:
                        state = in_string;
                        break;
                    case DIG_0:
                    case DIG_1to3:
                        state = str_oct_esc_1;
                        break;
                    case DIG_4to7:
                        state = str_oct_esc_2;
                        break;
                    case LET_u:
                        state = str_uni_esc_1;
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = in_string;      /* most likely recovery? */
                        break;
                }
                break;
            case str_oct_esc_1:
                switch (char_classes[c]) {
                    CASE_OCT_DIGIT:
                        state = str_oct_esc_2;
                        break;
                    case D_QUOTE:
                        ACCEPT(T_LITERAL);        /* string literal */
                        break;
                    case B_SLASH:
                        state = str_esc;
                        break;
                    case EOLN:
                        fprintf(stderr, "End of line in string literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                    default:
                        state = in_string;
                        break;
                }
                break;
            case str_oct_esc_2:
                switch (char_classes[c]) {
                    case D_QUOTE:
                        ACCEPT(T_LITERAL);        /* string literal */
                        break;
                    case B_SLASH:
                        state = str_esc;
                        break;
                    case EOLN:
                        fprintf(stderr, "End of line in string literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                    default:    /* including OCT_DIGIT */
                        state = in_string;
                        break;
                }
                break;
            case str_uni_esc_1:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = str_uni_esc_2;
                        break;
                    case D_QUOTE:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);        /* most likely recovery? */
                        break;
                    case B_SLASH:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = str_esc;        /* most likely recovery? */
                        break;
                    case EOLN:
                        fprintf(stderr, "End of line in string literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = in_string;      /* most likely recovery? */
                        break;
                }
                break;
            case str_uni_esc_2:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = str_uni_esc_3;
                        break;
                    case D_QUOTE:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);        /* most likely recovery? */
                        break;
                    case B_SLASH:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = str_esc;        /* most likely recovery? */
                        break;
                    case EOLN:
                        fprintf(stderr, "End of line in string literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = in_string;      /* most likely recovery? */
                        break;
                }
                break;
            case str_uni_esc_3:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = str_uni_esc_4;
                        break;
                    case D_QUOTE:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);        /* most likely recovery? */
                        break;
                    case B_SLASH:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = str_esc;        /* most likely recovery? */
                        break;
                    case EOLN:
                        fprintf(stderr, "End of line in string literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = in_string;      /* most likely recovery? */
                        break;
                }
                break;
            case str_uni_esc_4:
                switch (char_classes[c]) {
                    CASE_HEX_DIGIT:
                        state = in_string;
                        break;
                    case D_QUOTE:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        ACCEPT(T_LITERAL);        /* most likely recovery? */
                        break;
                    case B_SLASH:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = str_esc;        /* most likely recovery? */
                        break;
                    case EOLN:
                        fprintf(stderr, "End of line in string literal");
                        print_location(tok);
                        ACCEPT_REUSE(T_LITERAL);  /* punt */
                        break;
                    default:
                        fprintf(stderr, "Invalid escape in string literal");
                        print_location(tok);
                        state = in_string;      /* most likely recovery? */
                        break;
                }
                break;

            /* comments: */
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
            case in_old_comment:
                switch (char_classes[c]) {
                    case STAR:
                        state = ending_comment;
                        break;
                    case END:
                        fprintf(stderr, "End of file within comment.\n");
                        ACCEPT_REUSE(T_OLD_COMMENT);
                        break;
                    default:
                        break;
                }
                break;
            case ending_comment:
                switch (char_classes[c]) {
                    case END:
                        fprintf(stderr, "End of file within comment.\n");
                        /* fall through */
                    case SLASH:
                        ACCEPT(T_OLD_COMMENT);
                        break;
                    case STAR:
                        break;
                    default:
                        state = in_old_comment;
                        break;
                }
                break;
            case in_new_comment:
                switch (char_classes[c]) {
                    case EOLN:
                        ACCEPT_REUSE(T_NEW_COMMENT);
                        break;
                    default:
                        break;
                }
                break;
        }
    }
}
