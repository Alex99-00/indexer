/*
BEGIN  C-Head v0.2


  -Von:
   Alexander Klupsch, ia3394


  -Dateiname:
   param.c


  -Sprache:
   ANSI-C


  -Main(0|1):
   0


  -Zweck/Beschreibung:
   Parameterauswertung



  -Programm:
   indexer


  -Besonderheiten:
   
END---------------------------------------------------------------------75->
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <param.h>
#include <datei.h>
#include <alloc.h>


/* Gueltige Parameter */
static char *validParams[] = { "-a", "--add", "-s", "--sub" };

/* Anzahl gueltiger Parameter */
static int maxValidParam = 4;



/* 
  Gibt Parameter Struktur auf stdout aus. 
  
  IN: param - Parameter
  
  OUT: 
*/
void printParam( Parameter param ) {
     int i;
     
     printf("indexdatei: %s\n", param.indexdatei );
     printf("indexexists: %d\n", param.indexexists );
    
     printf("addListLen: %d\n", param.addListLen );
     for (i=0; i<param.addListLen; i++) 
          printf("addList[%d]: %s\n", i, param.addList[i] );
    
    
    
     printf("subListLen: %d\n", param.subListLen );
     for (i=0; i<param.subListLen; i++) 
          printf("subList[%d]: %s\n", i, param.subList[i] );
    
    
     printf("help:  %d\n", param.help );
     printf("error: %d\n", param.error );
     
}



/*
  Initialisiert eine Parameter Struktur.
  
  IN: param - Parameter
  
  OUT:
*/
static void iniParameter( Parameter *param ) {
  
     assert( param != NULL );
     
          
     param->indexdatei  = NULL;
     param->indexexists = 0;
     param->addList     = NULL;
     param->addListLen  = 0;
     param->subList     = NULL;
     param->subListLen  = 0;
     param->help        = 0;
     param->error       = 0;
     param->fnotfound   = 0;
}



/*
  Loescht eine Parameter Struktur komplett.
  
  IN: param - Parameter
  
  OUT: 
*/
void cleanupParameter( Parameter *param ) {
     int i;

     if ( param == NULL )
          return ;
     
     param->indexdatei = FREE( param->indexdatei );

     for ( i=0; i < param->addListLen; i++ ) {
           param->addList[i] = FREE( param->addList[i] );
     }
     param->addList = FREE( param->addList );


     for ( i=0; i < param->subListLen; i++ ) {
           param->subList[i] = FREE( param->subList[i] );
     }
     param->subList = FREE( param->subList );

     iniParameter( param ); /* Reset */
     
     return ;
}



/*
  Sucht Index von "fname" in "param.subList[]"

  IN: param - Parameter
      fname - Dateiname
      
  OUT: false(-1)|true(pos)
*/
static int isInSubList( Parameter *param, char *fname ) {
    int i;
    
    assert((param != NULL) && (fname != NULL));
    
    for ( i = 0; i < param->subListLen; i++ ) {
          if ( strcmp(param->subList[i], fname) == 0 )
               return i;
    }
    
    return (-1);
}



/*
  Sucht Index von "fname" in "param.addList[]"

  IN: param - Parameter
      fname - Dateiname
      
  OUT: false(-1)|true(pos)
*/
static int isInAddList( Parameter *param, char *fname ) {
    int i;
    
    assert((param != NULL) && (fname != NULL));
    
    for ( i = 0; i < param->addListLen; i++ ) {
          if ( strcmp(param->addList[i], fname) == 0 )
               return i;
    }
    
    return (-1);
}



/*
  Loescht einen Dateinamen aus "param.subList[]"

  IN: param - Parameter
      fname - Dateiname
      
  OUT: ok(0)
*/
static void removeFromSubList( Parameter *param, char *fname ) {
    int fidx, i;
    
    assert((param != NULL) && (fname != NULL));
    
    fidx = isInSubList( param, fname );
    
    if (fidx<0)
        return ;
        
    param->subList[fidx] = FREE(param->subList[fidx]);
    
    for ( i = fidx; i < (param->subListLen-1); i++ ) {
          param->subList[i] = param->subList[i+1];
    }
        
    param->subList = REALLOC(param->subList, 
                     (size_t)(sizeof(char *) * param->subListLen));

    --param->subListLen;
    
    return ;
}



/*
  Loescht einen Dateinamen aus "param.addList[]"

  IN: param - Parameter
      fname - Dateiname
      
  OUT: ok(0)
*/
static void removeFromAddList( Parameter *param, char *fname ) {
    int fidx, i;
    
    assert((param != NULL) && (fname != NULL));
    
    fidx = isInAddList( param, fname );
    
    if (fidx<0)
        return ;
        
    param->addList[fidx] = FREE(param->addList[fidx]);
    
    for ( i = fidx; i < (param->addListLen-1); i++ ) {
          param->addList[i] = param->addList[i+1];
    }
    
    param->addList = REALLOC(param->addList, 
                     (size_t)(sizeof(char *) * param->addListLen));

    --param->addListLen;
    
    return ;
}



