/*
BEGIN  C-Head v0.2


  -Von:
   Alexander Klupsch, ia3394


  -Dateiname:
   datei.c


  -Sprache:
   ANSI-C


  -Main(0|1):
   0


  -Zweck/Beschreibung:
   Funktionen die auf/mit Dateien arbeiten.


  -Programm:
   indexer


  -Besonderheiten:
   Datentyp "struct stat" wird als structStat neu definiert.
   Funktion stat(...) wird ueber Stat(...) aufgerufen.
   -> MSVC kennt mit Parameter "/Za" "stat(...)" und "struct stat" nicht.?

END---------------------------------------------------------------------75->
*/


#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>

#include <alloc.h>
#include <datei.h>



#if defined(_WIN32)
    typedef struct _stat structStat;
#else
    typedef struct stat structStat;
#endif


int Stat( char *fname, structStat *fstatus ) {
#if defined(_WIN32)
    return (_stat( fname, fstatus ));
#else
    return (stat( fname, fstatus ));
#endif
}



/* 
 Existiert Regulaere-Datei "fname" ?
 -> Nicht Directory, nicht Geraetedateien!
 
 IN : fname - path/name der zu testenden Datei
 OUT: true(1)|false(0)
*/
int findFile ( char *fname ) {
    FILE *fz = NULL;
    structStat fstatus;
    
    if ( fname == NULL )
         return 0;

    /* Muss in readFile(...) funktionieren und ist ein guter Test */
    if ( Stat(fname, &fstatus) < 0 ) {
         return 0;
    }

/* Abfragen ob Datei eine Regulaere Datei ist */
#if defined(_WIN32)
    if ( ! (_S_IFREG & fstatus.st_mode) )
             return 0;	
#else
    if ( ! (S_ISREG( fstatus.st_mode )) )
         return 0;
#endif

    /* Darf "ich" die oeffnen? 
       -> Ist eine fuer "mich" existente Datei */
    fz = fopen ( fname, "rb" );
     
    if ( fz != NULL ) {
         (void)fclose( fz );
         return 1; /* OK */
    } else {         
         return 0; 
    }
    
}



/*
  Liesst Datei "fname" ein und speichert die Laenge in "lenOfFile"
  
  IN:  fname - pfad/name der einzulesenden Datei
       lenOfFile -  zum ablegen der Laenge des eingelesenen Dateiinhalts
       
  OUT: return - Dateiinhalt
       lenOfFile - Laenge des eingelesenen Dateiinhalts
*/
char * readFile( char *fname, unsigned int *lenOfFile ) {

    FILE *fz=NULL;
    structStat fstatus;    
    size_t len=0;
    char *data=NULL;

    assert( lenOfFile != NULL );

    *lenOfFile = 0;
        
    if ( ! findFile(fname) ) {
         printf("Datei \"%s\" nicht gefunden oder keine Regulaere Datei.\n", fname);
         return data;
    }
    
    fz = fopen(fname, "rb");
    
    if (fz == NULL) {
        printf("Datei \"%s\" konnte nicht geoeffnet werden.\n", fname);
        return data;
    }
    
    if (Stat(fname, &fstatus) < 0) {/* -> findFile(fname) */
        printf("Fuer Datei \"%s\" konnte kein Status ermittelt werden.\n",
        fname);
        return data;
    }
    
    /* Speicher in der Groesse der Datei besorgen */
    data = MALLOC( (size_t)(fstatus.st_size + 1) );
    
    /* Datei komplet einlesen, laenge merken 
       -> vergleich mit fstatus.st_size */
    len = fread(data, 1, (size_t)fstatus.st_size, fz);
    (void)fclose(fz); /* Datei verarbeitet */
    data[len]='\0'; /* Zeichenkette Terminieren */
    
    /* Ist alles eingelesen worden? */
    if (len != fstatus.st_size) {
        data = FREE(data);
        fprintf(stderr, "Dateiinhalt von \"%s\" konnte nicht eingelesen \
werden.\n", fname);
        return data;
    }
    
    /* Laenge zurueckgeben */
    *lenOfFile = ((unsigned int)len);
        
    return data;
    
}

/*EOF*/
