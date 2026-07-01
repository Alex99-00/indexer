/*
BEGIN  C-Head v0.2


  -Von:
   Alexander Klupsch, ia3394


  -Dateiname:
   hashtbl.c


  -Sprache:
   ANSI-C


  -Main(0|1):
   0


  -Zweck/Beschreibung:
   Aufbau und Verwaltung einer Hash Tabelle.


  -Programm:
   indexer


  -Besonderheiten:
   In den Beschreibungen wird eine Hashtabelle auch als HTBL bezeichnet
   und eine Hashtabellen Struktur die als Wortliste benutzt wird mit WHTBL.

END---------------------------------------------------------------------75->
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <alloc.h>
#include <hashtbl.h>






/*------------------------------------------------------------------------*/
/*
  Berechnet ob Parameter "val" eine Primzahl ist.
  
  IN: val - zu testende Zahl
  
  OUT: return - true(1)|false(0) 
*/
int isPrim(unsigned int val) {    
    unsigned int t;
    
    /* Spezialfälle */
    switch (val) {
       case 1: return 1;
       case 2: return 1;
       case 3: return 1;
    }
    
    /* Ist "val" gerade */
    if ((val % 2) == 0) {
           return 0;
    } else {
    
       t = 3;/* Kleinster ungerader Teiler*/
       
       while ( t < val ) { /* Solange (t<val) Teiler suchen */
             if ((val % t) == 0) {
                 return 0; /* Teiler gefunden -> "val" keine Primzahl */
             } else {
                 t += 2;
             }
       }/*while*/
       
    }/*else*/
    
    return 1;/* Kein Teiler fuer "val" gefunden -> "val" ist Primzahl */
}


/*
  Berechnet die naechstgroessere Primzahl zu "val".
  
  IN: val - Basiswert zu der die naechste Primzahl gesucht wird
  
  OUT: return - Primzahl
*/
unsigned int calcNextPrim(unsigned int val) {


    switch (val) {    /* vorsichtig, nicht zu klein */
       case 1: return MINHTBLSIZE;
       case 2: return MINHTBLSIZE;
       case 3: return MINHTBLSIZE;
    }
    
    ++val;/* Naechste Primzahl -> diese kann schon Prim. sein(per Zufall) */
    
    if ((val % 2) == 0) 
         ++val;         /* ungerade */
    
    while ( isPrim(val)==0 ) {
            val += 2; /* Alle ungeraden bis naechste Primzahl */
    }
    
    return val; /* Primzahl */
}



/* 
   Berechnet einen Doppelhash Wert aus "val" und "htsize".
   
   ACHTUNG: htsize muss groesser sein als mod Wert
   --> Endlosschleife 
   
   IN: val - Wert der durch "calcHash" berechnet wurde
       htsize - Maximal moegliche Groesse
   
   OUT: return - Doppelhash Wert
*/
unsigned int calcDoppelHash( unsigned int val, unsigned int htsize ) {

   if ( htsize > 128 ) { /* 8bits */
        return 128 - ( val % 128 );
   }
   
   if ( htsize > 64 ) { /* 7bits */
        return 64 - ( val % 64 );
   }
   
   if ( htsize > 32 ) { /* 6bits */
        return 32 - ( val % 32 );
   }
   
   if ( htsize > 16 ) { /* 5bits */
        return 16 - ( val % 16 );
   }
   
   if ( htsize > 8 ) { /* 4bits */
        return 8 - ( val % 8 );
   }
   
   if ( htsize > 4 ) { /* 3bits */
        return 4 - ( val % 4 );
   }
   
   /* (MINHTBLSIZE == 5)  ---> wird niemals erreicht */
   if ( htsize > 2 ) { /* 2bits */
        return 2 - ( val % 2 );
   }
   
   if ( htsize > 1 ) { /* 1bit */
        return 1 - ( val % 1 );
   }
   
   if ( htsize == 1 ) {/* hmmm */
        return 1;
   }
   
   return 0;
}


/*
  Berechnet einen Hashwert aus dem String auf den "str" zeigt.
  Maximal Hashwert: htbl->size
  
  IN: str - Zeiger auf String, der zur Berechnung benutzt wird
      htbl - 
  
  OUT: return - Hashwert
*/
unsigned int calcHash( char *str, HashTbl *htbl ) {
    unsigned int i = 0, len;
    register unsigned int h;
        
    assert( (str != NULL) && (htbl != NULL) );
    
    len = strlen(str);
    
    h = ((unsigned char)str[i++]);
    
    while ( i < len ) {
            h = (( ( h * 256 ) + 
                   ((unsigned char)str[i++]) ) % htbl->size);
    }
    
    return h; /* Hashwert */
}

/*------------------------------------------------------------------------*/





/*------------------------------------------------------------------------*/

/*
  Initialisierung einer Hashtabellen Struktur.
  
  Besonderheit: 
  Jede! HashTbl Variable ist ueber iniHTBL(...) zu initialiseren
  und die Rueckgabe ist auszuwerten. Alle weiteren xxxHTBL oder
  xxxWHTBL Funktionen testen die Pointer nur noch mit assert(...) ab.
  
  IN: htbl - Hashtabelle die Initialisert wird
  
  OUT: return - true(0)|false(-1)
       htbl - Initialisierte Hashtabelle
*/
int iniHTBL( HashTbl *htbl ) {

    if ( htbl == NULL ) {
         return (-1);
    }
         
    htbl->size = 0;
    htbl->used = 0;
    htbl->tbl = NULL;
    htbl->files = 0;
    htbl->fnames = NULL;
    
    return 0;
}
/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/

