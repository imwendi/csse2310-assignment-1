//
// Created by wendi on 4/03/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "readDict.h"
#include "wordList.h"

/*
 * "Read" in "ReadLine" is past tense :)
 * ReadLine stores lines read from a file with the read_line function.
 * Stores the line read and a bool value representing if the line is the last 
 * line of the file.
 */

typedef struct {
    char *line;
    bool isLastLine;
} ReadLine;

/*
 * Reads a line from a file to a ReadLine
 */
static ReadLine *read_line(FILE *dict) {

    int index = 0;
    char *line = (char *) calloc(1, sizeof(char));
    int nextChar = fgetc(dict);
    bool isLastLine = false;

    while ((char) nextChar != '\n') {
        // Place the last read character into the output string
        line = (char *) realloc(line, (index + 2) * sizeof(char));
        line[index] = (char) nextChar;
     
        // Read the next character
        nextChar = fgetc(dict);
        // Break out of loop if EOF is found
        if (nextChar == EOF) {
            isLastLine = true;
            break;
        }

        index++;
    }

    // Add end of string character '\0'
    line[index] = '\0';

    // Create and output the ReadLine struct
    ReadLine *readLine = malloc(sizeof(ReadLine));
    readLine->line = line;
    readLine->isLastLine = isLastLine;

    return readLine;
}

/*
 * Frees memory allocated to a ReadLine at a given pointer
 */
static void free_readline(ReadLine *word) {
    free(word->line);
    free(word);
}

/*
 * Reads a dictionary file and outputs a WordList containing all the words
 * in the dictionary.
 */
WordList *file_to_wordlist(FILE *dict) {

    int numWords = 0;
    char **words = (char **) malloc(0);
    ReadLine *nextLine = read_line(dict);

    while (1) { 
        // Append the nextLine.line to words
        numWords++;
        words = (char **) realloc(words, numWords * sizeof(char *));
        
        /* 
         * Allocate memory for the word and then strcpy it into the
         * correct index in words
         */
        words[numWords - 1] = \
                (char *) calloc(strlen(nextLine->line) + 1, sizeof(char));
        strcpy(words[numWords - 1], nextLine->line);
       
        if (nextLine->isLastLine) {
            break;
        }

        // Free memory allocated to nextLine before reading the next line to it
        free_readline(nextLine);

        nextLine = read_line(dict);
    }


    // Create the output WordList struct
    WordList *wordList = malloc(sizeof(WordList));
    wordList->words = words;
    wordList->numWords = numWords;

    // Free nextLine
    free_readline(nextLine);

    return wordList;
}
