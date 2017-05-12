/**
 * Implements a spell-checker.
 */

#include <ctype.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "dictionary.h"
#undef calculate
#undef getrusage

// default dictionary
#define DICTIONARY "dictionaries/large"

// prototype
double calculate(const struct rusage *befr, const struct rusage *aftr);

int main(int argc, char *argv[])
{
    // check for correct number of args
    if (argc != 2 && argc != 3)
    {
        printf("Usage: speller [dictionary] text\n");
        return 1;
    }

    // structs for timing data
    struct rusage before, after;

    // benchmarks
    double time_load = 0.0;
    double time_check = 0.0;
    double time_size = 0.0;
    double time_unload = 0.0;

    // determine dictionary to use
    char* dictionary = (argc == 3) ? argv[1] : DICTIONARY;

    // load dictionary
    getrusage(RUSAGE_SELF, &before);
    bool loaded = load(dictionary);
    getrusage(RUSAGE_SELF, &after);

    // abort if dictionary not loaded
    if (!loaded)
    {
        printf("Could not load %s.\n", dictionary);
        return 1;
    }
    
    // calculate time to load dictionary
    time_load = calculate(&before, &after);

    // try to open text
    char *text = (argc == 3) ? argv[2] : argv[1];
    FILE *input = fopen(text, "r");
    if (input == NULL)
    {
        printf("Could not open %s.\n", text);
        unload();
        return 1;
    }

    // prepare to report misspellings
    printf("\nMISSPELLED WORDS\n\n");

    // prepare to spell-check
    int ind = 0, misspellings = 0, words = 0;
    char word[LENGTH + 1];

    
    return 0;
}