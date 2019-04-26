/* $Id: alloc_em.c 254 2001-06-25 20:06:36Z skaus $

	as malloc(), but runs error_out_of_memory() on error
*/

#include <stdlib.h>

#include "../err_fcts.h"
#include "../include/misc.h"

void *emalloc(const size_t size)
{	void *h;

	if((h = malloc(size)) == 0)
		error_out_of_memory();

	return h;
}