/*
  Loescht eine FirstNode Struktur samt aller Inhalte.
  
  IN: fnode - FirstNode die geloescht werden soll
  
  OUT: fnode - Mit NULL Initialiseirt
*/
void clearFirstNode( FirstNode *fnode ) {
    Node *node, *tmpNode;
    
    if ( fnode == NULL ) 
         return ;
    
    /* evtl. vorhandenes Wort freigeben */
    if ( fnode->word != NULL ) {
         fnode->word = FREE( fnode->word );
    }
    
    /* Alle Knoten freigeben */
    node = fnode->first;
    while ( node != NULL ) {
            tmpNode = node->next;
            FREE( node );
            node = tmpNode;
    }
    
    fnode->first = NULL;
    fnode->last = NULL;
    
    return ;
}


/*
  Loescht eine Hashtabellen Struktur samt aller Inhalte.
  
  Besonderheit:
  Funktioniert mit HTBL und WHTBL.
  
  IN: htbl - Hashtabelle die geloescht werden soll
  
  OUT: htbl - Geloescht und initialisiert
*/
void clearHTBL( HashTbl *htbl ) {

    if ( htbl == NULL )
         return ;
    

    {/* Dateinamen loeschen */
     unsigned int i;
     
     /* Alle Dateinamen loeschen */
     for ( i = 0; i < htbl->files; i++ ) {
           htbl->fnames[i] = FREE( htbl->fnames[i] );
     }
     
     /* Dateinamentabelle loeschen */
     htbl->fnames = FREE( htbl->fnames );
     htbl->files = 0;
    }
    
    
    
    {/* Tabelle loeschen */
     unsigned int i;
     
     /* Alle Knoten freigeben */
     for ( i = 0; i < htbl->size; i++ ) {
           clearFirstNode( &htbl->tbl[i] );
     }     
     htbl->used = 0;
     
     /* Tabelle freigeben */
     htbl->tbl = FREE( htbl->tbl );
     htbl->size = 0;
    }
    
}
/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/


/*
  Fuegt einer Hashtabellen Struktur einen Dateinamen hinzu.
  
  Besonderheit:
  Funktioniert mit HTBL und WHTBL.
  
  IN: htbl - Hashtabelle die einen neuen Dateinamen aufnehmen soll
      fname - Dateiname der hinzugefuegt werden soll
      
  OUT: htbl - "fname" dem Dateinamenarray zugefuegt
*/
void addFNameHTBL( HashTbl *htbl, char *fname ) {

    assert( ( htbl != NULL ) && ( fname != NULL ) );
    
    /* Dateinamenarray Groesse um eins erhoehen */
    htbl->fnames = REALLOC( htbl->fnames, (htbl->files+1)*sizeof(char *) );
    
    /* Speicher fuer Datein. Zeichenkette anlegen */
    htbl->fnames[htbl->files] = MALLOC( (size_t)(strlen(fname)+1) );
    
    /* Dateinamen kopieren */
    memmove( htbl->fnames[htbl->files], fname, strlen(fname) );
    
    /* Dateinamen Terminieren */
    htbl->fnames[htbl->files][strlen(fname)]='\0';
    
    /* Anzahl vorhandener Dateinamen um eins erhoehen */
    ++htbl->files;
    
    return ;
}


/*
  Entfernt einen Dateinamen aus der Hashtabelle.
  
  Besonderheit:
  Keine Fehlerauswertung. Uebergebener Index "idx" muss mit
  getFNameIdxHTBL(...) erstellt worden sein. Fehlerabfrage dort!
  Funktioniert mit HTBL und WHTBL.
  
  IN: htbl - Hashtabelle aus der der Dateiname entfernt werden soll
      idx - Index des Dateinamens in htbl.fnames[]
  
  OUT: htbl - Mit geloeschter Datei an idx
*/
void subFNameHTBL( HashTbl *htbl, unsigned int idx ) {
    unsigned int i;

    assert( htbl != NULL );
    
    /* Ist Dateiname vorhanden */
    if ( idx >= htbl->files )
         return ;
    
    /* Dateinamen loeschen */
    htbl->fnames[idx] = FREE( htbl->fnames[idx] );
    
    /* Rest umkopieren um Luecke zu schliessen */
    for ( i = idx; i < (htbl->files-1); i++ ) {
          htbl->fnames[i] = htbl->fnames[i+1];
    }
    
    /* Anzahl vorhandener Dateinamen um eins verkleinern */
    --htbl->files;
    
    /* Dateinamenarray auf neue Groesse setzen, also (size-1) */
    htbl->fnames = REALLOC( htbl->fnames, (htbl->files)*sizeof(char *) );
    
    return ;
}


/*
  Sucht den Index des Dateinamens "fname" in htbl.fnames[] und gibt
  ihn ueber "fidx" zurueck.
  
  Besonderheit:
  Funktioniert mit HTBL und WHTBL.
  
  IN: htbl - Hashtabelle
      fname - Dateiname der gesucht werden soll
      fidx - Speicher zum ablegen des Indexes
      
  OUT: return - true(0)|false(-1)
       fidx - Index des Dateinamens wenn gefunden, sonst undef
*/
int getFNameIdxHTBL( HashTbl *htbl, char *fname, unsigned int *fidx ) {
    unsigned int i;

    assert( (htbl != NULL) && (fidx != NULL) );
    
    /* "fname" mit allen vorhandenen Dateinamen vergleichen */
    for ( i = 0; i < htbl->files; i++ ) {
          if ( strcmp( htbl->fnames[i], fname ) == 0 ) {
               *fidx = i;
               return 0;
          }
    }
    
    return (-1);
}


