#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "wordList.h"

/*
 *  Returns a boolean array of all True values
 */
bool *fill_bool(int length) {
    bool *result = (bool *) malloc(length * sizeof(bool));

    // Set every value in the bool array to true
    for (int i = 0; i < length; i++) {
        result[i] = true;
    }

    return result;
}

/*
 * Masks a WordList with a mask that is an array of booleans.
 * Returns WordList of words corresponding to 'true' values in the mask.
 */
WordList *string_bool_mask(const bool *mask, WordList *listOfWords) {
    // Total number of true values
    int totalTrue = 0;
    // Array of indexes matching true in the bool mask
    int *trueIndices = (int *) malloc(0);

    // Check if the word's index corresponds to a true entry in the mask
    for (int i = 0; i < listOfWords->numWords; ++i) {
        if (mask[i]) {
            // Update the true counter
            totalTrue += 1;
            // Add the index to trueIndices
            trueIndices = realloc(trueIndices, totalTrue * sizeof(int));
            trueIndices[totalTrue - 1] = i;
        }
    }

    char **outputWords = (char **) malloc(totalTrue * sizeof(char *));
    for (int i = 0; i < totalTrue; ++i) {
        // Word corresponding to the current true index 
        char *currentWord = listOfWords->words[trueIndices[i]];
        // Get length of the word
        int wordLen = strlen(currentWord);
        // the +1 below is to account for the \0 character
        outputWords[i] = calloc(wordLen + 1, sizeof(char));
        // Add the word to the outputWords array
        strcpy(outputWords[i], currentWord);
    }

    free(trueIndices);

    // Create and return the output WordList
    WordList *output = malloc(sizeof(WordList));
    output->words = outputWords;
    output->numWords = totalTrue;

    return output;

}

/*
 * Frees memory associated to a WordList at a pointer
 */
void free_wordlist(WordList *listOfWords) {
    //Free all structure members
    for (int i = 0; i < (listOfWords->numWords); ++i) {
        // Free each string in the string array listofWords
        free(listOfWords->words[i]);
    }
    free(listOfWords->words);
    free(listOfWords);
}

/*
 * Comparator function to be passed to qsort
 * Compares two strings given as void pointers.
 * Returns int <1 if the first string comes before the second when
 * sorted alphabetically.
 * Returns 0 if they're equal in alphabetical order.
 * Returns int >1 if the secondstrin comes before the first alphabetically
 */
static int compare_words(const void *p, const void *q) {
    // Cast to pointers to strings
    char **first = (char **) p;
    char **second = (char **) q;

    // Compare the two strings at the pointers and return compare value
    return strcasecmp(*first, *second);
}

/*
 * Uses qsort to sort the array words in a WordList alphabetically
 */
void sort_wordlist(WordList *inputList) {
    // Run qsort to sort the inputList alphabetically
    qsort(inputList->words, inputList->numWords, sizeof(char *),
            compare_words);
}
