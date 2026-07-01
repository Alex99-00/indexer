/*
BEGIN  C-Head v0.2


  -Von:
   Alexander Klupsch, ia3394


  -Dateiname:
   alloc.c


  -Sprache:
   ANSI-C


  -Main(0|1):
   0


  -Zweck/Beschreibung:
   Dyamische Speicherbeschaffung und Freigabe


  -Programm:
   indexer


  -Besonderheiten:
   Im Fehlerfall von malloc/realloc: exit(1)


END---------------------------------------------------------------------75->
*/

#include <stdio.h>
#include <stdlib.h>

#if !defined(NDEBUG)
/* 
  Enthaelt die aktuelle Differenz von "MALLOC" zu "FREE" 
  Aufrufen --> Fuer Testzwecke.
*/
int mallocCounter=0;
#endif



/*
  Reserviert "size" Bytes an Speicher und gibt einen Zeiger auf den 
  Anfang des Speicherbereichs zurueck.
  
  IN: size - Groesse des benoetigten Speicherbereichs
  
  OUT: return - Pointer auf den Speicherbereich
  
*/
void * MALLOC (size_t size) {

  void *result = malloc (size);
  
  if (result == NULL) {
      perror ("Getting memory");
      exit (1);
  }
#if !defined(NDEBUG)
  ++mallocCounter;
#endif
  return result;
  
}




/*
  Veraendert die Groesse des durch ptr gekennzeichneten Speicherbereichs
  auf die Groesse die durch "size" beschrieben wird.
  
  IN: ptr - Zeiger auf bereits vorhandenen Speicherbereich oder NULL
      size - neue groesse des Speicherbereichs
      
  OUT: return - Pointer auf den Speicherbereich
*/
void * REALLOC (void *ptr, size_t size) {

  void *result = realloc (ptr, size);

    
  if (result == NULL) {
      perror ("reGetting memory");
      exit (1);
  }

#if !defined(NDEBUG)
  if ( ptr == NULL )
       ++mallocCounter;
#endif
  return result;
}




/*
  Gibt den Speicherbereich auf den "ptr" zeigt frei.
  
  IN: ptr - Zeiger auf freizugebenen Speicherbereich
  
  OUT: return - NULL
*/
void * FREE (void *ptr) {

  if ( ptr != NULL ) {
       free(ptr);
#if !defined(NDEBUG)
       --mallocCounter;
#endif
  }
  
  return NULL;
  
}


/*EOF*/
