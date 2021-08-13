#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "wordList.h"
#include "readDict.h"
#include "searchMethods.h"

// Minimum valid possible amount of input arguments (command name + pattern)
#define MIN_INPUT_ARGS 2

/*
 * Numbers corresponding to search options, these values are used for the
 * searchOption variable of OptionArg structs
 */
#define INVALID_OPTION (-1)
#define EXACT 0
#define PREFIX 1
#define ANYWHERE 2
#define SORT 3

/*
 * Struct for storing the pattern and filepath inputs to search
 */
typedef struct {
    char *pattern;
    char *filePath;
} NonOptionArgs;

/*
 * Struct for storing search options of a search:
 * searchOption: int corresponding to a searchOption macro
 * sortEnabled: int, 0 or 1 depending if sort is enabled
 * numOptions: int of total search options given
 */
typedef struct {
    int searchOption;
    int sortEnabled;
    int numOptions;
} OptionArgs;

/*
 * Output after successful search.
 * outputList: Output word list of matched words (unsorted)
 * selectedOptions: Input options given to search as OptionArgs struct
 */
typedef struct {
    WordList *outputList;
    OptionArgs *selectedOptions;
} SearchOutput;

static SearchOutput search_and_get_output(int argc, char  **argv);
static OptionArgs *get_args(int argc, char **argv);
static NonOptionArgs *get_pattern_and_filepath(int nonOptionCount,
        int argc, char **argv);
static bool check_pattern(char *pattern);
static void free_non_option_args(NonOptionArgs *options);
static WordList *run_search(int searchOption, char *pattern, FILE *dict);

int main(int argc, char **argv) {

    SearchOutput output = search_and_get_output(argc, argv);

    // Sort list if -sort option given
    if (output.selectedOptions->sortEnabled) {
        sort_wordlist(output.outputList);
    }
    free(output.selectedOptions);

    // Print matched words
    for (int i = 0; i < (output.outputList)->numWords; ++i) {
        printf("%s\n", (output.outputList)->words[i]);
    }
    free_wordlist(output.outputList);

    return 0;
}

/*
 * Given input arguments to main (argc, argv), do the following:
 * Check for invalid input arguments, pattern and file and exit with -1
 * if found incorrect input arguments.
 *
 * Else runs search on given or default dictionary as per the given options.
 *
 * Returns SearchOutput struct containing:
 * - unsorted WordList of matches
 * - OptionArgs struct corresponding to the given search options
 */
static SearchOutput search_and_get_output(int argc, char  **argv) {
    // Check arguments are valid and get search options
    OptionArgs *selectedOptions = get_args(argc, argv);
    if (selectedOptions->searchOption == INVALID_OPTION) {
        fprintf(stderr, "Usage: search [-exact|-prefix|-anywhere]"
                " [-sort] pattern [filename]\n");
        free(selectedOptions);
        exit(-1);
    }

    int nonOptionCount = argc - 1 - selectedOptions->numOptions;
    NonOptionArgs *patternAndPath = \
            get_pattern_and_filepath(nonOptionCount, argc, argv);

    // Check dictionary file path validity, then read dict file
    if (fopen(patternAndPath->filePath, "r") == NULL) {
        fprintf(stderr, "search: file \"%s\" can not be opened\n", \
                patternAndPath->filePath);
        free(selectedOptions);
        free_non_option_args(patternAndPath);
        exit(-1);
    }
    FILE *dict = fopen(patternAndPath->filePath, "r");

    // Check if pattern is valid
    if (!check_pattern(patternAndPath->pattern)) {
        fprintf(stderr, "search: pattern should only "
                "contain question marks and letters\n");
        free_non_option_args(patternAndPath);
        free(selectedOptions);
        fclose(dict);
        exit(-1);
    }

    WordList *outputList = run_search(selectedOptions->searchOption,
            patternAndPath->pattern, dict);
    fclose(dict);
    free_non_option_args(patternAndPath);

    // Return -1 if 0 words were found
    if (outputList->numWords < 1) {
        free(outputList);
        free(selectedOptions);
        exit(-1);
    }

    SearchOutput output;
    output.outputList = outputList;
    output.selectedOptions = selectedOptions;
    return output;
}

/*
 * Retrives input option arguments in argv to a WordList
 */
static WordList *get_input_options(int argc, char **argv) {
    int optionCount = 0;
    char **options = (char **) malloc(0);

    for (int i = 1; i < argc; ++i) {
        // Check if argument starts with a hypen (i.e. might be an option)
        if (argv[i][0] == '-') {
            /*
             * Check it isn't the filepath, 
             * i.e. if it's the last argument and the previous argument didn't
             * start with '-' (pattern cannot have '-')
             */
            if (i == argc - 1 && argv[argc - 2][0] != '-') {
                continue;
            }

            // Add found option to options
            optionCount++;
            options = (char**) realloc(options, sizeof(char *) * optionCount);

            int index = optionCount - 1;
            options[index] = calloc(strlen(argv[i]) + 1, sizeof(char));
            strcpy(options[index], argv[i]);
        }
    }

    // Create and return the output WordList
    WordList *optionList = malloc(sizeof(WordList));
    optionList->numWords = optionCount;
    optionList->words = options;

    return optionList;
}

