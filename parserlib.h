#ifndef PARSERLIB_H_INCLUED
#define PARSERLIB_H_INCLUED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

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
    char    *rdata;	// Row Data
    ssize_t size;	// Size of file
    char    **data;	// Data parsed by line
    ssize_t nbline;	// number of line
}file_t;

// Declaration
ssize_t strsize(char *t);
FILE *OpenFile(const char *FileName);
ssize_t SizeOfFile(FILE *f);
ssize_t CountLine(FILE *f);
ssize_t *LineLen(FILE *f, ssize_t nbline);
char **DataFromFile(FILE *f, ssize_t nbline);


# ifdef PARSERLIB_IMPLEMENTATION

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

ssize_t CountLine(FILE *f)
{
    int fd = fileno(f);
    ssize_t len = SizeOfFile(f);
    char *start = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    ERRALLOC(start);
    char *buf = start;

    char *end = buf + len;
    ssize_t nbline = 0;
    
    while (buf < end) {
	char *ret = memchr(buf, '\n', end-buf);
	if (!ret) break;
	nbline++;
	buf = ret+1;
    }
    munmap(start, len);
    return nbline;
}


char **DataFromFile(FILE *f, ssize_t nbline)
{
    int fd = fileno(f);
    ssize_t len = SizeOfFile(f);

    char *data = mmap(NULL,
                        len,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE,
                        fd,
                        0);
    fclose(f);

    if (data == MAP_FAILED) return NULL;

    char **lines = mmap(NULL,
                        sizeof(char *)*(nbline),
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        -1,
                        0);

    if (lines == MAP_FAILED) {
        munmap(data, len);
        return NULL;
    }
    ssize_t count = 0;
    char *p = data;
    char *end = data + len;

    if (len > 0)
        lines[count++] = p;

    while (p < end && count < nbline) {
        char *nl = memchr(p, '\n', end - p);
        if (!nl) break;

        *nl = '\0';
        p = nl + 1;

        if (p < end)
            lines[count++] = p;
    }
    return lines;
}


# endif // PARSERLIB_IMPLEMENTATION
#endif // PARSERLIB_H_INCLUED