/*
  Kopiert! alle Dateinamen aus "srchtbl" nach "dsthtbl".
  
  Besonderheit:
  Funktioniert mit HTBL und WHTBL.
  
  IN: srchtbl - Quell Hashtabelle
      dsthtbl - Ziel Hashtabelle
      
  OUT: dsthtbl - Hashtabelle mit den Dateinamen aus "srchtbl"
*/
void copyFNamesHTBL( HashTbl *dsthtbl, HashTbl *srchtbl ) {
    unsigned int i;
    
    assert( ( dsthtbl != NULL ) && ( srchtbl != NULL ) );
         
    /* Alle Dateinamen von "srchtbl" nach "dsthtbl" Kopieren */
    for ( i = 0; i < srchtbl->files; i++ ) {
          addFNameHTBL( dsthtbl, srchtbl->fnames[i] );
    }
    
    return ;
}


/*
  Druckt die Dateinamenliste au stdout aus.
  
  Besonderheit:
  Funktioniert mit HTBL und WHTBL
  
  IN: htbl - Hashtabelle dessen Dateinamen gedruckt werden sollen
  
  OUT:
*/
void printFNamesHTBL( HashTbl *htbl ) {
     unsigned int i;
     
     assert( htbl != NULL );
     
     /* Alle vorhandenen Dateinamen auf stdout ausgeben */
     for ( i = 0; i < htbl->files; i++ ) {
           printf( "File[%d]: %s\n", i, htbl->fnames[i] );
     }
}
/*------------------------------------------------------------------------*/



/*------------------------------------------------------------------------*/

/*
  Setzt die finale Groesse einer WHTBL
  
  Besonderheit:
  NICHT auf eine HTBL anwendbar! Nur WHTBL.
  
  IN: htbl - Hashtabelle die bearbeitet werden soll
  
  OUT: htbl - Verkleinerte Hashtabelle
*/
void finalizeSizeWHTBL( HashTbl *htbl ) {

   assert( htbl != NULL );
    
   /* Neue groesse anhand der benutzten plaetze setzen */
   htbl->tbl = REALLOC( htbl->tbl, (size_t)(sizeof(FirstNode)*htbl->used) );
    
   /* "size" Wert nun "used" Wert */
   htbl->size = htbl->used;
    
   return ;
}


/* 
  Vergroessert eine Hashtabelle, damit sie mehr Woerter aufnehmen kann.
  
  Besonderheit:
  NICHT auf eine HTBL anwendbar, Wortliste->Hashtabelle ueber buildHTBL. 
  Nur WHTBL!
  
  IN: htbl - Hashtabelle deren Groesse veraendert werden soll
  
  OUT: htbl - Vergroesserte Hashtabelle
*/
void resizeHTBL( HashTbl *htbl ) {

    assert( htbl != NULL );
    
    /* Hashtabelle vergroessern */
    htbl->tbl = REALLOC( htbl->tbl, (size_t)(sizeof(FirstNode)*
                                    (htbl->size+AUTO_RESIZE_SIZE)) );
    /* Neue groesse eintragen */
    htbl->size += AUTO_RESIZE_SIZE;
    
    return ;
}


/*
  Setzt die Groesse einer Hashtabelle und initialisiert diese mit
  NULL Werten.
  
  Besonderheit:
  Nur auf leere oder neue Hashtabellen anwenden! Kann man
  aber auch zum anlegen einer Worttabelle benutzen.
  
  IN: htbl - Hashtabelle deren Groesse geaendert werden soll
      size - neue Groesse der Hashtabelle
      
  OUT: htbl - Auf "size" groesse gesetzte, initialis. Hashtabelle 
*/
void setSizeHTBL( HashTbl *htbl, unsigned int size ) {
    unsigned int i;
    
    assert( htbl != NULL );
    
    /* Setzt die groesse einer Hashtabelle */
    htbl->tbl = REALLOC( htbl->tbl, (size_t)(sizeof(FirstNode)*size) );
    htbl->size = size;
    
    /* Alle FirstNode(s) initialiseren */
    for ( i = 0; i < size; i++ ) {
          htbl->tbl[i].first = NULL;
          htbl->tbl[i].last = NULL;
          htbl->tbl[i].word = NULL;  
    }
    
    return ;
}
/*------------------------------------------------------------------------*/




/*------------------------------------------------------------------------*/

/*
  Erstellt einen neuen Speicherbereich des Datentyps "Node" und gibt
  diesen zurueck. Der "next" Zeiger wird mit einer NULLreferenz 
  initialisiert.
  
  IN:
  
  OUT: return - Zeiger auf Speicherbereich der Groesse / des Typs  Node
*/
Node * newNode(void) {
       Node *node = MALLOC( sizeof(Node) );
       node->next = NULL;
       return node;
}


