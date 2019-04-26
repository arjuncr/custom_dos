/* $Id: alloc_es.c 254 2001-06-25 20:06:36Z skaus $

	as strdup(), but runs error_out_of_memory() on error
*/

#include <string.h>

#include "../err_fcts.h"
#include "../include/misc.h"

char *estrdup(const char * const s)
{	char *h;

	if((h = strdup(s)) == 0)
		error_out_of_memory();

	return h;
}
