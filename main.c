#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PARSERLIB_IMPLEMENTATION
#include "parserlib.h"


#if 0
#warning COUNTLINE 1
// Weaknesses
// - Slow
// - Can only determine the longest line and not all
// Strengths
// - Secure

// int CountLine(FILE *f, int filesize, int *linelen)
int CountLine(FILE *f)
{
    int count = 0;
    int NBline = 0;
    int chunk = 5;

    int filesize = SizeOfFile(FILE *f);

    // if (*linelen <= 0) chunk = 5;
    // else               chunk = *linelen;

    do {
	char buf[chunk];
	fgets(buf, chunk, f);
	int i;
	for (i = 0; i < chunk; i++) {
	    // printf(".%c.", buf[i]);
	    if (buf[i] == '\n') {
		NBline++;
		break;
	    }
	}
	if (i == chunk) {
	    chunk += 1;
	    count-=2; // count = count - (newline + \0)
	}
	count += i+1; // count = count + (nb char + newline)
	
    } while (ftell(f) < filesize);
    
    rewind(f);

    // *linelen = chunk;
    return NBline;
}

#elif 0
#warning COUNTLINE 2
// Weaknesses
// - Can not figure out the longest line
// - Conditional jump or move depends on uninitialised value(s) because buf[chunk] not fill of character
// Strengths
// - Really quick
// - Don't take much memory on ram
// - resiliant to the size of chunk BUT DON'T EXCEED 0x1000

int CountLine(FILE *f)
{
    int chunk = 0x1000;
    int nbline = 0;

    ssize_t filesize = SizeOfFile(f);
    
    for (int count = 0; count < filesize; count+=chunk) {
	char buf[chunk];

	fread(buf, sizeof(char), chunk, f);
	// printf("%s", buf);
	for (int i = 0; i < chunk; i++) {
	    if (buf[i] == '\n') nbline++;
	}
    }
    // puts("");
    rewind(f);
    return nbline;
}

#endif

char **DataFromFile(FILE *f, int nbline)
{
    char *chunk = NULL;
    size_t linelen = 0;
    char **data = NULL;
    
    data = (char **)malloc(sizeof(char *)*(nbline));
    ERRALLOC(data);
    
    for (int i = 0; i < nbline; i++) {
	ssize_t ret = getline(&chunk, &linelen, f);
	
	if (ret < 0) {
	    data[i] = (char *)malloc(sizeof(char)*(1));
	    ERRALLOC(data);
	    data[i][0] = '\0';
	    break;
	}
	data[i] = (char *)malloc(sizeof(char)*(ret + 1));
	ERRALLOC(data);

	for (ssize_t j = 0; j < ret; j++) data[i][j] = chunk[j];
	data[i][ret] = '\0';
    }
    rewind(f);
    return data;
}

char *Data1dFromFile(FILE *f, int filelen)
{
    char *data = (char *)malloc(sizeof(char)*filelen);
    ERRALLOC(data);
    
    fread(data, sizeof(char), filelen, f);
    
    return data;
}

#if 1
char *ReferanceFromFile(FILE *f, ssize_t filelen, ssize_t nbline)
{
    int *ref = (char *)malloc(sizeof(char)*nbline);
    ERRALLOC(ref);
    
    char *data = Data1dFromFile(f, filelen);

    for (ssize_t i = 0, j = 0, k = 0; i < filelen; i++, k++) {
	if (data[i] == '\n') {
	    ref[j] = k;
	    k = 0; j++;
	}
    }
    free(data);
    return ref;
}

#elif 0
char *ReferanceFromFile(FILE *f, ssize_t filelen)
{
    ssize_t nbline = CountLine(f);
    
    for (int i = 0; ) {
	char *fgets(char *s, int size, FILE *stream);
    }
}

#endif

char ***parse(const char *FileName, char delim)
{
    char bufp;
    char *buf = &bufp;
    size_t wordlen = 0;
    char ***ParsedData = NULL;
    
    FILE *f = OpenFile(FileName);
    
    int nbline = CountLine(f);
    char **data = DataFromFile(f, nbline);
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


int main(int argc, char **argv)
{
    argc--; argv++;
    if (argc < 1) {
	fprintf(stderr, "ERROR: No argument, the name of the file to read is missing\n");
	return 1;
    }
    
    FILE *file = OpenFile(argv[0]);

    FileRef fileref;
    fileref.len    = SizeOfFile(file);
    printf("Number-of-character: %d\n", fileref.len);
    fileref.nbline = CountLine(file);
    printf("Number-of-line: %d\n", NBline);
    fileref.
    


    char **tab2d = NULL;
    tab2d = DataFromFile(file, NBline);
    ERRALLOC(tab2d);
    // char *tab = DataFromFile(file, file_len);


    // fwrite(tab, file_len, sizeof(char), stdout);
    for (int i = 0; i < NBline; i++) {
	// puts(tab[i]);
	// printf("%s", tab2d[i]);
	// int ggwp;
	// for (ggwp = 0; tab2d[ggwp]; ggwp++) ;
	// printf("%p -> %s", tab2d[i], tab2d[i]);
	// int ggwp = strsize(tab2d[i]);
	int ggwp = strsize(tab2d[i]);
	fwrite(tab2d[i], ggwp, sizeof(char), stdout);
    }


    char ***tab3d = parse(argv[0], ' ');
    ERRALLOC(tab3d);

    for (int i = 0; i < NBline; i++) {
	for (int j = 0; tab3d[i] != NULL; j++) {
	    for (int k = 0; tab3d[i][j] != NULL; k++) {
		printf("|%c|", tab3d[i][j][k]);
	    }
	}
    }
    
    // free(tab);
    MEM_FREE(tab2d, NBline);
    
    // // int count = 0;
    // do {
    //     fgets(buf, sizeof(buf), file);
    // 	count += strlen(buf);
    // 	printf("|%s|\n", buf);
    // } while(count < file_len);

    fclose(file);
    return 0;
}
