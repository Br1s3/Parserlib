#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PARSERLIB_IMPLEMENTATION
#include "parserlib.h"

int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }

#if 1

    char ***ParsedData = ParseDataFile(argv[0], ',');
    if (!ParsedData) return 0;

#if defined(VAR_OPT)
    for (int i = 0; i < __ft.nbline; i++) {
	for (int j = 0; j < __ft.nbword[i]; j++) {
	    printf("[%s]", ParsedData[i][j]);
	}
	printf("\n");
    }

#else
    for (int i = 0; ParsedData[i]; i++) {
	for (int j = 0; ParsedData[i][j]; j++) {
	    printf("[%s]", ParsedData[i][j]);
	}
	putchar('\n');
    }
#endif
    
    FreeParsedData(ParsedData);
    return 0;
    
#elif !defined(NICOLA_ALGO)
    file_t ft;

    GetDataFile(argv[0], &ft);
    printf("Number-of-character: %ld\n", ft.size);
    printf("Number-of-line: %ld\n", ft.nbline);

    for (int i = 0; ft.data[i]; i++) {
	printf("nbline: %d $ [%s]\n", i, ft.data[i]);
    }

    FreeDataFile(&ft);
    
#endif
    return 0;
}
