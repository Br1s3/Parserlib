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
    ssize_t *linelen;
    char   **data;
}FileRef;

// Declaration
ssize_t strsize(char *t);
FILE *OpenFile(const char *FileName);
ssize_t SizeOfFile(FILE *f);
ssize_t CountLine(FILE *f);
ssize_t *LineLen(FILE *f, ssize_t nbline);
#if 0
char **DataFromFile(FILE *f, ssize_t nbline, ssize_t **ref);
// char **DataFromFile(FILE *f, ssize_t nbline);
#elif 1
char **DataFromFile(FILE *f, ssize_t nbline, ssize_t *ref);

#endif
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

ssize_t myfgets(char *buf, ssize_t chunk, FILE *f)
{
    if (chunk <= 0 || buf == NULL) return 0;
    if (f->_IO_read_ptr == f->_IO_read_end) return 0;

    ssize_t i;
    for (i = 0; i < chunk && (f->_IO_read_ptr[i] != '\0') && (f->_IO_read_ptr[i] != '\n'); i++) {
	buf[i] = f->_IO_read_ptr[i];
    }
    buf[i] = '\0';
    f->_IO_read_ptr += i+1;
    f->_IO_read_end -= i-1;
    if (i == 0) puts("ICI3");
    return i;
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
    // ssize_t ret = chunk;

    for (nbline = 0; ret != 0; nbline++) {
	ret = fgets(buf, chunk, f); // chunk 0x1000 is the weakness
	// ret = myfgets(buf, chunk, f); // chunk 0x1000 is the weakness

	// int overflow = 0;
	// for (overflow = 0; buf[overflow]; overflow++) ;
	// if (overflow >= chunk-1)  fprintf(stderr, "ERROR: The line is longer than 4098 bytes\n");
    }        
    rewind(f);
    return nbline;
}

ssize_t *LineLen(FILE *f, ssize_t nbline)
{
    // ssize_t nbline = CountLine(f);
    int chunk = 0x1000;
    char buf[chunk];
    char *ret = buf;

    ssize_t *ref = (ssize_t *)malloc(sizeof(ssize_t)*nbline);
    ERRALLOC(ref);

    long before;
    long after;
    ssize_t linelen = 0;
    
    for (ssize_t i = 0; ret != 0; i++) {
	before = ftell(f);
	ret = fgets(buf, chunk, f); // chunk 0x1000 is the weakness
	after = ftell(f);
	// ret = myfgets(buf, chunk, f); // chunk 0x1000 is the weakness
	linelen = after - before;

	ref[i] = linelen-1;
    }
    rewind(f);
    // for (int i = 0; i < nbline; i++) printf("line: %d - %ld\n", i+1, ref[i]);

    return ref;
}

#if 0
char **DataFromFile(FILE *f, ssize_t nbline, ssize_t **ref)
// char **DataFromFile(FILE *f, ssize_t nbline)
{
    int chunk = 0x1000;
    char buf[chunk];
    char *ret = buf;

    char **data = (char **)malloc(sizeof(char*)*nbline);
    ERRALLOC(data);

    *ref = (ssize_t *)malloc(sizeof(ssize_t)*nbline);
    ERRALLOC(data);

    long before;
    long after;
    size_t linelen;
    for (ssize_t i = 0; ret != 0; i++) {
	// char *before = &f->_IO_read_ptr[i];
	before = ftell(f);
	ret = fgets(buf, chunk, f); // chunk 0x1000 is the weakness
	// char *after = &f->_IO_read_ptr[i];
	after = ftell(f);
	// ret = myfgets(buf, chunk, f); // chunk 0x1000 is the weakness
	linelen = after - before;

	if (linelen <= 0) {
	    data[i] = (char *)malloc(sizeof(char)*1);
	    ERRALLOC(data[i]);
	    break;
	}
	*(ref[i]) = linelen;
	data[i] = (char *)malloc(sizeof(char)*(linelen + 1));
	ERRALLOC(data[i]);

	memcpy(data[i], buf, linelen);
    }
    rewind(f);
    return data;
}

#elif 1
char **DataFromFile(FILE *f, ssize_t nbline, ssize_t *ref)
{
    int chunk = 0x1000;
    char buf[chunk];
    char *ret = buf;

    char **data = (char **)malloc(sizeof(char*)*nbline);
    ERRALLOC(data);
    
    for (ssize_t i = 0; ret != 0; i++) {
	ret = fgets(buf, chunk, f); // chunk 0x1000 is the weakness

	data[i] = (char *)malloc(sizeof(char)*(ref[i] + 2));
	ERRALLOC(data[i]);

	#if 1
	int j;
	for (j = 0; j < ref[i]; j++) {
	    data[i][j] = buf[j];
	}
	data[i][j] = '\0';
	
	#elif 0
	memcpy(data[i], buf, ref[i]);
	#endif
    }
    rewind(f);
    return data;
}
#endif

# endif // PARSERLIB_IMPLEMENTATION
#endif // PARSERLIB_H_INCLUED
