#ifndef PARSERLIB_H_INCLUED
#define PARSERLIB_H_INCLUED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // Used for: strerror
#include <errno.h>
#include <sys/mman.h>  // Used for: mmap and munmap
#include <sys/types.h> // Used for: fstat 
#include <sys/stat.h>  // Used for: fstat
#include <unistd.h>    // Used for: fstat

// #define VAR_OPT


#define TESTMALLOC(x)                                                          \
do                                                                             \
{                                                                              \
    if (x == NULL) {                                                           \
        fprintf(stderr, "ERROR: %s, ligne : %d\n", strerror(errno), __LINE__); \
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
} while(0)


typedef struct
{
    ssize_t   size;	 // Size of file
    char     *rdata;	 // Row Data
    ssize_t   nbline;	 // number of line
    ssize_t  *linelen;   // length of the line each lines
    char    **data;	 // Data parsed by line
    ssize_t  *nbword;    // number of words each lines
    int       fd;	 // file directory
}file_t;


// Declaration
FILE *OpenFile(const char *FileName);
ssize_t SizeOfFile(FILE *f);
ssize_t CountLine(FILE *f);
char **DataFromFile(const char *FileName, file_t *ft);
int FreeDataFromFile(file_t *ft);
int freeparse(char ***parse);


# ifdef PARSERLIB_IMPLEMENTATION


#  if defined(VAR_OPT)
file_t __ft = {.size    = 0,
               .rdata   = NULL,
               .nbline  = 0,
               .linelen = NULL,
               .data    = NULL,
               .nbword  = NULL,
               .fd      = 0,
               };
#  endif

FILE *OpenFile(const char *FileName)
{
    FILE *f = NULL;
    f = fopen(FileName, "r");
    if (f == NULL) {
	fprintf(stderr, "ERROR: %s: \"%s\"\n", strerror(errno), FileName);
	return NULL;
    }
    #if defined(VAR_OPT)
    __ft.fd = fileno(f);
    #endif
    return f;
}


ssize_t SizeOfFile(FILE *f)
{
    struct stat st;
    
    #if defined(VAR_OPT)
    if (fstat(__ft.fd, &st) < 0) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
        close(__ft.fd);
        return 0;
    }
    __ft.size = st.st_size;
    return __ft.size;
    #endif
    
    if (fstat(fileno(f), &st) < 0) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
        fclose(f);
        return 0;
    }
    return st.st_size;
}

ssize_t CountLine(FILE *f)
{
#  if defined(VAR_OPT)
    if ((__ft.fd == 0) && (f != NULL))  __ft.fd = fileno(f);
    if (__ft.size == 0) SizeOfFile(f);
    
    char *start = mmap(NULL, __ft.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, __ft.fd, 0);
    __ft.rdata = start;    
    char *buf = start;
    char *end = buf + __ft.size;
    fclose(f);
#  else
    int fd = fileno(f);
    ssize_t len = SizeOfFile(f);

    char *start = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);

    if (start == MAP_FAILED) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
	return 0;
    }

    char *buf = start;
    char *end = buf + len;
#  endif

    ssize_t nbline = 0;

    while (buf < end) {
	char *ret = memchr(buf, '\n', end - buf);
	if (ret) {
	    nbline++;
	    buf = ret+1;
	} else break;
    }
    
#  if defined(VAR_OPT)
    __ft.nbline = nbline;
#  else
    munmap(start, len);
#  endif
    
    return nbline;
}

char **DataFromFile(const char *FileName, file_t *ft)
{
    FILE *f = OpenFile(FileName);
    if (!f) return NULL;
    
#  if defined(VAR_OPT)

    ft->nbline = CountLine(f);
    ft->size = __ft.size;
    if (__ft.size == 0) {
	fprintf(stdout, "INFO: File is empty\n");
	munmap(__ft.rdata, __ft.size);
	fclose(f);
	return NULL;
    }
    ft->rdata = __ft.rdata;
    __ft.linelen = (ssize_t *)malloc(sizeof(ssize_t)*ft->nbline);
    TESTMALLOC(__ft.linelen);

#  else
    int fd = fileno(f);

    ft->data = NULL;
    ft->size = SizeOfFile(f);
    
    if (ft->size == 0) {
	fprintf(stdout, "INFO: File is empty\n");
	fclose(f);
	return NULL;
    }
    ft->nbline = CountLine(f);

    ft->rdata = mmap(NULL,
                    ft->size,
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE,
                    fd,
                    0);
    fclose(f);

    if (ft->rdata == MAP_FAILED) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
	return NULL;
    }
