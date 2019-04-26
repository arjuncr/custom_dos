/* $Id: ctxt_mkn.c 252 2001-06-19 20:42:23Z skaus $

	Create an item name.
*/

#include "../config.h"

#include <assert.h>
#include <stdio.h>

#include "../include/context.h"

void ctxtMkItemName(char * const name
	, const Context_Tag tag
	, const unsigned num)
{	assert(name);

	ctxtCheckInfoTag(tag);
	if(num)
		sprintf(name, "%c%04x", (unsigned char)tag, num);
	else {
		*name = (unsigned char)tag;
		name[1] = 0;
	}
}
