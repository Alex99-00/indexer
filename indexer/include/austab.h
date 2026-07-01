#ifndef __AUSTAB_H
#define __AUSTAB_H


#define FAUSTAB "austab.txt"


/*

 iso-8859-1:
 
 0-9 = 48-57
 A-K = 65-90
 a-k = 97-122
 
 Ä = 196
 ä = 228
 
 Ö = 214
 ö = 246
 
 Ü = 220
 ü = 252
 
 ß = 223

*/


#define AUSTABDATA " \
0-47 \
58-64 \
91-96 \
123-128 \
129-195 \
197-213 \
215-219 \
221-222 \
224-227 \
229-245 \
247-251 \
253-255 \
"



typedef struct {
        int  tbl[256];     /* Boolsches Array */
}Austab;


int buildAustab( Austab *atbl );
void printAustab( Austab *atbl );

#else
#error "Multiple include"
#endif

/*EOF*/
