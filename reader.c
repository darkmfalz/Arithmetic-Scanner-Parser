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
        tail->prev = old_tail;
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
    //If you pass in a location -- any arbitrary location
    //this method will set the location to point to the first
    //line in the LinkedList of lines, and the first column
    //in that line
    //Location, after all, is the current location being read
    //from the LinkedList
    loc->line = head.next;
    loc->column = 0;
}

int get_character(location_t *loc)
{
    int rtn;
    //don't return an actual character if the location points 
    //to a place outside of the line
    if (loc->column >= loc->line->length) {
        return 0;
    }
    //return is the character at the location, if the location
    //is a valid position inside the line.
    //this statement also increments the column in the location.
    rtn = loc->line->data[loc->column++];
    //If the current location is at the end of the line, then
    //the location is moved to the start of the next line.
    if (loc->column >= loc->line->length && loc->line->next) {
        loc->line = loc->line->next;
        loc->column = 0;
    }
    return rtn;
}

void move_location_back(location_t *loc, int count){

    int i;

    for(i = 0; i < count; i++){

        if(loc->column == 0){

            loc->line = loc->line->prev;
            loc->column = loc->line->length - 1;

        }
        else{

            loc->column--;

        }

    }

}

void finalize_reader()
{
    //Create a pointer to the first line
    line_t * l = head.next;

    //Iterates through all the lines in the LinkedList
    //and then clears the lines from memory.
    while (l) {
        line_t * t = l;
        if (l->data)
            free(l->data);
        l = l->next;
        free(t);
    }
}
