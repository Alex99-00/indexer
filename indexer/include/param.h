
#ifndef __PARAM_H
#define __PARAM_H

typedef struct {
        char   *indexdatei;
        int    indexexists;
        char   **addList;
        int    addListLen;
        char   **subList;
        int    subListLen;
        int    help;
        int    error;
        int    fnotfound;
}Parameter;


void cleanupParameter( Parameter *param );

Parameter evalParams( int argc, char ** argv);

void printParam( Parameter param );


#else
#error "Multiple include"
#endif


/*EOF*/