/*
  Haengt ein neues Wort an eine WHTBL an.
  
  Besonderheit:
  NICHT auf eine HTBL anwendbar!    
  
  IN: htbl - WHTBL an die das Wort angehaengt werden soll
      word - Wort das hinzugefuegt werden soll
      fidx - Index im Dateinamenarray fuer dieses Wort
      col - Spalte in der das Wort steht
      row - Zeile in der das Wort steht
  
  OUT: htbl - WHTBL an die das Wort angehaengt wurde
*/
void addWordWHTBL( HashTbl *htbl, char *word, unsigned int fidx, 
                 unsigned int col, unsigned int row ) {
                 
    assert ( (htbl != NULL) && (word != NULL) && (fidx < htbl->files) );
    
    if ( htbl->used == htbl->size ) {
         resizeHTBL( htbl );
    }
    
    /* Speicher fuer eine Node anlegen */
    htbl->tbl[htbl->used].first = newNode();
    
    /* Alle Node Werte einsetzen */
    htbl->tbl[htbl->used].first->elem.fidx = fidx;
    htbl->tbl[htbl->used].first->elem.row = row;
    htbl->tbl[htbl->used].first->elem.col = col;
    
    /* Speicher fuer das Wort der FirstNode anlegen */
    htbl->tbl[htbl->used].word = MALLOC( (size_t)(strlen(word)+1) );
    
    /* Wort kopieren und Zeichenkette Terminieren */
    memmove( htbl->tbl[htbl->used].word, word, strlen(word) );
    htbl->tbl[htbl->used].word[strlen(word)]='\0';    
    
    /* .last Pointer korrekt setzen */
    htbl->tbl[htbl->used].last = htbl->tbl[htbl->used].first;
    
    /* Nun ein Platz mehr benutzt */
    ++htbl->used;
    
    return ;
}


/*
  Setzt ein Wort an eine bestimmbare Stelle in der Hashtabelle.
  
  
  Besonderheit:
  Es wird mit einer bestehenden FirstNode Struktur gearbeitet.
  In "fnode" wird der Ursprung auf NULL gesetzt.
  Ist auf WHTBL anwendbar.
  
  Anmerkung:
  if-Zweig -> Anstatt der FirstNode Inhalte können auch die FirstNdes(s)
  der Tabellen ausgetauscht werden.
    
  
  IN: htbl - Hashtabelle in der das Wort gesetzt werden soll
      fnode - FirstNode Struktur die eingesetzt werden soll
      idx - Position in der Hashtabelle an die das Wort gesetzt werden soll
  
  OUT: htbl Hashtabelle an die an "idx" "fnode" angehaengt wurde
*/
void setWordHTBL( HashTbl *htbl, FirstNode *fnode, unsigned int idx ) {

    assert( ( htbl != NULL ) && ( fnode != NULL ) && (idx < htbl->size) );
         
    /* Ist die FirstNode noch leer */
    if ( htbl->tbl[idx].first == NULL ) {
    
         /* Zeiger in FirstNode umkopieren und die Ursprungszeiger 
            auf NULL setzen. */
            
         htbl->tbl[idx].first = fnode->first;
         htbl->tbl[idx].last = fnode->last;
         fnode->first = NULL;         
         fnode->last = NULL;
        /* htbl->tbl[idx].last = htbl->tbl[idx].first;*/
                  
         htbl->tbl[idx].word = fnode->word;
         fnode->word = NULL;
                           
         ++htbl->used; /* Ein Eintrag ist dazugekommen */

    } else {
         
         /* Es sind schon Knoten in FirstNode, also hintenanhaengen */
         htbl->tbl[idx].last->next = fnode->first;
         htbl->tbl[idx].last = fnode->last;
         /*htbl->tbl[idx].last = fnode->first;*/
         fnode->first = NULL;
         fnode->last = NULL;
         
    }
    
    return ;
}



/*
  Bestimmt die Position eines Wortes in der Hashtabelle.
  
  Besonderheit:
  Funktioniert nur mit HTBL.
  
  IN: htbl - Hashtabelle in der die Wortposition bestimmt werden soll
      word - Suchwort
      idx - Position des Wortes in der Hashtabelle
      
  OUT: return - fehler(-1)|true(1)|false(0)       
*/
int getWordIdxHTBL( HashTbl *htbl, char *word, unsigned int *idx ) {
    unsigned int hash, dhash, rehash=0;

    assert( (word != NULL) && (idx != NULL) && (htbl != NULL) );

    /* Hashwert fuer "word" berechnen */
    hash = calcHash( word, htbl );
    
    /* Doppelhashwert berechnen */
    dhash = calcDoppelHash( hash, htbl->size );

    /* Solange kein leerer Platz gefunden wurde */
    while ( htbl->tbl[hash].first != NULL ) {
    
            ++rehash;
            /* Endlosschleife im Fehlerfall vermeiden */
            if ( rehash > (3*htbl->size) ) 
                 return (-1);
            
            /* Ist in FirstNode das gesuchte Wort */
            if ( strcmp( word, htbl->tbl[hash].word ) == 0 ) {
                 *idx = hash;
                 return 1;
            }
            
            /* ReHash */
            hash = ((hash+dhash) % htbl->size);
            
    }
    
    return 0;
}

/*------------------------------------------------------------------------*/




/*------------------------------------------------------------------------*/

