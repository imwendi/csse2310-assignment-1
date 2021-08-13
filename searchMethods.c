#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "searchMethods.h"
#include "wordList.h"

/*
 * Checks if a string contains only letters.
 */
static bool check_str_alpha(char *word) {
    for (int i = 0; i < strlen(word); ++i) {
        // Check if the character is a letter
        if (!isalpha(word[i])) {
            return false;
        }
    }

    return true;
}

/* Tests a character of an input pattern against a dictionary character.
 * Returns true only on valid matches defined in the assignment description.
 */
static bool is_valid_char(char inputLetter, char dictLetter) {
    bool result;
    // Convert letters to lower case
    char iLetter = tolower(inputLetter);
    char dLetter = tolower(dictLetter);

    // Check if the input letter is a question mark?
    if ((inputLetter == '?') && isalpha(dLetter)) {
        result = true;
    // Check if the input letter is equal to the dictionary letter
    } else if (isalpha(iLetter) && (iLetter == dLetter) && isalpha(dLetter)) {
        result = true;
    } else {
        result = false;
    }

    return result;
}

/* Performs prefix or exact match against a dictionary of words dictList given
 * as a WordList struct.
 * Returns a mask (bool arr) corresponding to dictList
 * indices of words which match the input pattern.
 * If matchLength is true, exact matching is performed.
 */
static bool *bool_mask_match(char *pattern,
        WordList *dictList, bool matchLength) {

    bool *wordFlags = fill_bool(dictList->numWords);

    for (int word = 0; word < dictList->numWords; ++word) {

        /* If length matching is enabled, flag false if the input word length
         * is not equal to the length of the dict word length.
         */
        if (matchLength && strlen(pattern) \
                != strlen(dictList->words[word])) {
            wordFlags[word] = false;
            continue;
        }
       
        // Flag a word as false if it contains non-letters
        if (!check_str_alpha(dictList->words[word])) {
            wordFlags[word] = false;
            continue;
        }

        // Empty input pattern matches everything 
        if (strlen(pattern) == 0) {
            wordFlags[word] = true;
            continue;
        }

        // Flag a word as false if its length is less than the input string 
        if (strlen(pattern) > strlen(dictList->words[word])) {
            wordFlags[word] = false;
            continue;
        }

        for (int i = 0; i < strlen(pattern); ++i) {
            if (!is_valid_char(pattern[i], (dictList->words)[word][i])) {
                wordFlags[word] = false;
                /* Break and move onto the next word as
                 * soon as the current word has
                 * been flagged false.
                 */
                break;
            }
        }
    }

    return wordFlags;
}

/*
 * Runs exact matching on a WordList given an input pattern and a WordList of
 * a dictionary.
 * Returns pointer to WordList of matching words.
 */
WordList *exact_match(char *pattern, WordList *dictList) {
    // Run exact matching
    bool *wordFlags = bool_mask_match(pattern, dictList, true);

    // Get and return output wordList given the mask wordFlags
    return string_bool_mask(wordFlags, dictList);
}

/*
 * Runs prefix matching on a WordList given an input pattern and a WordList of
 * a dictionary.
 * Returns pointer to WordList of matching words.
 */
WordList *prefix_match(char *pattern, WordList *dictList) {
    // Run prefix matching
    bool *wordFlags = bool_mask_match(pattern, dictList, false);

    // Get and return output WordList given the mask wordFlags
    return string_bool_mask(wordFlags, dictList);
}

/*
 * Runs anywhere matching on a WordList given an input pattern and a WordList
 * of a dictionary.
 * Returns pointer to WordList of matching words.
 */
WordList *anywhere_match(char *pattern, WordList *dictList) {
    bool *wordFlags = (bool*) calloc(dictList->numWords, sizeof(bool));
   
    /*
     * Temporary WordList only containing one word.
     * Used to store the current one being checked for the pattern
     */
    WordList tempList;
    tempList.numWords = 1;
    tempList.words = (char **) malloc(sizeof(char *) * 1);

    for (int word = 0; word < (dictList->numWords); word++) {
        char *currentWord = dictList->words[word];

        // Flag false if dict word contains non-letters
        if (!check_str_alpha(currentWord)) {
            wordFlags[word] = false;
            continue;
        }

        /*
         * Process for anywhere matching:
         * Take all letters of the word from the ith letter as a word and run
         * prefix matching on it.
         * Flag word as true and exit loop if patter is matched, else
         * repeat from the i+1th letter.
         *
         * (A rather naive algorithm, but hey, it works! :D )
         */
        for (int i = 0; i < strlen(currentWord); i++) {
            tempList.words[0] = &(currentWord[i]);
            if (bool_mask_match(pattern, &tempList, false)[0]) {
                wordFlags[word] = true;
                break;
            }
        }
    }

    // Free memory allocated to the single word stored in tempList
    free(tempList.words);
    
    // Get and return the output WordList given the mask wordFlags
    return string_bool_mask(wordFlags, dictList);
}
