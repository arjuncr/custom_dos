/* $Id: alloc_e1.c 262 2001-07-16 20:28:38Z skaus $

	as erealloc(), but deallocates the pointer on failure
*/

#include "../config.h"

#include <stdlib.h>

#include "../err_fcts.h"
#include "../include/misc.h"

void *efrealloc(void * const poi, const size_t size)
{	void *h;

	if((h = realloc(poi, size)) == 0) {
		error_out_of_memory();
		myfree(poi);
	}

	return h;
}