/*
  Zerlegt eine Zeichenkette in Woerter.
  
  Besonderheit:
  Funktioniert nur mit WHTBL.
  
  IN: whtbl - WHTBL, als Worttabelle(liste) genutzt
      data - Zu zerlegende Zeichenkette
      dataLen - Laenge der Zeichenkette
      aktFileName - Dateiname zu der die Zeichenkette gehoert
      austab - Ausschlusstabelle
  
  OUT: return - fehler(-1)|fehler(-2)|ok(0)
       whtbl - WHTBL in die die gefundenen Woerter eingetragen wurden
*/
int string2WHTBL( HashTbl *whtbl, char *data, unsigned int dataLen,
                    char *aktFileName, Austab *austab ) {
    unsigned int row=1, col=0, pos=0, posAhead=0, fidx;
    char tmpChar ;

    /*  */
    if ( (data==NULL) || (whtbl==NULL) || (aktFileName==NULL) || 
         (austab==NULL) )
          return (-1);
    
   
    /* Dateinamen aus dem die Zeichenkette aus "data" stammt,
       der Dateinamentabelle zufuegen */
    addFNameHTBL( whtbl, aktFileName );
    /* Ist der Dateiname auch sicher eingetragen worden und an welcher 
       Stelle */
    if ( getFNameIdxHTBL( whtbl, aktFileName, &fidx ) < 0 )
         return (-2);
    
    
    /* Bis alle Zeichen aus "data" verarbeitet sind */
    for ( pos = 0; pos < dataLen;  ) {

      switch (data[pos]) {
      
        /* Neue Zeile */
        case '\n':             
             ++row; /* Zeilenmarker um eins erhoehen */
             col=0; /* Spaltenmarker reset */
             pos++; /* Laufende Position in "data" um eins erhoehen, 
                       weil \n verarbeitet wurde */
             continue; /* Am Ende der Zeichenkette? */
        break;
        
        default:
             /* Vorausschau Variable initialisieren */
             posAhead = pos;
             
             /* austab->tbl[x] ist entweder true oder false fuer aktuelles
                Zeichen. true = Trennzeichen */

             while ( ((! austab->tbl[ 
                          /* Vorsicht Falle, 128-255 negativ */
                         ( (int) ((unsigned char)data[posAhead]) ) 
                                    ]) && 
                     (posAhead < dataLen)) ) {
                       posAhead++;
             }
             
             /* Ist gefundenes Wort min. 3 Zeichen lang?
                Naechstes Zeichen in Zeichenkette merken.
                ( Am Ende ist ein \0 Zeichen, also sicher fuer data[last] )
                In Zeichenkette gefundenes Wort terminieren.
                Wort der Wortliste zufuegen.
                Altes Zeichen in Zeichenkette wiederherstellen.
                Positionsvariablen aktualisieren.
                Am Ende der Zeichenkette?
             */
             if ( (posAhead - pos) > 2 ) {
                   tmpChar = data[posAhead]; 
                   data[posAhead] = '\0';
                   addWordWHTBL( whtbl, &data[pos], fidx, col, row );
                   data[posAhead] = tmpChar;
                   col += posAhead-pos;
                   pos = posAhead;
                   continue;             
             }/*if*/
             
        break;    
    
      }/*switch*/
      
      /* Ist das Wort kleiner 3 Zeichen gewesen, wird hier weitergemacht.
         Eigentlich +2 da ja aktuelles und naechstes schon ungueltig sind.*/
      pos++; col++;
     
    }/*for (pos) */
    
    /* Wortlistengroesse auf belegte Plaetze setzen. */
    finalizeSizeWHTBL( whtbl );
    
    return 0;
}
/*------------------------------------------------------------------------*/




/*------------------------------------------------------------------------*/

/*
  Codeauslagerung aus buildHTBL
  
  Berechnet neue Positionen fuer die Eintraege aus srchtbl und
  haengt die Referenzen entsprechend um.
  Ist noetig da die dsthtbl eine kleinere groesse hat.
  
  IN: srchtbl - Quell Hashtabelle
      dsthtbl - Ziel Hashtabelle
  
  OUT: dsthtbl - Neu aufgebaute Hashtabelle
*/
void makeSmallHTBL( HashTbl *srchtbl, HashTbl *dsthtbl ) {
     unsigned int i, hash, dhash, rehash=0;
   
     assert( (srchtbl != NULL) && (dsthtbl != NULL) );
   
     /* Dateinamen umkopieren */
     copyFNamesHTBL( dsthtbl, srchtbl );
     
     /* Alle Plaetze in Tabelle durchgehen */
     for ( i = 0; i < srchtbl->size; i++ ) {
     
           /* Leere Node -> nichts zu tun */
           if ( srchtbl->tbl[i].first == NULL )
                continue;

           /* Hashwert fuer Zeichenkette berechnen */
           hash = calcHash( srchtbl->tbl[i].word, dsthtbl );
           
           /* Ist die Position frei? */
           if ( dsthtbl->tbl[hash].first != NULL ) {
                ++rehash;
                
                /* Wenn nicht frei, dann Rehash bis freie Position
                   gefunden wurde. */
                dhash = calcDoppelHash( hash, dsthtbl->size );
                hash = ((hash+dhash) % dsthtbl->size);                
                               
                while ( dsthtbl->tbl[hash].first != NULL ) {
                        hash = ((hash+dhash) % dsthtbl->size);                        
                        ++rehash;
                }
                
           }
           
           /* Daten umhaengen und alte Pointer auf NULL setzen */
           dsthtbl->tbl[hash].first = srchtbl->tbl[i].first;
           dsthtbl->tbl[hash].last = srchtbl->tbl[i].last;
           dsthtbl->tbl[hash].word = srchtbl->tbl[i].word;
           srchtbl->tbl[i].first = NULL;
           srchtbl->tbl[i].last  = NULL;
           srchtbl->tbl[i].word = NULL;
           
     }
     
}


