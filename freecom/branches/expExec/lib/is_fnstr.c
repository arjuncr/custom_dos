/* $Id: is_fnstr.c 252 2001-06-19 20:42:23Z skaus $

	Tests if the string is constructed out of is_fnchar()
	characters only

*/

#include "../config.h"

#include <assert.h>

#include "../include/misc.h"

int is_fnstr(const char * const s)
{
	assert(s);
	return *skipfnam(s) == 0;
}
