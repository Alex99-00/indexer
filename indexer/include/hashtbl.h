#ifndef __HASHTBL_H
#define __HASHTBL_H



 
typedef struct {
        unsigned int fidx; /* Index in Dateinamenliste*/
        unsigned int row;  /* Zeile */
        unsigned int col;  /* Spalte */
}Element;




typedef struct _Node Node;
struct _Node {
        Element elem;  /* Daten */
        Node    *next; /* Naechster Knoten */
};



typedef struct {
        Node *first; /* Erster Knoten */
        Node *last;  /* Letzter Knoten */
        char *word;  /* Wort fuer diese Knotenliste*/
}FirstNode;



typedef struct {
        FirstNode     *tbl; /* Array von FirstNode(s) */
        unsigned int  size; /* Gesamtgroesse des tbl Arrays */
        unsigned int  used; /* Belegte Plaetze 
                              !(In feritger HashTbl. Belegungszustand)! */
        char          **fnames;  /* Array mit Dateinamen */
        unsigned int  files;     /* Groesse des "fnames" Array */
}HashTbl;


/* Maximale Wortgroesse */
#define MAXWORDLEN (256)


/* Maximale [Pfad]Dateinamen-laenge. */
#define MAXFNAMELEN (1024)


/* Maximale laenge einer int Zahl als Zeichenkette */
#define MAXLENOFINTASSTRING (10)


/* Maximale Groesse einer Zeile in der Indexdatei */
/* 256+1024+(2*10)+(3*sizeof(char)) = 1303 */
#define MAXBUFINDEXLINE (MAXWORDLEN+MAXFNAMELEN+(2*MAXLENOFINTASSTRING)+(3*(sizeof(char))))


/* Automatische Groessenaenderung von HashTbl.tbl */
#define AUTO_RESIZE_SIZE (16384)


/* Minimalgroesse einer Hashtabelle */
#define MINHTBLSIZE (5)



int iniHTBL( HashTbl *htbl );
#include <austab.h>
int string2WHTBL( HashTbl *whtbl, char *data, unsigned int dataLen,
                    char *aktFileName, Austab *austab );
int buildHTBL( HashTbl *srchtbl );
int subFNameRefHTBL( HashTbl *htbl, char *fname );
int saveHTBL( HashTbl *htbl, char *fname );
int readHTBL( HashTbl *htbl, char *fname );
int readWHTBL( HashTbl *whtbl, char *fname );
void searchHTBL( HashTbl *htbl );
void clearHTBL( HashTbl *htbl );
int getFNameIdxHTBL( HashTbl *htbl, char *fname, unsigned int *fidx );



/*
void clearFirstNode( FirstNode *fnode );
void addFNameHTBL( HashTbl *htbl, char *fname );
void subFNameHTBL( HashTbl *htbl, unsigned int idx );
void printFNamesHTBL( HashTbl *htbl );
void finalizeSizeWHTBL( HashTbl *htbl );
void resizeHTBL( HashTbl *htbl );
Node * newNode( void );
void addWordWHTBL( HashTbl *htbl, char *word, unsigned int fidx,  
                 unsigned int row,  unsigned int col);
void printHTBL( HashTbl *htbl );
*/



#else
#error "Multiple include"
#endif



/*EOF*/