/*
  Erstellt aus einer WHTBL(Hashtabelle als Wortliste) eine 
  HTBL(Hashtabelle).
  
  IN: srchtbl - Hashtabelle als Wortliste.
      htbl - Ziel in das die Hashtabelle aufgebaut werden soll.
      
  OUT: return - fehler(-1)|ok(0)
       htbl - Neu aufgebaute Hashtabelle
*/
int buildHTBL( HashTbl *srchtbl ) {
    unsigned int i, hash, dhash, found, cnt=0;
    HashTbl ihtbl;
    
    if ( srchtbl == NULL )
          return (-1);
    
    /* Temporaere Hashtabelle initialisieren */
    (void)iniHTBL( &ihtbl );/* (ihtbl != NULL)  ...locale Variable */
    
    /* Groesse setzen, hier wird davon ausgegangen, das
       alle Woerter unterschiedlich sind. Spaeter wird die 
       Anzahl doppelt vorhandener Woerter gezaehlt und dann eine
       verkleinerte Hashtabelle aufgebaut. */
    setSizeHTBL( &ihtbl, calcNextPrim( srchtbl->used ));
    
    
    /* Dateinamen umkopieren */
    copyFNamesHTBL( &ihtbl, srchtbl );    
    
    /* Alle Woerter aus Wortliste verarbeiten.
       Wortliste: .used == .size */
    for ( i = 0; i < srchtbl->used; i++ ) {
          
          /* Hashwert berechnen */
          hash = calcHash( srchtbl->tbl[i].word, &ihtbl );
          /* evtl. benoetigter Doppelhashwert berechnen */
          dhash = calcDoppelHash( hash, ihtbl.size );
          
          /* Noch kein Platz gefunden? */
          found = 0;          
          while ( ! found ) {          
                  
                  /* Leere Tabellenposition, dann hier eintragen */
                  if ( ihtbl.tbl[hash].word == NULL ) {
                  
                       /* neu */
                       setWordHTBL( &ihtbl, &srchtbl->tbl[i], hash );
                       found = 1;
                       ++cnt; /* Unterschiedliche Woerter */
                       
                  } else {
                  
                       /* Steht hier das selbe Wort, dann anhaengen. */
                       if ( strcmp( ihtbl.tbl[hash].word, 
                                    srchtbl->tbl[i].word ) == 0 ) {
                       
                            /* gleiche Woerter */
                            setWordHTBL( &ihtbl, &srchtbl->tbl[i], hash );
                            found = 1;
                            
                       }
                       
                  }/*if-else*/
                  
                  /* Nichts passendes gefunden, Rehash und nochmal 
                     versuchen */
                  hash = (hash+dhash)%ihtbl.size;
                  
          }/*while*/                    
    }/*for*/
    
     /* Alte Wortliste loeschen */
     clearHTBL(srchtbl);
     /* Neu initialisieren */
     (void)iniHTBL(srchtbl);
     /* Endgueltige groesse setzen. */
     setSizeHTBL( srchtbl, calcNextPrim( cnt + ((cnt/100)*10) ) );
     /* Aufbau der endgueltigen Hashtabelle */
     makeSmallHTBL( &ihtbl, srchtbl );
     /**/
     srchtbl->used = cnt;
     /* Temporaere Tabelle loeschen */
     clearHTBL( &ihtbl );

    return 0;
}



/*
  Entfernt aus einer Hashtabelle alle Referenzen auf Woerter aus
  Datei "fname".

  Besonderheit:
  Funktioniert mit HTBL und WHTBL.
  
  IN: htbl - Hashtabelle aus der die Referenzen entfernt werden sollen
      fname - Dateiname anhand dessen die zu entfernenden Woerter erkannt 
              werden
  
  OUT: return - fehler(-1)|ok(0)
       htbl - HTBL aus der alle Refere. auf "fname" entfernt wurden
*/
int subFNameRefHTBL( HashTbl *htbl, char *fname ) {
    unsigned int i;
    unsigned int  fidx, tfidx;
    HashTbl ihtbl;
    Node *node;
    
    if ( (htbl == NULL) || (fname == NULL) ) 
         return (-1);
    
    /* Index ,der zu entfernenden Datei, im Dateinamenarray holen */
    if ( getFNameIdxHTBL( htbl, fname, &fidx ) < 0 )
         return 0;
    
    /* Ist nur noch eine Dateireferenz vorhanden, ganze Tabelle loeschen. */
    if ( htbl->files == 1 ) {
         clearHTBL( htbl );
         return 0;
    }
    
    /* Dateinamen aus Dateinamenliste loschen */
    subFNameHTBL( htbl, fidx );


    /* Temporaere Tabelle anlegen */
    (void)iniHTBL( &ihtbl );
    copyFNamesHTBL( &ihtbl, htbl );
    

    /* Alle Plaetze in Tabelle verarbeiten */
    for ( i = 0; i < htbl->size; i++ ) {
          
          /* Kurze Schreibweise... */
          node = htbl->tbl[i].first;
          
          /* Nichts zu tun in dieser FirstNode */
          if ( node == NULL )
               continue;
                  
          
          /* Liste verarbeiten */
          while ( node != NULL ) {
          
                  /* Ist die Node eine die entfernt werden soll?
                     Wenn gleich wird die Node einfach uebergangen. */
                  if ( node->elem.fidx != fidx ) {
               
                     /* Neuen Dateinamenindex erstellen */
                     if ( node->elem.fidx > fidx )
                          tfidx = node->elem.fidx-1;
                     else
                          tfidx = node->elem.fidx;
                     
    
                     /* Wort der Wortliste zufuegen */
                     addWordWHTBL( &ihtbl, htbl->tbl[i].word, tfidx, 
                                   node->elem.col, node->elem.row );
                     
                  } 
                  
                  node = node->next;
                  
          }/*while*/          
    }/*for*/
    
    
    finalizeSizeWHTBL( &ihtbl );
    /*  Alte Tabele loeschen */
    clearHTBL( htbl );
    (void)iniHTBL( htbl );/* (htbl != NULL)  muss hier schon lange getestet
                             sein */
    
    
    *htbl = ihtbl;
    
    return 0;
}
/*------------------------------------------------------------------------*/




