#ifndef PARSERLIB_H_INCLUED
#define PARSERLIB_H_INCLUED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERRALLOC(x)                                                            \
do                                                                             \
{                                                                              \
    if (x == NULL) {                                                           \
        fprintf(stderr, "ERROR: %s, ligne : %d\n", strerror(errno), __LINE__); \
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
} while(0)

#define MEM_ALLOC(ptr, H, W, type)	                     \
do {							     \
    ptr = (type **)malloc(sizeof(type *) * H);	             \
    ERRALLOC(ptr);			                     \
    for (int i = 0; i < H; i++) {	                     \
        ptr[i] = (type *)malloc(sizeof(type)*W + 1);	     \
        ERRALLOC(ptr[i]);		                     \
    }							     \
} while (0)

#define MEM_FREE(ptr, H)	  \
do {				  \
    for (int i = 0; i < H; i++) { \
        free(ptr[i]);		  \
    }				  \
    free(ptr);			  \
} while (0)

typedef struct
{
    ssize_t  len;
    ssize_t  nbline;
    char    *data;
    ssize_t *linelen;
}FileRef;

// Declaration
ssize_t strsize(char *t);
FILE *OpenFile(const char *FileName);
ssize_t SizeOfFile(FILE *f);
ssize_t CountLine(FILE *f);


# ifdef PARSERLIB_IMPLEMENTATION

ssize_t strsize(char *t)
{
    ssize_t i;
    for (i = 0; t[i] != '\0'; i++) ;
    return i;
}

FILE *OpenFile(const char *FileName)
{
    FILE *f = NULL;
    f = fopen(FileName, "r");
    if (f == NULL) {
	fprintf(stderr, "ERROR: %s: \"%s\"\n", strerror(errno), FileName);
    }
    return f;
}

ssize_t SizeOfFile(FILE *f)
{
    ssize_t ret = 0;
    
    if (fseek(f, 0L, SEEK_END) < 0) {
	fprintf(stderr, "ERROR: %s\n", strerror(errno));
	fclose(f);
	return -1;
    }
    ret = ftell(f);
    if (ret < 0) {
	fprintf(stderr, "ERROR: %s\n", strerror(errno));
	fclose(f);
	return ret;
    }
    rewind(f);
    return ret;
}

// Weaknesses
// - Can not figure out the longest line
// - Wrong NBline if the file exceed 0x1000 character
// Strengths
// - Don't take much memory on ram
// - Fastest speed
ssize_t CountLine(FILE *f)
{
    ssize_t nbline = 0;
    int chunk = 0x1000;
    char buf[chunk];
    char *ret = buf;

    for (nbline = 0; ret != NULL; nbline++) {
	ret = fgets(buf, chunk, f); // chunk 0x1000 is the weakness
	// int overflow = 0;
	// for (overflow = 0; buf[overflow]; overflow++) ;
	// if (overflow >= chunk-1)  fprintf(stderr, "ERROR: The line is longer than 4098 bytes\n");
    }        
    rewind(f);
    return nbline;
}


# endif // PARSERLIB_IMPLEMENTATION
#endif // PARSERLIB_H_INCLUED
