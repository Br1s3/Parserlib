#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define WITHOUT_LINELEN
// #define NICOLA_ALGO

// #define PARSE_CSV_IMPLEMENTATION
// #include "parse_csv.h"

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
    
    #if defined(WITHOUT_LINELEN)
    FileRef fileref;

    FILE *file = OpenFile(argv[0]);
    // fileref.len    = SizeOfFile(file);
    // printf("Number-of-character: %ld\n", fileref.len);
    fileref.nbline = CountLine(file);
    // printf("Number-of-line: %ld\n", fileref.nbline);
    
    fileref.data = DataFromFile(file, fileref.nbline);
    // for (int i = 0; i < fileref.nbline; i++) {	
	// fwrite(fileref.data[i], fileref.linelen[i], sizeof(char), stdout);
	// putchar('\n');
	
	// puts(fileref.data[i]);
	
	// printf("%s\n", fileref.data[i]);
    // }
    
    for (int i = 0; i < fileref.nbline; i++) {
	free(fileref.data[i]);
    }
    free(fileref.data);

    #elif defined(NICOLA_ALGO)
    FileRef fileref;
    fileref.nbline = count_line_of_file(argv[0]);

    fileref.data = get_file_data(argv[0], fileref.nbline);

    // for (int i = 0; i < fileref.nbline; i++) {
    // 	puts(fileref.data[i]);
    // }
    
    for (int i = 0; i < fileref.nbline; i++) {
	free(fileref.data[i]);
    }
    free(fileref.data);
    

    #elif !defined(WITHOUT_LINELEN)
    
    fileref.linelen = LineLen(file, fileref.nbline);    
    fileref.data = DataFromFile(file, fileref.nbline, fileref.linelen);

//    for (int i = 0; i < fileref.nbline; i++) {	
	// printf("line: %d, linelen: %ld \t|", i, fileref.linelen[i]);
	// fwrite(fileref.data[i], fileref.linelen[i], sizeof(char), stdout);
	// putchar('\n');

//	puts(fileref.data[i]);

	// printf("%s\n", fileref.data[i]);
//    }

    for (int i = 0; i < fileref.nbline; i++) {
	free(fileref.data[i]);
    }
    free(fileref.data);
    fclose(file);

    #endif
    return 0;
    
    // char ***tab3d = parse(argv[0], ' ');
    // ERRALLOC(tab3d);

    // for (int i = 0; i < fileref.nbline; i++) {
    // 	for (int j = 0; fileref.linelen[i]; j++) {
    // 	    for (int k = 0; k < 5; k++) {
    // 		printf("|%c|", tab3d[i][j][k]);
    // 	    }
    // 	}
    // }
    
    // free(tab);
    // MEM_FREE(tab2d, NBline);
    
    // // int count = 0;
    // do {
    //     fgets(buf, sizeof(buf), file);
    // 	count += strlen(buf);
    // 	printf("|%s|\n", buf);
    // } while(count < file_len);

    return 0;
}