/*------------------------------------------------------------------------*/

/*
  Gibt eine Hashtabelle aud stdout aus.
  
  Besonderheit:
  Funktioniert mit HTBL und WHTBL.
  
  IN: htbl - Auszugebende Hashtabelle
  
  OUT:
*/
void printHTBL( HashTbl *htbl ) {
    unsigned int i;
    Node *node;
    
    assert ( htbl != NULL );
    
    /* Alle Elemente der Tabelle durchgehen */
    for ( i = 0; i < htbl->size; i++ ) {
    
          node = htbl->tbl[i].first;
          
          while ( node != NULL ) {

                printf("%s:%s:%d:%d\n", 
                        htbl->tbl[i].word,
                        htbl->fnames[node->elem.fidx],
                        node->elem.row,
                        node->elem.col
                      );
                
                node = node->next;
          }
          
    }
    
    fflush(stdout);
    
    return ;
}


/*
  Gibt den Inhalt einer FirstNode Struktur aus stdout aus.
  
  IN: htbl - Hashtabelle
      idx - Index der FirstNode in "htbl"
  
  OUT:
*/
void printFirstNode( HashTbl *htbl, unsigned int idx ) {
    Node *node;
    
    assert( (htbl != NULL) && (idx < htbl->size) );
    
    node = htbl->tbl[idx].first;
    
    while ( node != NULL ) {
            printf("row: %d, col: %d, file: %s\n", 
                        node->elem.row,
                        node->elem.col,
                        htbl->fnames[node->elem.fidx]                        
                  );
            node = node->next;
    }
    
    return ;
}

/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/

/*
  Speichert eine Hashtabelle Zeilenweise in eine Txtdatei.
  
  Besonderheit:
  Funktioniert mit HTBL und WHTBL.
  
  IN: htbl - Hashtabelle die gespeichert werden soll
      fname - Dateiname der Textdatei
  
  OUT: return - fehler(-1)|ok(0)
*/
int saveHTBL( HashTbl *htbl, char *fname ) {
    unsigned int i;
    int ret;
    FILE *fz;
    Node *node;
        
    assert( htbl != NULL );

    /* Datei zur Ausgabe anlegen */
    fz = fopen( fname, "wb" );
    if ( fz == NULL ) {
         perror("Save Hashtabelle ");
         return (-1);
    }
    
    /* Alle Elemente verarbeiten */
    for ( i = 0; i < htbl->size; i++ ) {
          
          node = htbl->tbl[i].first;
          
          while ( node != NULL ) {
          
                ret = fprintf( fz, "%s:%s:%d:%d\n",
                               htbl->tbl[i].word, 
                               htbl->fnames[node->elem.fidx],
                               node->elem.row, 
                               node->elem.col
                             );
                
                /* ret<0 Fehler beim schreiben in Datei */
                if ( ret < 0 ) {
                     perror("Save Hashtabelle ");
                     fclose(fz);
                     return (-1);
                }
                
                node = node->next;
          
          }/*while*/  
    }
    
    fclose(fz);
    
    return 0;
}



