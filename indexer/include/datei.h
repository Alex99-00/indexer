
#ifndef __DATEI_H
#define __DATEI_H


char * readFile( char *fname, unsigned int *lenOfFile );


int findFile ( char * fname );

#else
#error "Multiple include"
#endif

/*EOF*/
