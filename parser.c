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

static int indent_level = 0;
#define    INDENT_WIDTH   4

/* forward declarations: */

static void parse_compilation_unit();
static void parse_package_opt();
static void parse_imports();
static void parse_type_definitions();
static void parse_modifiers();
static void parse_brackets_opt();
static void parse_qualified_name();
static void parse_qualified_name_tail();
static void parse_qualified_name_list();
static void parse_more_qualified_names();
static void parse_class_definition();
static void parse_extends_clause();
static void parse_implements_clause();
static void parse_throws_clause();
static void parse_fields();
static void parse_field();
static void parse_field_tail();
static void parse_qn_field_tail();
static void parse_type_field_tail();
static void parse_name_field_tail();
static void parse_routine_tail();
static void parse_routine_body_opt();
static void parse_params_opt();
static void parse_more_params();
static void parse_param();
static void parse_var_decs();
static void parse_var_decs_tail();
static void parse_eq_initializer_opt();
static void parse_more_var_decs();
static void parse_initializer();
static void parse_initializer_list_opt();
static void parse_more_initializers();
static void parse_type();
static void parse_compound_stmt();
static void parse_balanced_stuff();     /* temporary */
static void parse_stuff();;             /* temporary */
static void parse_expr_list();
static void parse_more_exprs();
static void parse_expr_list_opt();
static void parse_expr_opt();
static void parse_expr();
static void parse_atom();

static int at_bol = 1;
     /* We're about to start a new line of output. */
static int comment_nl = 0;
     /* We've just printed a newline because of a comment.
        Don't print *another* newline if the parser asks for one. */

/********
    Print a newline.  Remember we did so, so put_token can indent
    appropriately.  Skip newline if we've already done it in the process
    of printing a comment.
 ********/
static void newline()
{
    if (comment_nl) {
        comment_nl = 0;
    } else {
        putchar('\n');
        at_bol = 1;
    }
}

/********
    Print current token.  Precede with appropriate space.
 ********/
static void put_token()
{
    if (at_bol) {
        int i = (indent_level) * INDENT_WIDTH;
        while (i--) {
            putchar(' ');
        }
    } else {
        if (tok.tc == T_NEW_COMMENT || tok.tc == T_OLD_COMMENT) {
            int i = INDENT_WIDTH;
            while (i--) {
                putchar(' ');
            }
        } else {
            putchar(' ');       /* YOU NEED TO FIX THIS; insert space
                                between tokens only when appropriate */
        }
    }
    at_bol = 0;
    comment_nl = 0;     /* no longer care if comment printed a newline */
}

/********
    Get next token from the scanner.  Filter out white space, comments,
    and newlines.

    Print comments appropriately:
      - If comment is preceded by a newline in input, precede it with
        newline in output.  If an old-style comment is followed by a
        newline, add a newline to the output.  These are the ONLY places
        where the formatter pays attention to white space in the input.
      - if the comment is new style (// to eoln), follow with newline
        (scanner does not include newline in comment).
      - with both of the previous rules, if the parser also decides to
        generate a newline (e.g. for a semicolon), combine them so we
        don't generate an *extra* one.
 ********/
static void get_token()
{
    token_class prev_class = T_SPACE;
    do {
        scan(&loc, &tok);
        if (tok.tc == T_NL_SPACE && prev_class == T_OLD_COMMENT) {
            /* comment was followed by a newline */
            newline();
            comment_nl = 1;     /* remember this newline */
        }
        if (tok.tc == T_OLD_COMMENT || tok.tc == T_NEW_COMMENT) {
            if (prev_class == T_NL_SPACE) {
                /* comment was preceded by a newline */
                newline();
                comment_nl = 1;     /* remember this newline */
            }
            put_token();    /* print comment itself */
            if (tok.tc == T_NEW_COMMENT) {
                /* newline is not part of comment */
                comment_nl = 0;     /* we really do want this one */
                newline();
                comment_nl = 1;     /* remember this newline */
            }
        }
        prev_class = tok.tc;
    } while (tok.tc == T_SPACE       || tok.tc == T_NL_SPACE
          || tok.tc == T_OLD_COMMENT || tok.tc == T_NEW_COMMENT);
}

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
    A token of class tc is expected from the scanner.  Verify and print.
 ********/