/*
 * Given an option as a string, returns its corresponding option number
 */
static int get_option_num(char *option) {
    int optionNum = -1;
    char *validOptions[] = {"-exact", "-prefix", "-anywhere", "-sort"};

    // Find which validOption the given option matches
    for (int i = 0; i < 4; ++i) {
        if (strcmp(option, validOptions[i]) == 0) {
            optionNum = i;
        }
    }

    return optionNum;
}

/*
 * Sets search & sort options of an OptionArg given an input option as a string
 */
static void set_selected_options(OptionArgs *selectedOptions, char *option) {
    // Set searchOption if option is one of -anywhere, -exact or -prefix
    if (get_option_num(option) <= ANYWHERE) {
        selectedOptions->searchOption = get_option_num(option);
    // Set sortEnabled if option is -sort
    } else if (get_option_num(option) == SORT) {
        selectedOptions->sortEnabled = 1;
    }
}

/*
 * - Checks for correct argument number
 * - Checks search options are valid
 *
 * Returns selected options as an OptionArgs struct
 */
static OptionArgs *get_args(int argc, char **argv) {
    OptionArgs *selectedOptions = malloc(sizeof(OptionArgs));
    selectedOptions->searchOption = EXACT;
    // Get WordList of -(option) arguments
    WordList *optionList = get_input_options(argc, argv);

    // Check if number of -option arguments is correct
    int numOptions = optionList->numWords;
    if (numOptions > 2) {
        selectedOptions->searchOption = INVALID_OPTION;
    }
    selectedOptions->numOptions = numOptions;

    // Check if number of input args is correct
    if (!(argc >= MIN_INPUT_ARGS + numOptions \
            && (argc <= (MIN_INPUT_ARGS + numOptions + 1)))) {
        selectedOptions->searchOption = INVALID_OPTION;
    }

    if (selectedOptions->searchOption != INVALID_OPTION) {
        switch (numOptions) {
            case 1:
                // Set search option
                set_selected_options(selectedOptions, optionList->words[0]);
                break;
            case 2:
                for (int i = 0; i < 2; ++i) {
                    // Set selected option
                    set_selected_options(selectedOptions,
                            optionList->words[i]);
                    if (selectedOptions->searchOption == INVALID_OPTION) {
                        break;
                    }
                }
                // Check for sort option was given and for identical options
                if (!selectedOptions->sortEnabled ||
                        !strcmp(optionList->words[0], optionList->words[1])) {
                    selectedOptions->searchOption = INVALID_OPTION;
                }
                break;
            default:
                // If no given seach options, set the defaults
                selectedOptions->sortEnabled = 0;
                selectedOptions->searchOption = EXACT;
                break;
        }
    }

    free_wordlist(optionList);
    return selectedOptions;
}

/*
 * Retrives pattern and filename when search is run to NonOptionArgs struct
 */
static NonOptionArgs *get_pattern_and_filepath(int nonOptionCount,
        int argc, char **argv){

    NonOptionArgs *result = malloc(sizeof(NonOptionArgs));

    /*
     * If theres one non option arg, interpret it as the pattern and save
     * to result
     */
    if (nonOptionCount == 1) {
        // Set the pattern
        result->pattern = calloc(strlen(argv[argc - 1]) + 1,
                sizeof(char));
        strcpy(result->pattern, argv[argc - 1]);

        char *defaultPath = "/usr/share/dict/words";
        result->filePath = calloc(strlen(defaultPath) + 1, sizeof(char));
        strcpy(result->filePath, defaultPath);

    } else {
        /*
         * Else both pattern and file path are given options, save these to
         * result
         */
        result->pattern = calloc(strlen(argv[argc - 2]) + 1,
                sizeof(char));
        strcpy(result->pattern, argv[argc - 2]);

        result->filePath = calloc(strlen(argv[argc - 1]) + 1,
                sizeof(char));
        strcpy(result->filePath, argv[argc - 1]);
    }

    return result;
}

/*
 * Frees memory allocated to a NonOptionArgs struct
 */
static void free_non_option_args(NonOptionArgs *options) {
    free(options->pattern);
    free(options->filePath);
    free(options);
}

/*
 * Checks if a pattern given as a string is valid as per the assignment spec
 */
static bool check_pattern(char *pattern) {

    bool result = true;
    
    // Check if each character in the pattern is a letter or ?
    for (int i = 0; i < strlen(pattern); ++i) {
        if (!isalpha(pattern[i]) && pattern[i] != '?') {
            result = false;
            break;
        }
    }

    return result;
}

/*
 * Searches through a dictionary file with given pattern and search options
 */
static WordList *run_search(int searchOption, char *pattern, FILE *dict) {
    WordList *dictList = file_to_wordlist(dict);
    WordList *outputList;

    // Run different search mode depending on given searchOption
    switch (searchOption) {
        case PREFIX:
            outputList = prefix_match(pattern, dictList);
            break;
        case ANYWHERE:
            outputList = anywhere_match(pattern, dictList);
            break;
        default:
            outputList = exact_match(pattern, dictList);
    }

    // Clean up the word list of the dictionary
    free_wordlist(dictList);

    return outputList;
}