/*
  Liesst eine von diesem Programm gespeicherte Indexdatei ein
  und baut eine fertige Hashtabelle auf.
  
  Besonderheit:
  Eine fehlerhafte Indexdatei kann zu undefiniertem Verhalten fuehren.  
  
  IN: htbl - Hashtabelle in die die Daten abgelegt werden sollen
      fname - Dateiname der Textdatei
  
  OUT: return - fehler(-1)|ok(0)
       htbl - Hashtabelle aus Daten aus "fname" aufgebaut
*/
int readHTBL( HashTbl *htbl, char *fname ) {
#define MAXSSCANF MAXBUFINDEXLINE+1 /* 1303 + 1 */

    FILE *fz;
    
    char tbuf[MAXBUFINDEXLINE+2], 
         tword[MAXWORDLEN+1], 
         tfname[MAXFNAMELEN+1];    
         
    unsigned int  tcol, trow, tfidx;
    
    if ( htbl == NULL )
         return (-1);

    
    fz = fopen( fname, "rb" );
    if ( fz == NULL ) {
         perror("Read Hashtabelle ");
         return (-1);
    }
    
    /* Zeilenweise aus Datei lesen, max. "tbuf" groesse pro Ziele */
    while ( fscanf(fz, "%1303s\n", tbuf) != EOF ) {
            if (
                sscanf( tbuf, "%[^:]:%[^:]:%u:%u", 
                        tword, tfname, &trow, &tcol ) != 4 ) {
                fclose(fz);
                return (-1);
            }
            
            /* Fals Dateiname noch nicht enthalten, eintragen. */
            if ( getFNameIdxHTBL( htbl, tfname, &tfidx ) < 0 ) {
                 addFNameHTBL( htbl, tfname );
            }
            
            /* Ist der Dateiname eingetragen? 
               Wenn ja, welche Position? */
            if ( getFNameIdxHTBL( htbl, tfname, &tfidx ) < 0 ) {
                 clearHTBL( htbl );
                 return (-1);
            }
            
            /* Das Wort eintragen */
            addWordWHTBL( htbl, tword, tfidx, tcol, trow );
    }    
    
    fclose(fz);
    
    finalizeSizeWHTBL( htbl );

    /* Hashtabelle aufbauen */
    return buildHTBL( htbl );

}



/*
  Liesst eine von diesem Programm gespeicherte Indexdatei ein.
  
  IN: whtbl - Hashtabelle, als Wortliste, in die die Daten abgelegt werden
              sollen
      fname - Dateiname der Textdatei
  
  OUT: return - fehler(-1)|ok(0)
       whtbl - WHTBL in die alle daten aus "fname" eingetragen wurden
*/
int readWHTBL( HashTbl *whtbl, char *fname ) {
    FILE *fz;
    char tbuf[MAXBUFINDEXLINE+2], 
         tword[MAXWORDLEN+1], 
         tfname[MAXFNAMELEN+1];
         
    unsigned int tcol, trow, tfidx;
    
    if ( whtbl == NULL )
         return (-1);
    
    fz = fopen( fname, "rb" );
    if ( fz == NULL ) {
         perror("Read Hashtabelle als Wortliste ");
         return (-1);
    }
    
    /* Zeilenweise aus Datei lesen, max. "tbuf" groesse pro Ziele */
    while ( fscanf(fz, "%1303s\n", tbuf) != EOF ) {
            if (
                sscanf( tbuf, "%[^:]:%[^:]:%u:%u", 
                        tword, tfname, &trow, &tcol ) != 4 ) {
                fclose(fz);
                return (-1);
            }
            
            /* Fals Dateiname noch nicht enthalten, eintragen. */
            if ( getFNameIdxHTBL( whtbl, tfname, &tfidx ) < 0 ) {
                 addFNameHTBL( whtbl, tfname );                 
            }
            
            /* Ist der Dateiname eingetragen? 
               Wenn ja, welche Position? */
            if ( getFNameIdxHTBL( whtbl, tfname, &tfidx ) < 0 ) {
                 clearHTBL( whtbl );
                 return (-1);
            }
            
            /* Das Wort eintragen */
            addWordWHTBL( whtbl, tword, tfidx, tcol, trow );
    }    
    
    fclose(fz);
    
    finalizeSizeWHTBL( whtbl );
    
    return 0;
}
/*------------------------------------------------------------------------*/



/*------------------------------------------------------------------------*/

/*
  Sucht in einer Hashtabelle nach Woertern, die von stdin gelesen werden.
  Die/Das Ergebnis wird auf stdout ausgegeben.

  IN: htbl - Hashtabelle in der gesucht werden soll
  
  OUT:
*/
void searchHTBL( HashTbl *htbl ) {
/* MAXWORDLEN + \n + \0 */
#define MAXLINELEN (MAXWORDLEN + 2)

     char line[MAXLINELEN];
     unsigned int  idx;
     int ret;
     
     assert(htbl != NULL);
     

     printf("Geben sie ein Suchwort ein:\n");fflush(stdout);
     
     /* Solange Zeilenweise lesen bis EOF oder Fehler */
                        /* n-1 -> \0 */
     while ( fgets( line, MAXLINELEN, stdin ) != NULL ) {
     
             /* User hat zu viele Zeichen angegeben, wenn vorletztes 
                Zeichen kein '\n' ist.                
                Rest wird bei naechstem fgets(...) gelesen!
                Nicht schoen, aber ein User Fehler. */
             if ( strlen(line) == (MAXLINELEN-1) )
                  if ( line[strlen(line)-1] != '\n' ) {
                       printf("Suchwort ist zu lang.\n\n");
                       continue;
                  }
             
                          
             line[strlen(line)-1]='\0';/* \n loeschen */
             
             /* Index in Hashtabelle berechnen */
             if ( (ret = getWordIdxHTBL( htbl, line, &idx )) < 0 ) {
                  printf("Bei der Suche nach \"%s\" ist ein Fehler \
aufgetreten.\n\n", line);
                  return ;
             }
             
             
             if ( ret == 0 ) {
             
                  printf("Suche nach \"%s\" war erfolglos\n\n", line); 
                  
             } else {
             
             /* Wenn Wort gefunden, FirstNode Struktur ausgeben */
                  printf("Suche nach \"%s\" war erfolgreich\n", line);                   
                  printFirstNode( htbl, idx );
                  printf("\n");
             }
             
             fflush(stdout);
     }/*while*/
     
}
/*------------------------------------------------------------------------*/


/*EOF*/
