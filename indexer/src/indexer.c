/*
BEGIN  C-Head v0.2


  -Von:
   Alexander Klupsch, ia3394


  -Dateiname:
   indexer.c


  -Sprache:
   ANSI-C


  -Main(0|1):
   1


  -Zweck/Beschreibung:
   Main


  -Programm:
   indexer


  -Besonderheiten:
   

END---------------------------------------------------------------------75->
*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include <sys/time.h>


#include <datei.h>
#include <param.h>
#include <hashtbl.h>
#include <alloc.h>


#if !defined(NDEBUG) 
/* Siehe: "alloc.c" */
extern int mallocCounter;
#endif

struct timeval tv1, tv2;

void printTimeResult(char *pre) {
   printf( "%s\n", pre);
   if ( tv2.tv_usec >= tv1.tv_usec ) {
    printf( "time: %lu.%lu\n", tv2.tv_sec - tv1.tv_sec, tv2.tv_usec - tv1.tv_usec );
   } else {
    printf( "time: %lu.%lu\n", tv2.tv_sec - tv1.tv_sec, tv1.tv_usec - tv2.tv_usec );
   }
}

/* 
  Ausgabe der Hilfe auf stdout.
*/
void printHelp(void) {
     printf("indexer\n");
     printf("indexer [optionen] indexdatei [optionen]\n");
     printf("optionen:\n");
     printf("-a|--add textdatei - \"textdatei\" dem Index hinzufuegen\n");
     printf("-s|--sub textdatei - \"textdatei\" aus dem Index entfernen\n");
     printf("\n");
}



/*
  Auswertung der erstellten Parameter Struktur.
  
  IN: param - Parameter Struktur
      argv - Programm Parameter
*/
int doParameterTest( Parameter param, char **argv ) {

    if ( param.help ) {
         printHelp();
         return (-1);
    }
    
    if ( param.fnotfound != 0 ) {
         printf("Die Datei \"%s\" wurde nicht gefunden.\n\n", 
                 argv[param.fnotfound]);
         return (-1);
    }
    
    if ( param.error ) {
         printf("Fehlerhafte Komandozeilensyntax.\n\n");         
         return (-1);
    }    
    
    if ( param.indexdatei == NULL ) {
         printf("Es wurde keine Indexdatei angegeben.\n\n");
         return (-1);
    }
    
    if ( ( ! param.indexexists ) && ( param.subListLen > 0 ) ) {
         printf("Die angegebene Indexdatei existiert nicht, enfernen nicht \
moeglich.\n\n");
         return (-1);
    }
    
    
    if ( (( param.addListLen == 0 ) && ( param.subListLen == 0 ) && 
          ( ! param.indexexists )) ) {
           printf("Es wurde eine nicht existierende Indexdatei angegeben. \
Suchen nicht moeglich.\n\n");
           return (-1);
    }
            
    
    return 0;
}



