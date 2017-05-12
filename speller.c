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
    
    // spell-check each word in text
    for (int x = fgetc(input); x != EOF; x = fgetc(input))
    {
        // allow only alphabetical characters and apostrophes
        if (isalpha(x) || (x == '\'' && ind > 0))
        {
            // append character to word
            word[ind] = x;
            ind++;

            // ignore alphabetical strings too long to be words
            if (ind > LENGTH)
            {
                // consume remainder of alphabetical string
                while ((x = fgetc(input)) != EOF && isalpha(x))
                {

                    // prepare for new word
                    ind = 0;
                
                }
            }
        }
        
        // ignore words with numbers (like MS Word can)
        else if (isdigit(x))
        {
            // consume remainder of alphanumeric string
            while ((x = fgetc(input)) != EOF && isalnum(input))
            {
            // prepare for new word
                ind = 0;
            }    
            
        }

        // we must have found a whole word
        else if (ind > 0)
        {
            // terminate current word
            word[ind] = '\0';

            // update counter
            words++;

            // check word's spelling
            getrusage(RUSAGE_SELF, &before);
            bool misspelled = !check(word);
            getrusage(RUSAGE_SELF, &after);

            // update benchmark
            time_check += calculate(&before, &after);

            // print word if misspelled
            if (misspelled)
            {
                printf("%s\n", word);
                misspellings++;
            }

            // prepare for next word
            ind = 0;
        }
    }
    
    // check whether there was an error
    if (ferror(input))
    {
        fclose(input);
        printf("Error reading %s.\n", text);
        unload();
        return 1;
    }

    // close text
    fclose(input);

    // determine dictionary's size
    getrusage(RUSAGE_SELF, &before);
    unsigned int n = size();
    getrusage(RUSAGE_SELF, &after);

    // calculate time to determine dictionary's size
    time_size = calculate(&before, &after);

    // unload dictionary
    getrusage(RUSAGE_SELF, &before);
    bool unloaded = unload();
    getrusage(RUSAGE_SELF, &after);

    // abort if dictionary not unloaded
    if (!unloaded)
    {
        printf("Could not unload %s.\n", dictionary);
        return 1;
    }

    // calculate time to unload dictionary
    time_unload = calculate(&before, &after);

    // report benchmarks
    printf("\nWORDS MISSPELLED:     %d\n", misspellings);
    printf("WORDS IN DICTIONARY:  %d\n", n);
    printf("WORDS IN TEXT:        %d\n", words);
    printf("TIME IN load:         %.2f\n", time_load);
    printf("TIME IN check:        %.2f\n", time_check);
    printf("TIME IN size:         %.2f\n", time_size);
    printf("TIME IN unload:       %.2f\n", time_unload);
    printf("TIME IN TOTAL:        %.2f\n\n", 
    time_load + time_check + time_size + time_unload);

    
    return 0;
}

/**
 * Returns number of seconds between b and a.
 */
double calculate(const struct rusage *befr, const struct rusage *aftr)
{
    if (befr == NULL || aftr == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((aftr->ru_utime.tv_sec * 1000000 + aftr->ru_utime.tv_usec) - 
                 (befr->ru_utime.tv_sec * 1000000 + befr->ru_utime.tv_usec)) + 
                ((aftr->ru_stime.tv_sec * 1000000 + aftr->ru_stime.tv_usec) - 
                 (befr->ru_stime.tv_sec * 1000000 + befr->ru_stime.tv_usec)))
                / 1000000.0);
    }
}
