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
FILE   *OpenFile(const char *FileName);
ssize_t SizeOfFile(FILE *f);
ssize_t CountLine(FILE *f);
char  **GetCpyDataFile(const char *FileName, file_t *ft);
char ***ParseCpyDataFile(const char *FileName, char delim);
int     FreeCpyDataFile(file_t *ft);
int     FreeCpyParsedData(char ***parse);

char ***ParseDataFile(const char *FileName, char delim, file_t *ft);
int FreeParsedData(char ***pdata, file_t *ft);


# ifdef PARSERLIB_IMPLEMENTATION


#  if !defined(VAR_OPT)
FILE *OpenFile(const char *FileName)
{
    FILE *f = NULL;

    f = fopen(FileName, "r");
    if (f == NULL) {
	fprintf(stderr, "ERROR: %s: \"%s\"\n", strerror(errno), FileName);
	return NULL;
    }

    return f;
}

ssize_t SizeOfFile(FILE *f)
{
    struct stat st;

    if (fstat(fileno(f), &st) < 0) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
        fclose(f);
        return 0;
    }

    return st.st_size;
}

ssize_t CountLine(FILE *f)
{
    int fd = fileno(f);
    ssize_t len = SizeOfFile(f);
    char *start = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);

    if (start == MAP_FAILED) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
	fclose(f);
	return 0;
    }

    char *buf = start;
    char *end = buf + len;
    ssize_t nbline = 0;

    while (buf < end) {
	char *ret = memchr(buf, '\n', end - buf);
	if (ret) {
	    nbline++;
	    buf = ret+1;
	} else break;
    }

    munmap(start, len);

    return nbline;
}

char **GetCpyDataFile(const char *FileName, file_t *ft)
{
    FILE *f = OpenFile(FileName);
    if (!f) return NULL;

    ft->fd = fileno(f);
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
                    ft->fd,
                    0);
    fclose(f);

    if (ft->rdata == MAP_FAILED) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
	return NULL;
    }

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

            ft->data[nbline++] = p;
            p = nl + 1;
	} else {
	    ft->data[nbline++] = p;
	    break;
	}
    }
    return ft->data;
}

char ***ParseCpyDataFile(const char *FileName, char delim)
{
    file_t __ft;
    if (!GetCpyDataFile(FileName, &__ft)) return NULL;

    char ***pdata = (char ***)malloc(sizeof(char **)*(__ft.nbline+1));
    TESTMALLOC(pdata);
    pdata[__ft.nbline] = NULL;

    for (ssize_t i = 0; i < __ft.nbline; i++) {
	ssize_t nbword = 0;
        ssize_t j = 0;

        for (j = 0; __ft.data[i][j] != '\0'; j++) {
	    if (__ft.data[i][j] == delim) nbword++;
	}
	nbword++;

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
    free(__ft.data);
    return pdata;
}

int FreeCpyDataFile(file_t *ft)
{
    if (ft->data == NULL) {
	fprintf(stdout, "INFO: Can not free data from file because parameter == NULL\n");
	return -1;
    }
    free(ft->data);
    int res;

    if ((res = munmap(ft->rdata, ft->size)) < 0)
	fprintf(stderr, "ERROR: %s\n", strerror(errno));
    
    return res;
}

int FreeCpyParsedData(char ***parse)
{
    if (parse == NULL) {
	fprintf(stdout, "INFO: Can not free data from file because parameter == NULL\n");
	return -1;
    }

    for (ssize_t i = 0; parse[i]; i++) free(parse[i]);
    free(parse);

    return 0;
}

#  endif


#  if defined(VAR_OPT)
file_t __ft = {.size    = 0,
               .rdata   = NULL,
               .nbline  = 0,
               .linelen = NULL,
               .data    = NULL,
               .nbword  = NULL,
               .fd      = 0,
               };

FILE *OpenFile(const char *FileName)
{
    FILE *f = NULL;

    f = fopen(FileName, "r");
    if (f == NULL) {
	fprintf(stderr, "ERROR: %s: \"%s\"\n", strerror(errno), FileName);
	return NULL;
    }
    __ft.fd = fileno(f);

    return f;
}

ssize_t SizeOfFile(FILE *f)
{
    struct stat st;
    (void)f;
    
    if (fstat(__ft.fd, &st) < 0) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
        close(__ft.fd);
        return 0;
    }
    __ft.size = st.st_size;

    return __ft.size;
}

ssize_t CountLine(FILE *f)
{
    if ((__ft.fd == 0) && (f != NULL))  __ft.fd = fileno(f);
    if (__ft.size == 0) SizeOfFile(f);
    
    char *start = mmap(NULL, __ft.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, __ft.fd, 0);

    if (start == MAP_FAILED) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
	fclose(f);
	return 0;
    }
    __ft.rdata = start;    

    char *buf = start;
    char *end = buf + __ft.size;
    fclose(f);

    ssize_t nbline = 0;

    while (buf < end) {
	char *ret = memchr(buf, '\n', end - buf);
	if (ret) {
	    nbline++;
	    buf = ret+1;
	} else break;
    }
    
    __ft.nbline = nbline;
    
    return nbline;
}