/*
  Fuehrt alle noetigen Schritte aus um eine neue Indexdatei aufzubauen.
  
  Besonderheit:
  Die printf(...) Meldungen sind auch Kommentare.
  
  IN: param - Parameter die zum aufbauen benoetgt werden.
  
  OUT: fehler(-1)|ok(0)
*/
int buildMode( Parameter param ) {
    int i;
    unsigned int fidx, flen;
    char *data = NULL;
    /* Ausschlusstabelle */
    Austab austab;
    HashTbl whtbl;

    
    /* Ausschlusstabelle aufbauen */
    if ( buildAustab( &austab ) < 0 ) {         
         return (-1);
    }

    
    /* Lokale Variable ist eigentlich nie NULL */
    if ( iniHTBL(&whtbl) < 0 ) {
         printf("Initialisierung der Hashtabelle fehlgeschlagen.\n\n");
         return (-1);
    }



    if ( param.indexexists ) {
gettimeofday( &tv1, NULL );
         printf("Lese alte Indexdatei ein.\n");
         if ( readWHTBL( &whtbl, param.indexdatei ) < 0 ) {
              printf("Einlesen der Indexdatei \"%s\" ist fehlgeschlagen.\
\n\n", param.indexdatei);
              clearHTBL( &whtbl );
              return (-1);
         }
gettimeofday( &tv2, NULL );printTimeResult("time to read old index file");
    }

    

if ( param.addListLen > 0 ) 
   gettimeofday( &tv1, NULL ); 

    /* Woerter aus allen per "--add" angegebenen Dateinamen zufuegen */
    for ( i = 0; i < param.addListLen; i++ ) {
          printf("Indiziere Datei \"%s\"\n", param.addList[i]);
          
          if ( getFNameIdxHTBL( &whtbl, param.addList[i], &fidx ) == 0 ) {
               printf("Datei \"%s\" ist schon indiziert. Add uebergangen.\
\n",  param.addList[i]);
               continue;
          }

          data = readFile( param.addList[i], &flen );

          if ( string2WHTBL( &whtbl, data, flen, param.addList[i], &austab )
               < 0 ) {
               printf("Indizieren der Datei \"%s\" ist fehlgeschlagen.\n\n",
                        param.addList[i]);
               data = FREE( data );
               clearHTBL( &whtbl );
               return (-1);
          }
          data = FREE( data );
    }

if ( param.addListLen > 0 ) {
    gettimeofday( &tv2, NULL );printTimeResult("time to add files");
}



if ( param.subListLen > 0 ) 
   gettimeofday( &tv1, NULL );

    /* Woerter aus allen per "--sub" angegebenen Dateinamen aus
       Index entfernen */
    for ( i = 0; i < param.subListLen; i++ ) {
          printf("Entferne datei \"%s\"\n",param.subList[i]);

          if ( getFNameIdxHTBL( &whtbl, param.subList[i], &fidx ) < 0 ) {
               printf("Datei \"%s\" ist nicht im Index. Sub nicht moeglich.\
\n", param.subList[i]);
               continue;
          }

          if ( subFNameRefHTBL( &whtbl, param.subList[i] ) < 0 ) {
               printf("Die Dateireferenz auf \"%s\" konnte nicht aus dem \
Index entfernt werden.\n\n", param.subList[i]);
               clearHTBL( &whtbl );
               return (-1);
          }
    }
if ( param.subListLen > 0 ) {
   gettimeofday( &tv2, NULL );printTimeResult("time to sub files");
}



gettimeofday( &tv1, NULL );
    
    printf("Baue Hashtabelle um eine Sortierte Liste zu speichern\n");
    
    /* buildHTBL  ->  sortierte Speicherung der Wortliste */
    if ( buildHTBL( &whtbl ) < 0 ) {
         printf("Das Aufbauen der Hashtabelle (zum sortierten speichern) \
ist fehlgeschlagen.\n\n");
         clearHTBL( &whtbl );
         return (-1);
    }

gettimeofday( &tv2, NULL );printTimeResult("time to build hash tabel");



gettimeofday( &tv1, NULL );    
    printf("Speichere die Daten in Indexdatei \"%s\"\n", param.indexdatei);
    
    /* Hashtabelle speichern */
    if ( saveHTBL( &whtbl, param.indexdatei ) < 0 ) {
         printf("Das Speichern der Indexdatei \"%s\" ist fehlgeschlagen.\
\n\n", param.indexdatei);
         clearHTBL( &whtbl );
         return (-1);
    }
    
gettimeofday( &tv2, NULL );printTimeResult("time to save table");

    printf("Lösche Hashtabelle aus Speicher\n");
gettimeofday( &tv1, NULL );    
    clearHTBL( &whtbl );
gettimeofday( &tv2, NULL );printTimeResult("time to free tabel");
    return 0;
}



/*
  Fuert alle noetigen Schritte aus um eine Indexdatei einzulesen,
  eine Hashtabelle aufzubauen und diese dann zum durchsuchen zur
  verfuegung zu stellen.
  
  Besonderheit:
  Die printf(...) Meldungen sind auch Kommentare.
  
  IN: param - Parameter Struktur mit allen benoetigten Informationen
  
  OUT: return fehler(-1)|ok(0)
*/
int searchMode( Parameter param ) {
    HashTbl htbl;

    
    /* Lokale Variable ist eigentlich nie NULL */
    if ( iniHTBL( &htbl ) < 0 ) { 
         printf("Initialisierung der Hash Tabelle fehlgeschlagen.\n\n");
         return (-1);
    }

    
    printf("Lese Indexdatei ein und baue Hashtabelle auf\n");

    
    /* Indexdatei einlesen(als Hashtabelle) */
    if ( readHTBL( &htbl, param.indexdatei ) < 0 ) {
         printf("Einlesen der Indexdatei ist fehlgeschlagen.\n\n");
         clearHTBL( &htbl );
         return (-1);
    }
    
    
    /* Ist die Indexdatei leer? */
    if ( htbl.used == 0 ) {
         printf("Es wurde eine leere Indexdatei eingelesen. Suchen nicht \
moeglich.\n\n");
         clearHTBL( &htbl );
         return (-1);
    }
    printf("%d verschiedene Worte in Indexdatei\n", htbl.used);

    
    /* User darf nun suchen */
    searchHTBL( &htbl );
    
    /* User hat gesucht --> Aufraeumen */
    clearHTBL( &htbl );
    
    return 0;
}



/*
  Main des Programms Indexer
  
  IN: argc - Anzahl Parameter.
      argv - Parameter
  
  OUT: return - exitcode
*/
int main(int argc, char **argv) {
    Parameter param;
    
    /* Programm Parameter auswerten und in eigene Struktur umwandeln */
    param = evalParams( argc, argv );
printParam(param);
    /* Parameter Struktur auswerten, wenn ok Verarbeitung starten */
    if ( doParameterTest(param, argv) == 0 ) {
    
         if ( (( param.addListLen == 0 ) && ( param.subListLen == 0 )) ) {
              printf("searchMode...\n");fflush(stdout);
              searchMode( param );
         } else {         
              printf("buildMode...\n");fflush(stdout);
              buildMode( param );
         }
         
    }

    cleanupParameter( &param );


#if !defined(NDEBUG)
    printf("mallocCounter: %d\n", mallocCounter);    
#endif


    return 0;
}


/*EOF*/
