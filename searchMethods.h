#ifndef SEARCHMETHODS_H
#define SEARCHMETHODS_H

#include "wordList.h"

WordList *exact_match(char *pattern, WordList *dictList);
WordList *prefix_match(char *pattern, WordList *dictList);
WordList *anywhere_match(char *pattern, WordList *dictList);

#endif //SEARCHMETHODS_H
