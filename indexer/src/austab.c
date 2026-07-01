/*
BEGIN  C-Head v0.2


  -Von:
   Alexander Klupsch, ia3394


  -Dateiname:
   austab.c


  -Sprache:
   ANSI-C


  -Main(0|1):
   0


  -Zweck/Beschreibung:
   Aufbau einer Ausschlusstabelle fuer Datentyp "char".


  -Programm:
   indexer


  -Besonderheiten:
   

END---------------------------------------------------------------------75->
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <alloc.h>
#include <datei.h>
#include <austab.h>




/*
  Gibt den Inhalt einer Ausschlusstabelle aus.
  
  IN: atbl - Zeiger auf eine Ausschlusstabelle
  
  OUT: atbl - Initialisierte Auschlusstabelle
*/
void printAustab( Austab *atbl ) {
     int i;
     
     if ( atbl == NULL ) {
          printf("Ausschlusstabelle fehlerhaft. Ausgabe nicht moeglich.\n");
          return ;
     }
     
     for ( i=0; i<=255; i++) {
           printf("%d: %d : %c\n", i, atbl->tbl[i], i);
     }
     printf("\n");
}


static void resetAustab( Austab *atbl ) {
     int i;
     
     assert( atbl != NULL );
     
     /* reset Austab -> alle Elemente auf (false=0) */
     for ( i = 0; i < 256; i++ ) {
           atbl->tbl[i] = 0;
     }
}


/*
  Baut die Ausschlusstabelle auf.
  Die Daten die angeben welche Zeichen Auszuschliessen sind, sind
  in "austab.h" als "AUSTABDATA" definiert. Da die Daten fest
  im Programm stehen, ist ein Fehlerfall eigentlich auszuschliessen.
  
  IN: atbl - Zeiger auf eine ATabelle
  
  OUT: atbl - Mit Daten gefuellte ATabelle 
*/
int buildAustab( Austab *atbl ) {
     char *ptr;
     int i, zeichen, scanRet;
     unsigned int von, bis;
     
     if ( atbl == NULL )
          return (-1);
     
     resetAustab( atbl );		 

     ptr =  AUSTABDATA;
     
     /* Zeilenweise einlesen */
     while ( (scanRet = sscanf( ptr, "%u-%u\n%n", &von, &bis, &zeichen ))
             != EOF ) {

              ptr += zeichen;
   
            
              if ( (von >= 255) || (bis > 255) ) {
                   fprintf( stderr, "Ausschlusstabelle konnte nicht \
aufgebaut werden.\nDateninput fehlerhaft.\n" );
                   return (-1);
              }
   
              /* Bereich von -> bis auf true=1 setzen */
              switch (scanRet) {
                 case 2:
                  for ( i = von; i <= bis; i++ ) {
                        atbl->tbl[i] = 1;
                  }
                 break;
   
       
                 default: 
                  fprintf( stderr, "Ausschlusstabelle konnte nicht \
aufgebaut werden.\nDateninput fehlerhaft.\n" );                  
                  return (-1);
                 break;
              }
     }
     
     return 0;
}

/*EOF*/