char **GetCpyDataFile(const char *FileName, file_t *ft)
{
    FILE *f = OpenFile(FileName);
    if (!f) return NULL;
    
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

            __ft.linelen[nbline] = nl - p;
            ft->data[nbline++] = p;
            p = nl + 1;
	} else {
	    ft->data[nbline++] = p;
	    break;
	}
    }
    return ft->data;
}


char ***ParseCpyDataFile(const char *FileName, char delim)
{
    if (!GetCpyDataFile(FileName, &__ft)) return NULL;
 
    char ***pdata = (char ***)malloc(sizeof(char **)*(__ft.nbline+1));
    TESTMALLOC(pdata);
    pdata[__ft.nbline] = NULL;

    // This is the differents without VAR_OPT
    __ft.nbword = (ssize_t *)malloc(sizeof(ssize_t)*(__ft.nbline));
    TESTMALLOC(__ft.nbword);

    for (ssize_t i = 0; i < __ft.nbline; i++) {
	ssize_t nbword = 0;
        ssize_t j = 0;

        for (j = 0; __ft.data[i][j] != '\0'; j++) {
	    if (__ft.data[i][j] == delim) nbword++;
	}
	nbword++;

	__ft.nbword[i] = nbword;

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
    free(__ft.data);

    return pdata;
}

int FreeCpyDataFile(file_t *ft)
{
    if (ft->data == NULL) {
	fprintf(stdout, "INFO: Can not free data from file because parameter == NULL\n");
	return -1;
    }
    free(ft->data);
    free(__ft.linelen);
    int res;

    if ((res = munmap(ft->rdata, ft->size)) < 0)
	fprintf(stderr, "ERROR: %s\n", strerror(errno));

    return res;
}

int FreeCpyParsedData(char ***parse)
{
    if (parse == NULL) {
	fprintf(stdout, "INFO: Can not free data from file because parameter == NULL\n");
	return -1;
    }

    free(__ft.linelen);
    free(__ft.nbword);
    for (ssize_t i = 0; i < __ft.nbline; i++) free(parse[i]);
    free(parse);

    int res = 0;
    if((res = munmap(__ft.rdata, __ft.size)) < 0) {
	fprintf(stderr, "ERROR: %s\n", strerror(errno));
	return res;
    }

    return res;
}

#  endif



char ***ParseDataFile(const char *FileName, char delim, file_t *ft)
{
    FILE *f = NULL;
    f = fopen(FileName, "r+");
    if (f == NULL) {
	fprintf(stderr, "ERROR: %s: \"%s\"\n", strerror(errno), FileName);
	return NULL;
    }
    // Open the file

    struct stat st;
    ft->fd = fileno(f);
    ft->linelen = NULL;

    if (fstat(ft->fd, &st) < 0) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
        fclose(f);
        return 0;
    }
    ft->size = st.st_size;
    // Get the size of the file

    ft->rdata = mmap(NULL, ft->size, PROT_READ | PROT_WRITE, MAP_SHARED, ft->fd, 0);
    if (ft->rdata == MAP_FAILED) {
	fprintf(stderr, "ERROR: %s, %d\n", strerror(errno), __LINE__);
	fclose(f);
	return 0;
    }
    // mmap a link to the file
    
    char *buf = ft->rdata;
    char *end = buf + ft->size;
    ft->nbline = 0;
    while (buf < end) {
	char *nl = memchr(buf, '\n', end - buf);
	if (nl) {
	    ft->nbline++;
	    buf = nl+1;
	} else break;
    }
    // Get the number of line of the file

    ft->nbword = (ssize_t *)malloc(sizeof(ssize_t)*ft->nbline);
    TESTMALLOC(ft->nbword);

    fclose(f);
    
    char ***pdata = (char ***)malloc(sizeof(char **)*(ft->nbline+1));
    TESTMALLOC(pdata);
    pdata[ft->nbline] = NULL;
    
    ssize_t k = 0;
    for (ssize_t i = 0; i < ft->nbline; i++) {
	ssize_t nbword = 0;
	ssize_t j = 0;
	
	for (j = 0; ft->rdata[k] != '\n'; j++, k++)
	    if (ft->rdata[k] == delim) nbword++;
	if (ft->rdata[k] == '\n') k++;

	nbword++;
	
        pdata[i] = (char **)malloc(sizeof(char *)*(nbword+1));
	TESTMALLOC(pdata[i]);
	pdata[i][nbword] = NULL;
	ft->nbword[i] = nbword;
	
	nbword = 0;
	end = &ft->rdata[(k-1)-j] + j;
	buf = &ft->rdata[(k-1)-j];
	for (j = 0; buf < end+1; j++) {
	    char *nw = memchr(buf, delim, end - buf);

	    if (nw) {
		pdata[i][j] = buf;
		buf = nw + 1;
	    } else {
		pdata[i][j] = buf;
		break;
	    }
	}
    }
    
    return pdata;
}

int FreeParsedData(char ***pdata, file_t *ft)
{
    // for (ssize_t i = 0; i < ft->nbline; i++) {
    for (ssize_t i = 0; pdata[i]; i++) {
	free(pdata[i]);
    }
    free(pdata);
    free(ft->nbword);
    return munmap(ft->rdata, ft->size);
}


# endif // PARSERLIB_IMPLEMENTATION
#endif // PARSERLIB_H_INCLUED
