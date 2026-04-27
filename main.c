#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// #define NICOLA_ALGO

#define PARSERLIB_IMPLEMENTATION
#include "parserlib.h"

#if 0
char ***parse(const char *FileName, char delim)
{
    char bufp;
    char *buf = &bufp;
    size_t wordlen = 0;
    char ***ParsedData = NULL;
    
    FILE *f = OpenFile(FileName);
    
    int nbline = CountLine(f);
    char **data = DataFromFile(f);
    ERRALLOC(data);

    ParsedData = (char ***)malloc(sizeof(char **)*nbline);
    ERRALLOC(data);
    for (int i = 0; i < nbline; i++) {
	ssize_t linelen = strsize(data[i]);
	
	ParsedData[i] = (char **)malloc(sizeof(char *)*linelen);
	ERRALLOC(ParsedData);
	ssize_t ret = 0;
	for (ssize_t j = 0; j < linelen; j+=ret) {
	    
	    ret = getdelim(&buf, &wordlen, delim, f);
	    if (ret < 0) {
		ParsedData[i][j] = (char *)malloc(sizeof(char)*1);
		ERRALLOC(ParsedData);
		ParsedData[i][j][0] = '\0';
		break;
	    }
	    ParsedData[i][j] = (char *)malloc(sizeof(char)*(ret+1));
	    ERRALLOC(ParsedData);

	    for (int k = 0; k < ret; k++) {
		ParsedData[i][j][k] = buf[k];
	    }
	    ParsedData[i][j][ret] = '\0';
	}
    }
    
    return ParsedData;
}
#endif


int count_line_of_file(char *path)
{
    FILE *file = NULL;
    size_t len = 0;
    int count = 0;
    char *buffer = NULL;

    file = fopen(path, "r");
    if (!file) return (0);
    while (getline(&buffer, &len, file) != -1) {
        count++;
    }
    if (buffer) free(buffer);
    fflush(stdout);
    return count;
}

char **get_file_data(char *path, int nb_line)
{
    FILE *file = NULL;
    char **array_value = NULL;
    size_t len = 0;
    char *buffer = NULL;
    int index = 0;

    file = fopen(path, "r");
    if (!file) return (NULL);
    array_value = malloc(sizeof(char *) * nb_line);
    while (getline(&buffer, &len, file) != -1) {
        buffer[strlen(buffer) - 1] = 0;
        if (index > 0)
            array_value[index - 1] = strdup(buffer);
        index++;
    }
    fclose(file);
    if (buffer) free(buffer);
    return (array_value);
}


int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }
    
    #if !defined(NICOLA_ALGO)
    file_t ft;

    FILE *file = OpenFile(argv[0]);
    // ft.size    = SizeOfFile(file);
    // printf("Number-of-character: %ld\n", ft.size);
    ft.nbline = CountLine(file);
    // fclose(file);
    // printf("Number-of-line: %ld\n", ft.nbline);
    ft.data = DataFromFile(file, ft.nbline);
    munmap(ft.data, sizeof(char *)*ft.nbline);
    // for (int i = 0; i < ft.nbline; i++) {
	// fwrite(ft.data[i], ft.linelen[i], sizeof(char), stdout);
	// putchar('\n');
	
	// puts(ft.data[i]);
	
	// printf("%s\n", ft.data[i]);
    // }
    // munmap(ft.data, ft.size);
    // for (int i = 0; i < ft.nbline; i++) {
    // 	free(ft.data[i]);
    // }
    // free(ft.data);

    #else
    file_t ft;
    ft.nbline = count_line_of_file(argv[0]);

    ft.data = get_file_data(argv[0], ft.nbline);

    // for (int i = 0; i < ft.nbline; i++) {
    // 	puts(ft.data[i]);
    // }
    
    for (int i = 0; i < ft.nbline; i++) {
	free(ft.data[i]);
    }
    free(ft.data);
    
    #endif
    return 0;
}