/*
  Fuegt einen Dateinamen zu "param.addList[]" hinzu.

  IN: param - Parameter
      fname - Dateiname
      
  OUT:
*/
static void addToAddList( Parameter *param, char *fname ) {
    
    assert((param != NULL) && (fname != NULL));
    
    param->addList = REALLOC( param->addList, 
                     (size_t)(sizeof(char *) * (param->addListLen+1)) );
    
    param->addList[param->addListLen] = MALLOC( (size_t)(strlen(fname)+1) );
    
    memmove( param->addList[param->addListLen], fname, 
             (size_t)(strlen(fname)+1) );
    
    ++param->addListLen;
    
    return ;
}



/*
  Fuegt einen Dateinamen zu "param.subList[]" hinzu.

  IN: param - Parameter
      fname - Dateiname
      
  OUT:
*/
static void addToSubList( Parameter *param, char *fname ) {

    assert((param != NULL) && (fname != NULL));

    param->subList = REALLOC( param->subList, 
                     (size_t)(sizeof(char *) * (param->subListLen+1)) );
    
    param->subList[param->subListLen] = MALLOC( (size_t)(strlen(fname)+1) );
    
    memmove( param->subList[param->subListLen], fname, 
             (size_t)(strlen(fname)+1) );
    
    ++param->subListLen;
    
    return ;
}



/* !!! assume argv[0] = fileName.self !!! 

  Wertet die Programm Parameter aus und erstellt eine Parameter Struktur
  daraus.
  
  IN: argc - Anzahl Parameter
      argv - Programm Parameter
  
  OUT return - Parameter Struktur
*/
Parameter evalParams( int argc, char ** argv) {

     /* result */
     Parameter param;
     
     iniParameter( &param );

     /* keine Parameter */
     if ( argc == 1 ) {
          param.help = 1;
          return param;
     }
     
     
     /* -h|--help mit argc=2 */
     if ( (argc == 2) && 
          ( 
            (memcmp(argv[1], "-h",     strlen("-h"))==0) || 
            (memcmp(argv[1], "--help", strlen("--help"))==0) 
          ) 
        ) {
          param.help = 1;
          return param;
     }
     

     /* mahr als ein Parameter und -h|--help angegeben -> Fehler */
     if ( argc > 2 ) {
     
        int i;
        for ( i=1; i<argc; i++ ) {
        
            if ( (memcmp(argv[i], "-h",     strlen("-h"))==0) || 
                 (memcmp(argv[i], "--help", strlen("--help"))==0) 
               ) {
                   param.error = 1;
                   return param;
               } /*if*/
               
        }/*for*/
        
     }/*if*/
     
     
     /*-------------------------------------------------------------------*/
    {/*BLOCK0*/
     int vp, av=0, found; /* vp = index in validParam;   
                             av = index in argv */

     while ( ++av < argc ) { /* fuer alle argv[av] */            
             found = 0;
             
             
       /* alle validParam[vp] testen */
       for ( vp = 0; vp < maxValidParam; vp++ ) { 
       

             /* validParam in argv[av] gefunden */             
             if ( strcmp(argv[av], validParams[vp]) == 0 ) {

                  ++av;  /* next = param value */

                  if ( (av > argc) || (argv[av] == NULL) ) {
                       cleanupParameter(&param); 
                       param.error = 1;
                       return param;
                  }
		                                          /* Orginaldatei muss nicht
					                                     existieren fuer --sub */
                  if ( ((findFile( argv[av])==1) || ((vp==2)||(vp==3))) ) {
                       found = 1;
                       switch (vp) {
                       
                         /*-----------------------------------------------*/
                         case 0: 
                         case 1:
                         
                         if ( isInAddList( &param, argv[av] ) >= 0 )
                              continue;
                              
                         if ( isInSubList( &param, argv[av] ) >= 0 ) {
                              removeFromSubList( &param, argv[av] );
                              continue;
                         }
                              
                         addToAddList( &param, argv[av] );
                         vp = maxValidParam;/*break for (vp)*/
                         break;

                         /*-----------------------------------------------*/
                         case 2: 
                         case 3: 
                         
                         if ( isInSubList( &param, argv[av] ) >= 0 )
                              continue;
                              
                         if ( isInAddList( &param, argv[av] ) >= 0 ) {
                              removeFromAddList( &param, argv[av] );
                              continue;
                         }
                         
                         addToSubList( &param, argv[av] );
                         vp = maxValidParam;/*break for (vp)*/
                         break;

                         /*-----------------------------------------------*/
                         default: 
                                 cleanupParameter(&param); 
                                 param.error = 1;
                                 return param; 
                         break;
                         
                       }/*switch*/                       

                  } else {
                       cleanupParameter(&param);
                       param.fnotfound = av;
                       return param;
                  }/* if-else (findFile) */              
                  
             }/*if (strcmp) */
             
              
       }/*for (vp)*/
      
       if ( found==0 ) { 
            
           if ( param.indexdatei != NULL) { /* schon vorhanden */
                cleanupParameter(&param);
                param.error = 1;
                return param;
           }
           
           param.indexdatei = (char *)MALLOC((size_t)(strlen(argv[av])+1));
           memmove(param.indexdatei, argv[av],(size_t)(strlen(argv[av])+1));
                        
           if ( findFile( argv[av] ) ) {
                param.indexexists = 1;
                
           } else {
                param.indexexists = 0;
           }
            
       }/*if (! found)*/
       
     }/*while (av)*/
     
     
    }/*BLOCK0*/
     
    
     return param;

}

/*EOF*/
