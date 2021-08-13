#ifndef WORDLIST_H
#define WORDLIST_H

#include <stdbool.h>

/* Struct used to store a list of words.
 *
 * Contains an array of strings and an integer equal to the
 * number of strings in the string array.
 *
 * Used for functions which operate on string arrays where it is useful to
 * also know the number of strings in the array.
 * (i.e. the exact/prefix/anywhere search functions in searchMethods.c)
 */
typedef struct {
    char **words;
    int numWords;
} WordList;

bool *fill_bool(int length);

WordList *string_bool_mask(const bool *mask, WordList *listOfWords);
void free_wordlist(WordList *listOfWords);
void sort_wordlist(WordList *inputList);

#endif