static void match(token_class tc)
{
    if (tc == T_ID_DEC && tok.tc == T_IDENTIFIER) {
        tok.tc = T_ID_DEC;
    }
    if (tc != tok.tc) {
        parse_error();
    }
    put_token();
    get_token();
}

/********
    Scan source, identify structure, and print appropriately.
 ********/
void parse()
{
    set_to_beginning(&loc);
    get_token();

    parse_compilation_unit();
}

static void parse_compilation_unit()
{
    parse_package_opt();
    parse_imports();
    parse_type_definitions();
    match(T_EOF);
}

static void parse_package_opt()
{
    switch (tok.tc) {
        case T_PACKAGE:
            match(T_PACKAGE);
            parse_qualified_name();
            match(T_SEMIC);
            newline();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_imports()
{
    switch (tok.tc) {
        case T_IMPORT:
            match(T_IMPORT);
            parse_expr();
            match(T_SEMIC);
            newline();
            parse_imports();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_type_definitions()
{
    switch (tok.tc) {
        case T_SEMIC:
            match(T_SEMIC);
            newline();
            parse_type_definitions();
            break;
        case T_MODIFIER:
        case T_CLASSWORD:
        case T_SYNCHRONIZED:
            parse_modifiers();
            parse_class_definition();
            parse_type_definitions();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_modifiers()
{
    switch (tok.tc) {
        case T_MODIFIER:
            match(T_MODIFIER);
            parse_modifiers();
            break;
        case T_SYNCHRONIZED:
            match(T_SYNCHRONIZED);
            parse_modifiers();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_brackets_opt()
{
    switch (tok.tc) {
        case T_LBRAC:
            match(T_LBRAC);
            match(T_RBRAC);
            parse_brackets_opt();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_qualified_name()
{
    match(T_IDENTIFIER);
    parse_qualified_name_tail();
}

static void parse_qualified_name_tail()
{
    switch (tok.tc) {
        case T_DOT:
            match(T_DOT);
            match(T_IDENTIFIER);
            parse_qualified_name_tail();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_qualified_name_list()
{
    parse_qualified_name();
    parse_more_qualified_names();
}

static void parse_more_qualified_names()
{
    switch (tok.tc) {
        case T_COMMA:
            match(T_COMMA);
            parse_qualified_name_list();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_class_definition()
{
    match(T_CLASSWORD);
    match(T_ID_DEC);        /* NOTE THIS!!
                               Passing T_ID_DEC instead of T_IDENTIFIER */
    parse_extends_clause();
    parse_implements_clause();
    match(T_LBRACE);
    newline();
    parse_fields();
    match(T_RBRACE);
    newline();
}

static void parse_extends_clause()
{
    switch (tok.tc) {
        case T_EXTENDS:
            match(T_EXTENDS);
            parse_qualified_name_list();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_implements_clause()
{
    switch (tok.tc) {
        case T_IMPLEMENTS:
            match(T_IMPLEMENTS);
            parse_qualified_name_list();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_throws_clause()
{
    switch (tok.tc) {
        case T_THROWS:
            match(T_THROWS);
            parse_qualified_name_list();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_fields()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_LBRACE:
        case T_SEMIC:
        case T_MODIFIER:
        case T_TYPE:
        case T_SYNCHRONIZED:
            parse_field();
            parse_fields();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_field()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_LBRACE:
        case T_MODIFIER:
        case T_TYPE:
        case T_SYNCHRONIZED:
            parse_modifiers();
            parse_field_tail();
            break;
        case T_SEMIC:
            match(T_SEMIC);
            newline();
            break;
        default:
            parse_error();
            break;
    }
}

static void parse_field_tail()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
            parse_qualified_name();
                /* PROBLEM!: this name may be either a type name or an
                    identifier declaration, and we won't know until we
                    see what comes next.  How do we format it? */
            parse_qn_field_tail();
            break;
        case T_LBRACE:
            parse_compound_stmt();      /* static initializer */
            break;
        case T_TYPE:
            match(T_TYPE);
            parse_type_field_tail();
            break;
        default:
            parse_error();
            break;
    }
}

static void parse_qn_field_tail()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_LBRAC:
            parse_type_field_tail();
            break;
        case T_LPAREN:
            parse_routine_tail();       /* constructor */
            break;
        default:
            parse_error();
            break;
    }
}

static void parse_type_field_tail()
{
    parse_brackets_opt();
    match(T_IDENTIFIER);
    parse_name_field_tail();
}

static void parse_name_field_tail()
{
    switch (tok.tc) {
        case T_LBRAC:
        case T_EQUALS:
        case T_SEMIC:
        case T_COMMA:
            parse_var_decs_tail();
            match(T_SEMIC);
            newline();
            break;
        case T_LPAREN:
            parse_routine_tail();       /* non-constructor method */
            break;
        default:
            parse_error();
            break;
    }
}

static void parse_routine_tail()
{
    match(T_LPAREN);
    parse_params_opt();
    match(T_RPAREN);
    parse_brackets_opt();
    parse_throws_clause();
    parse_routine_body_opt();
}

static void parse_routine_body_opt()
{
    switch (tok.tc) {
        case T_SEMIC:
            match(T_SEMIC);
            newline();
            break;
        case T_LBRACE:
            parse_compound_stmt();
            break;
        default:
            parse_error();
            break;
    }
}

static void parse_params_opt()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_TYPE:
            parse_param();
            parse_more_params();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_more_params()
{
    switch (tok.tc) {
        case T_COMMA:
            match(T_COMMA);
            parse_param();
            parse_more_params();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_param()
{
    parse_type();
    parse_brackets_opt();
    match(T_IDENTIFIER);
    parse_brackets_opt();
}

static void parse_var_decs()
{
    match(T_IDENTIFIER);
    parse_var_decs_tail();
}

static void parse_var_decs_tail()
{
    parse_brackets_opt();
    parse_eq_initializer_opt();
    parse_more_var_decs();
}

static void parse_eq_initializer_opt()
{
    switch (tok.tc) {
        case T_EQUALS:
            match(T_EQUALS);
            parse_initializer();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_more_var_decs()
{
    switch (tok.tc) {
        case T_COMMA:
            match(T_COMMA);
            parse_var_decs();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_initializer()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_OPERATOR:
        case T_KWOPERATOR:
        case T_LBRAC:
        case T_LPAREN:
        case T_EQUALS:
        case T_QMARK:
        case T_DOT:
        case T_STAR:
        case T_LITERAL:
        case T_TYPE:
        case T_ATOMWORD:
            parse_expr();
            break;
        case T_LBRACE:
            match(T_LBRACE);
            parse_initializer_list_opt();
            match(T_RBRACE);
            break;
        default:
            parse_error();
            break;
    }
}

static void parse_initializer_list_opt()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_OPERATOR:
        case T_KWOPERATOR:
        case T_LBRACE:
        case T_LBRAC:
        case T_LPAREN:
        case T_EQUALS:
        case T_QMARK:
        case T_DOT:
        case T_STAR:
        case T_LITERAL:
        case T_TYPE:
        case T_ATOMWORD:
            parse_initializer();
            parse_more_initializers();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_more_initializers()
{
    switch (tok.tc) {
        case T_COMMA:
            match(T_COMMA);
            parse_initializer();
            parse_more_initializers();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_type()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
            parse_qualified_name();
            break;
        case T_TYPE:
            match(T_TYPE);
            break;
        default:
            parse_error();
            break;
    }
}

/* NB: you will need to modify routine parse_compound_stmt() */
static void parse_compound_stmt()
{
    indent_level++;
    match(T_LBRACE);
    newline();
    parse_balanced_stuff();
    indent_level--;
    match(T_RBRACE);
    newline();
}

static void parse_expr_list()
{
    parse_expr();
    parse_more_exprs();
}

static void parse_more_exprs()
{
    switch (tok.tc) {
        case T_COMMA:
            match(T_COMMA);
            parse_expr_list();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_expr_list_opt()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_OPERATOR:
        case T_KWOPERATOR:
        case T_LBRAC:
        case T_LPAREN :
        case T_EQUALS:
        case T_QMARK:
        case T_DOT:
        case T_STAR:
        case T_LITERAL:
        case T_TYPE:
        case T_ATOMWORD:
            parse_expr_list();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_expr_opt()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_OPERATOR:
        case T_KWOPERATOR:
        case T_LBRAC:
        case T_LPAREN :
        case T_EQUALS:
        case T_QMARK:
        case T_DOT:
        case T_STAR:
        case T_LITERAL:
        case T_TYPE:
        case T_ATOMWORD:
            parse_expr();
            break;
        default:
            break;      /* just return */
    }
}

static void parse_expr()
{
    parse_atom();
    parse_expr_opt();
}

/* NB: you will need to modify routine parse_atom() */
static void parse_atom()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_DOT:
        case T_TYPE:
        case T_LITERAL:
        case T_OPERATOR:
        case T_KWOPERATOR:
        case T_EQUALS:
        case T_STAR:
        case T_ATOMWORD:
            match(tok.tc);
            break;
        case T_LBRAC:
            match(T_LBRAC);
            parse_expr_opt();
            match(T_RBRAC);
            break;
        case T_QMARK:
            match(T_QMARK);
            parse_expr();
            match(T_COLON);
            break;
        case T_LPAREN:
            match(T_LPAREN);
            parse_expr_list_opt();
            match(T_RPAREN);
            break;
        default:
            parse_error();
            break;      /* just return */
    }
}

/* NB: the following two routines accept random junk inside a class
   definition.  You will need to replace them. */

static void parse_balanced_stuff()
{
    switch (tok.tc) {
        case T_LBRACE:
            match(T_LBRACE);
            newline();
            parse_balanced_stuff();
            match(T_RBRACE);
            newline();
            parse_balanced_stuff();
            break;
        case T_RBRACE:
            break;      /* just return */
        default:
            parse_stuff();
            parse_balanced_stuff();
            break;
    }
}

static void parse_stuff()
{
    switch (tok.tc) {
        case T_IDENTIFIER:
        case T_OPERATOR:
        case T_KWOPERATOR:
        case T_LBRAC:
        case T_RBRAC:
        case T_LPAREN:
        case T_RPAREN:
        case T_EQUALS:
        case T_COLON:
        case T_QMARK:
        case T_COMMA:
        case T_DOT:
        case T_STAR:
        case T_MODIFIER:
        case T_LITERAL:
        case T_TYPE:
        case T_ATOMWORD:
        case T_CLASSWORD:
        case T_BREAK:
        case T_CASE:
        case T_CATCH:
        case T_CONTINUE:
        case T_DEFAULT:
        case T_DO:
        case T_ELSE:
        case T_FINALLY:
        case T_FOR:
        case T_GOTO:
        case T_IF:
        case T_RETURN:
        case T_SWITCH:
        case T_SYNCHRONIZED:
        case T_THROW:
        case T_TRY:
        case T_WHILE:
            match(tok.tc);
            break;
        case T_SEMIC:
            match(T_SEMIC);
            newline();
            break;
        default:
            parse_error();
            break;
    }
}
