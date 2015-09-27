/**********************************************************************
    Maintain input as a linked list of lines.
    Return characters on demand.
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reader.h"

#define MAX_LINE_LEN 512

static line_t head = {0, 0, 0};

/********
    Read source program into a linked list of line_t structures,
    with (empty) head node "head".
 ********/
void initialize_reader(FILE *fp)
{
    char s[MAX_LINE_LEN];
    line_t * tail = &head;
    while (1) {
        line_t * old_tail;
        if (!fgets(s, MAX_LINE_LEN, fp)) break;
        old_tail = tail;
        tail = (line_t *) malloc(sizeof(line_t));
        tail->data = strdup(s);
        tail->line_num = old_tail->line_num + 1;
        tail->length = strlen(s);
        if (tail->length == MAX_LINE_LEN-1 && tail->data[MAX_LINE_LEN]) {
            fprintf(stderr, "input line %d too long (%d max)\n",
                tail->line_num, MAX_LINE_LEN);
            exit(-1);
        }
        tail->next = 0;
        old_tail->next = tail;
    }
}

void set_to_beginning(location_t *loc)
{
    loc->line = head.next;
    loc->column = 0;
}

int get_character(location_t *loc)
{
    int rtn;
    if (loc->column >= loc->line->length) {
        return 0;
    }
    rtn = loc->line->data[loc->column++];
    if (loc->column >= loc->line->length && loc->line->next) {
        loc->line = loc->line->next;
        loc->column = 0;
    }
    return rtn;
}

void finalize_reader()
{
    line_t * l = head.next;

    while (l) {
        line_t * t = l;
        if (l->data)
            free(l->data);
        l = l->next;
        free(t);
    }
}
