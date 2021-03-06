/* $Id: lastdget.c 283 2001-08-21 18:21:21Z skaus $

	Get Last Directory

*/

#include "../config.h"

#include <assert.h>

#include "../include/context.h"
#include "../include/misc.h"
#include "../err_fcts.h"

void lastDirGet(char ** const buf)
{
	assert(buf);

	*buf = 0;
	ctxtGet(2, CTXT_TAG_LASTDIR, 1, buf);
}
