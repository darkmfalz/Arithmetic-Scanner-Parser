/**********************************************************************
    Maintain input as a linked list of lines.
    Return characters on demand.
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reader.h"

#define MAX_LINE_LEN 512

//the head of the linkelist of line_t structs
static line_t head = {0, 0, 0};

/********
    Read source program into a linked list of line_t structures,
    with (empty) head node "head".
 ********/
void initialize_reader(FILE *fp)
{
    //It would appear that this string stores
    //the current line
    char s[MAX_LINE_LEN];
    //tail is, apparently, the current node --
    //so we start at the head, as our tail,
    //then keep adding things on, maybe
    line_t * tail = &head;
    while (1) {
        //this is to store the old tail when we assign the new one
        line_t * old_tail;
        //I don't understand the condition, but this breaks
        //Maybe this detects the end of line?
        if (!fgets(s, MAX_LINE_LEN, fp)) break;

        //Now we assign tail to old_tail...
        old_tail = tail;
        //... And then create the new tail
        tail = (line_t *) malloc(sizeof(line_t));
        //This just copies the string from s into tail data
        tail->data = strdup(s);
        //Increments the line number of the tail
        tail->line_num = old_tail->line_num + 1;
        tail->length = strlen(s);
        //Throw an error if the line is too long to read
        if (tail->length == MAX_LINE_LEN-1 && tail->data[MAX_LINE_LEN]) {
            fprintf(stderr, "input line %d too long (%d max)\n",
                tail->line_num, MAX_LINE_LEN);
            exit(-1);
        }
        //Attach the tail to the LinkedList
        tail->next = 0;
        old_tail->next = tail;
    }

    //In summary, this method creates a linkedlist of lines
    //in the file that was read -- but it doesn't return it.
    //It just initializes it from the file.
    //I think, based on the way the linkelist fields are
    //passed only through the methods, this would be an
    //analog to "private" fields in Java.

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
