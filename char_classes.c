/**********************************************************************
    Character classes, to simplify switch statement labels.

    NB: These classes do NOT suffice for the assignment;
        you will need to modify this table.

 **********************************************************************/

#include "char_classes.h"

const char_class char_classes[128] = {
  END,        /* nul   ^@ */      /* returned by get_character at EOF */
  OTHER,      /* soh   ^A */
  OTHER,      /* stx   ^B */
  OTHER,      /* etx   ^C */
  OTHER,      /* eot   ^D */
  OTHER,      /* enq   ^E */
  OTHER,      /* ack   ^F */
  OTHER,      /* bel   ^G */
  OTHER,      /* bs    ^H */
  WHITE,      /* ht    ^I */
  EOLN,       /* nl    ^J */    /* 10 */
  OTHER,      /* vt    ^K */
  WHITE,      /* ff    ^L */
  EOLN,       /* cr    ^M */
  OTHER,      /* so    ^N */
  OTHER,      /* si    ^O */
  OTHER,      /* dle   ^P */
  OTHER,      /* dc1   ^Q */
  OTHER,      /* dc2   ^R */
  OTHER,      /* dc3   ^S */
  OTHER,      /* dc4   ^T */    /* 20 */
  OTHER,      /* nak   ^U */
  OTHER,      /* syn   ^V */
  OTHER,      /* etb   ^W */
  OTHER,      /* can   ^X */
  OTHER,      /* em    ^Y */
  OTHER,      /* sub   ^Z */
  OTHER,      /* esc   ^[ */
  OTHER,      /* fs    ^\ */
  OTHER,      /* gs    ^] */
  OTHER,      /* rs    ^^ */    /* 30 */
  OTHER,      /* us    ^_ */
  WHITE,      /* sp */
  BANG,       /* ! */
  D_QUOTE,    /* " */
  OTHER,      /* # */
  LET_OTHER,  /* $ */
  PCT,        /* % */
  AMP,        /* & */
  S_QUOTE,    /* ' */
  LPAREN,     /* ( */
  RPAREN,     /* ) */
  STAR,       /* * */
  PLUS,       /* + */
  COMMA,      /* , */
  MINUS,      /* - */
  DOT,        /* . */
  SLASH,      /* / */
  DIG_0,      /* 0 */
  DIG_1to3,   /* 1 */
  DIG_1to3,   /* 2 */
  DIG_1to3,   /* 3 */
  DIG_4to7,   /* 4 */
  DIG_4to7,   /* 5 */
  DIG_4to7,   /* 6 */
  DIG_4to7,   /* 7 */
  DIG_89,     /* 8 */
  DIG_89,     /* 9 */
  COLON,      /* : */
  SEMIC,      /* ; */
  L_THAN,     /* < */
  EQUALS,     /* = */
  G_THAN,     /* > */
  QMARK,      /* ? */
  OTHER,      /* @ */
  LET_ABC,    /* A */
  LET_ABC,    /* B */
  LET_ABC,    /* C */
  LET_DF,     /* D */
  LET_E,      /* E */
  LET_DF,     /* F */
  LET_OTHER,  /* G */
  LET_OTHER,  /* H */
  LET_OTHER,  /* I */
  LET_OTHER,  /* J */
  LET_OTHER,  /* K */
  LET_L,      /* L */
  LET_OTHER,  /* M */
  LET_OTHER,  /* N */
  LET_OTHER,  /* O */
  LET_OTHER,  /* P */
  LET_OTHER,  /* Q */
  LET_OTHER,  /* R */
  LET_OTHER,  /* S */
  LET_OTHER,  /* T */
  LET_OTHER,  /* U */
  LET_OTHER,  /* V */
  LET_OTHER,  /* W */
  LET_X,      /* X */
  LET_OTHER,  /* Y */
  LET_OTHER,  /* Z */
  LBRAC,      /* [ */
  B_SLASH,    /* \ */
  RBRAC,      /* ] */
  CARET,      /* ^ */
  LET_OTHER,  /* _ */
  OTHER,      /* ` */
  LET_ABC,    /* a */
  LET_b,      /* b */   /* lc only, for escape sequences */
  LET_ABC,    /* c */
  LET_DF,     /* d */
  LET_E,      /* e */
  LET_f,      /* f */   /* lc only, for escape sequences */
  LET_OTHER,  /* g */
  LET_OTHER,  /* h */
  LET_OTHER,  /* i */
  LET_OTHER,  /* j */
  LET_OTHER,  /* k */
  LET_L,      /* l */
  LET_OTHER,  /* m */
  LET_n,      /* n */   /* lc only, for escape sequences */
  LET_OTHER,  /* o */
  LET_OTHER,  /* p */
  LET_OTHER,  /* q */
  LET_r,      /* r */   /* lc only, for escape sequences */
  LET_OTHER,  /* s */
  LET_t,      /* t */   /* lc only, for escape sequences */
  LET_u,      /* u */   /* lc only, for escape sequences */
  LET_OTHER,  /* v */
  LET_OTHER,  /* w */
  LET_X,      /* x */
  LET_OTHER,  /* y */
  LET_OTHER,  /* z */
  LBRACE,     /* { */
  BAR,        /* | */
  RBRACE,     /* } */
  TILDE,      /* ~ */
  OTHER       /* del   ^? */
};
