
#ifndef __ALLOC_H
#define __ALLOC_H


void * MALLOC (size_t size);

void * REALLOC (void *ptr, size_t size);

void * FREE (void *ptr);

#else
#error "Multiple include"
#endif

/*EOF*/