#  endif

    ft->data = (char **)malloc(sizeof(char *)*(ft->nbline+1));
    TESTMALLOC(ft->data);
    ft->data[ft->nbline] = NULL;

    ssize_t nbline = 0;
    char *p = ft->rdata;
    char *end = ft->rdata + ft->size;
        
    while ((p < end)) {
        char *nl = memchr(p, '\n', end - p);
	if (nl) {
            *nl = '\0';
	    if (*(nl-1) == '\r') *(nl-1) = '\0'; // remove '\r'

#  if defined(VAR_OPT)
            __ft.linelen[nbline] = nl - p;
#  endif
            ft->data[nbline++] = p;
            p = nl + 1;
	} else {
	    ft->data[nbline++] = p;
	    break;
	}
    }
    return ft->data;
}

int FreeDataFromFile(file_t *ft)
{
    if (ft->data == NULL) {
	fprintf(stdout, "INFO: Can not free data from file because parameter == NULL\n");
	return 0;
    }
    int res;
    if ((res = munmap(ft->rdata, ft->size)) < 0) {
	fprintf(stderr, "ERROR: %s\n", strerror(errno));
    }
    free(ft->data);

#  if defined(VAR_OPT)
    free(__ft.linelen);

#  endif
    
    return res;
}

char ***parse(const char *FileName, char delim)
{
#  if !defined(VAR_OPT)
    file_t __ft;
#  endif

    if (!DataFromFile(FileName, &__ft)) return NULL;

    char ***pdata = (char ***)malloc(sizeof(char **)*(__ft.nbline+1));
    TESTMALLOC(pdata);
    pdata[__ft.nbline] = NULL;

#  if defined(VAR_OPT)
    __ft.nbword = (ssize_t *)malloc(sizeof(ssize_t)*(__ft.nbline));
    TESTMALLOC(__ft.nbword);
#  endif

    for (ssize_t i = 0; i < __ft.nbline; i++) {

	ssize_t nbword = 0;
        ssize_t j = 0;

        for (j = 0; __ft.data[i][j] != '\0'; j++) {
	    if (__ft.data[i][j] == delim) nbword++;
	}
	nbword++;

#  if defined(VAR_OPT)	
	__ft.nbword[i] = nbword;
#  endif

        pdata[i] = (char **)malloc(sizeof(char *)*(nbword+1));
	TESTMALLOC(pdata[i]);
	pdata[i][nbword] = NULL;

	char *end = __ft.data[i] + j;
	char *p   = __ft.data[i];
	
	nbword = 0;

	while (p < end+1) {
	    char *nw = memchr(p, delim, end - p);

	    if (nw) {
		*(nw) = '\0';
		pdata[i][nbword++] = p;
		p = nw + 1;
	    } else {
		pdata[i][nbword++] = p;
		break;
	    }
	}
    }
#  if !defined(VAR_OPT)
    free(__ft.data);
#  endif
    return pdata;
}

#  if defined(VAR_OPT)
int freevarspe(char ***parse)
{
    
    free(__ft.linelen);
    free(__ft.nbword);
    free(__ft.data);

    for (ssize_t i = 0; i < __ft.nbline; i++) free(parse[i]);
    free(parse);

    int res = 0;
    if((res = munmap(__ft.rdata, __ft.size)) < 0) {
	fprintf(stderr, "ERROR: %s\n", strerror(errno));
	return res;
    }
    return res;
}
#endif

int freeparse(char ***parse)
{
    if (parse == NULL) {
	fprintf(stdout, "INFO: Can not free data from file because parameter == NULL\n");
	return -1;
    }

#  if defined(VAR_OPT)
    return freevarspe(parse);

#  else
    for (ssize_t i = 0; parse[i]; i++) free(parse[i]);
    free(parse);
    return 0;

#  endif

}


# endif // PARSERLIB_IMPLEMENTATION
#endif // PARSERLIB_H_INCLUED
