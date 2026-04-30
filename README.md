# Parserlib

Simple library that is able to parse any text or csv file pretty quickly.

## Ready to start

```console
$ make 
$ ./main test.txt
```

## Exemple of perf

- Classic algo:
0s 181ms -> to parse sqlite3.txt witch is size is 7.5Mo
	total heap usage: 1,464,919 allocs, 1,242,042 frees, 34,957,617 bytes allocated
4s 925ms -> 677Mo with line length of 65 bytes
	total heap usage: 31,739,909 allocs, 21,159,940 frees, 1,756,283,906 bytes allocated
5s 875ms -> 1.3Go with line length of 500 bytes
	total heap usage: 7,345,735 allocs, 4,897,158 frees, 2,664,060,042 bytes allocated

- My:
0s 118ms -> to parse sqlite3.txt witch is size is 7.5Mo
	total heap usage: 222,879 allocs, 222,879 frees, 20,035,448 bytes allocated
3s 186ms -> 677Mo with line length of 65 bytes
	total heap usage: 10,579,971 allocs, 10,579,971 frees, 338,559,456 bytes allocated
4s 153ms -> 1.3Go with line length of 500 bytes
	total heap usage: 2,448,579 allocs, 2,448,579 frees, 78,354,912 bytes allocated
