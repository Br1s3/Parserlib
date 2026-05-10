#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// #define VAR_OPT
// #define PARSING_COPY_DATA_FILE
#define PARSING_DATA_FILE
// #define PROG_REF

#define PARSERLIB_IMPLEMENTATION
#include "parserlib.h"


#if defined(PARSING_COPY_DATA_FILE) && defined(VAR_OPT) && !defined(PROG_REF)
#warning Parse copy data from file with VAR_OPT
int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }

    char ***ParsedData = ParseCpyDataFile(argv[0], ',');
    if (!ParsedData) return 0;

    for (int i = 0; i < __ft.nbline; i++) {
	for (int j = 0; j < __ft.nbword[i]; j++) {
	    printf("[%s]", ParsedData[i][j]);
	}
	printf("\n");
    }

    FreeCpyParsedData(ParsedData);
    return 0;
}


#elif defined(PARSING_COPY_DATA_FILE) && !defined(VAR_OPT) && !defined(PROG_REF)
#warning Parse copy data from file without VAR_OPT
int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }

    char ***ParsedData = ParseCpyDataFile(argv[0], ',');
    if (!ParsedData) return 0;

    for (int i = 0; ParsedData[i]; i++) {
	for (int j = 0; ParsedData[i][j]; j++) {
	    printf("[%s]", ParsedData[i][j]);
	}
	putchar('\n');
    }
    
    FreeCpyParsedData(ParsedData);
    return 0;
}


#elif !defined(PARSING_COPY_DATA_FILE) && !defined(PROG_REF) && !defined(PARSING_DATA_FILE)
#warning Get copy data from file
int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }

    file_t ft;

    GetCpyDataFile(argv[0], &ft);
    printf("Number-of-character: %ld\n", ft.size);
    printf("Number-of-line: %ld\n", ft.nbline);

    for (int i = 0; ft.data[i]; i++) {
	printf("nbline: %d $ [%s]\n", i, ft.data[i]);
    }

    FreeCpyDataFile(&ft);
    
    return 0;
}

#elif defined(PARSING_DATA_FILE) && !defined(PROG_REF)
#warning Parse link data from file
int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }

    file_t ft;
    char ***ParsedData = ParseDataFile(argv[0], ' ', &ft);

    // This parsing method is not meant to be used to printf the buffer!
    
    ParsedData[3][1][0] = 'p';
    ParsedData[3][1][1] = 'r';
    ParsedData[3][1][2] = 'o';
    ParsedData[3][1][3] = 'u';
    // ParsedData[3][1][4] = 't';

    FreeParsedData(ParsedData, &ft);
    return 0;
}

#elif defined(PROG_REF) && !defined(PARSING_DATA_FILE) && !defined(PARSING_COPY_DATA_FILE)
#warning Parse copy data from file with ref prog
char **get_file_data(char *path, int nb_line);
int find_char(char *src, char to_find);
char **get_data(char *src, char *inter);
char ***parse_data(char **src, int nb_line, char *inter);
char ***parse_csv(char* file_path, char *inter);

int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }

    char ***ParsedData = parse_csv(argv[0], ",");

    for (int i = 0; ParsedData[i]; i++) {
	for (int j = 0; ParsedData[i][j]; j++) {
	    printf("[%s]", ParsedData[i][j]);
	    // printf("%d,%ld,\n", gg++, strtoul(ParsedData[i][j], NULL, 16));
	}
	putchar('\n');
    }

    for (int i = 0; ParsedData[i]; i++) {
	for (int j = 0; ParsedData[i][j]; j++) {
	    if (ParsedData[i][j])
	        free(ParsedData[i][j]);
	}
	free(ParsedData[i]);
    }
    free(ParsedData);
    return 0;
}


int count_line_of_file(char *path)
{
    FILE *file = NULL;
    size_t len = 0;
    int count = 0;
    char *buffer = NULL;

    file = fopen(path, "r");
    if (!file) {puts("chemin introuvable");return (0);}
    while (getline(&buffer, &len, file) != -1) {
        count++;
    }
    if (buffer) free(buffer);
    fflush(stdout);
    fclose(file);
    return (count + 1);
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
    array_value = malloc(sizeof(char *) * (nb_line + 1));
    while (getline(&buffer, &len, file) != -1) {
        buffer[strlen(buffer) - 1] = 0;
        array_value[index] = strdup(buffer);
        index++;
    }
    array_value[index] = NULL;
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

    final_array = (char ***)malloc(sizeof(char **) * (nb_line + 1));
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
    char ***test = NULL;
    int nb_line = count_line_of_file(file_path) - 1;

    array = get_file_data(file_path, nb_line);
    if (!array)
        return (NULL);
    test = parse_data(array, nb_line, inter);
    for (int i = 0; i < nb_line; i++) {
        free(array[i]);
    }
    free(array);
    return (test);
}

#endif
