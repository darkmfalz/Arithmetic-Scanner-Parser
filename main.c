#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include "scanner.h"
#include "parser.h"

int main (int argc, char *argv[]) {
    FILE *fp;
    
    //presumably some error has occurred
    //I think exit(1) exits the open file
    //and returns an error code?
    if (argc < 2) {
        printf("Usage: ./calculator infile.txt\n");
        exit(1);
    }
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error: infile.txt does not exist.\n");
        exit(1);
    }
  
    //this is in reader.c
    initialize_reader(fp);
    
    // Do scan, parse, evaluate here 
    parse();
    
    finalize_reader();
    
    exit(0);
}
