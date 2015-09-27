/**********************************************************************
    Character classes, to simplify switch statement labels.
 **********************************************************************/

#ifndef CHAR_CLASSES_H
#define CHAR_CLASSES_H

typedef enum {
    END,    /* NUL, returned by reader at end of file */

    LET_ABC,    /* ABCa c */
    LET_b,      /* b      */
    LET_DF,     /* DFd    */
    LET_E,      /* Ee     */
    LET_f,      /* f      */
    LET_L,      /* Ll     */
    LET_n,      /* n      */
    LET_r,      /* r      */
    LET_t,      /* t      */
    LET_u,      /* u      */
    LET_X,      /* Xx     */
    LET_OTHER,  /* GHIJK MNOPQRSTUVW YZ ghijk m opq s vw yz _ $ */

    DIG_0,      /* 0      */
    DIG_1to3,   /* 123    */
    DIG_4to7,   /* 4567   */
    DIG_89,     /* 89     */

    WHITE,      /* space, tab, or form feed */
    EOLN,       /* newline or carriage return */

    BANG,       /* ! */
    D_QUOTE,    /* " */
    PCT,        /* % */
    AMP,        /* & */
    S_QUOTE,    /* " */
    LPAREN,     /* ( */
    RPAREN,     /* ) */
    STAR,       /* * */
    PLUS,       /* + */
    COMMA,      /* , */
    MINUS,      /* - */
    DOT,        /* . */
    SLASH,      /* / */
    COLON,      /* : */
    SEMIC,      /* ; */
    L_THAN,     /* < */
    EQUALS,     /* = */
    G_THAN,     /* > */
    QMARK,      /* ? */
    LBRAC,      /* [ */
    B_SLASH,    /* \ */
    RBRAC,      /* ] */
    CARET,      /* ^ */
    LBRACE,     /* { */
    BAR,        /* | */
    RBRACE,     /* } */
    TILDE,      /* ~ */
    OTHER       /* catchall for everything else:
                   valid only inside comments and strings */
} char_class;
extern const char_class char_classes[128];

/* definitions to make switch statements easier to write: */

#define CASE_LET_DF \
    case LET_DF:    \
    case LET_f

#define CASE_LETTER \
    case LET_ABC:   \
    case LET_b:     \
    CASE_LET_DF:    \
    case LET_E:     \
    case LET_L:     \
    case LET_n:     \
    case LET_r:     \
    case LET_t:     \
    case LET_u:     \
    case LET_X:     \
    case LET_OTHER

#define CASE_NZ_DIGIT   \
    case DIG_1to3:      \
    case DIG_4to7:      \
    case DIG_89

#define CASE_OCT_DIGIT  \
    case DIG_0:         \
    case DIG_1to3:      \
    case DIG_4to7

#define CASE_DEC_DIGIT  \
    CASE_OCT_DIGIT:     \
    case DIG_89

#define CASE_HEX_DIGIT  \
    CASE_DEC_DIGIT:     \
    case LET_ABC:       \
    case LET_b:         \
    CASE_LET_DF:        \
    case LET_E

#define CASE_SIGN       \
    case PLUS:          \
    case MINUS

#endif
