#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define NICOLA_ALGO

#define PARSERLIB_IMPLEMENTATION
#include "parserlib.h"

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

int find_char(char *src, char to_find)
{
    int count = 0;

    for (int i = 0; src[i]; i++) {
        if (src[i] == to_find)
            count++;
    }
    return (count + 1);
}

char **get_data(char *src, char *inter)
{
    char *temp = NULL;
    char **array_value = NULL;
    int nb_column = find_char(src, inter[0]);
    int index = 0;

    array_value = malloc(sizeof(char *) * (nb_column + 1));
    temp = strtok(src, inter);
    for (int i = 0; temp; i++, index++) {
        array_value[index] = strdup(temp);
        temp = strtok( NULL, inter);
        array_value[index + 1] = NULL;
    }
    return (array_value);
}

char ***parse_data(char **src, int nb_line, char *inter)
{
    char ***final_array = NULL;

    final_array = malloc(sizeof(char **) * (nb_line + 1));
    if (!final_array) return (NULL);
    for (int i = 0; i < nb_line; i++) {
        final_array[i] = get_data(src[i], inter);
        final_array[i + 1] = NULL;
    }
    return (final_array);
}

char ***parse_csv(char* file_path, char *inter)
{
    char **array = NULL;
    // verif inter in string ? nice : quit
    int nb_line = count_line_of_file(file_path) - 1;

    array = get_file_data(file_path, nb_line);
    if (!array) return (NULL);
    char ***test = parse_data(array, nb_line, inter);
    // printf("%s - %s - %s\n", test[1][0], test[1][1], test[1][2]);
    return (test);
}


int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }

    #if 1

    char ***ParsedData = parse(argv[0], ' ');
    if (!ParsedData) return 1;

// #if defined(VAR_OPT)
//     for (int i = 0; i < __ft.nbline; i++) {
// 	for (int j = 0; j < __ft.nbword[i]; j++) {
// 	    printf("%s ", ParsedData[i][j]);
// 	}
// 	printf("\n");
//     }

// #else
//     for (int i = 0; ParsedData[i]; i++) {
// 	for (int j = 0; ParsedData[i][j]; j++) {
// 	    printf("%s ", ParsedData[i][j]);
// 	}
// 	putchar('\n');
//     }
// #endif
    
    freeparse(ParsedData);
    return 0;
    
    #elif !defined(NICOLA_ALGO)
    file_t ft;

    DataFromFile(argv[0], &ft);
    // printf("Number-of-character: %ld\n", ft.size);
    // printf("Number-of-line: %ld\n", ft.nbline);

    // for (int i = 0; i < ft.nbline; i++) {
    // for (int i = ft.nbline-1; i > -1; i--) {
	// fwrite(ft.data[i], ft.linelen[i], sizeof(char), stdout);
	// putchar('\n');

	// puts(ft.data[i]);
	
	// printf("%s\n", ft.data[i]);
    // }

    FreeDataFromFile(&ft);

    #else
    char ***ParsedData = parse_csv(argv[0], " ");

    // for (int i = 0; ParsedData[i]; i++) {
    // 	for (int j = 0; ParsedData[i][j]; j++) {
    // 	    printf("%s ", ParsedData[i][j]);
    // 	}
    // 	printf("\n");
    // }

    for (int i = 0; ParsedData[i]; i++) {
	for (int j = 0; ParsedData[i][j]; j++) {
	    free(ParsedData[i][j]);
	}
	free(ParsedData[i]);
    }
    free(ParsedData);
    
    #endif
    return 0;
}
