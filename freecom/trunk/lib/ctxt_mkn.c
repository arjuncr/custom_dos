/* $Id: ctxt_mkn.c 1816 2018-04-26 01:38:22Z bartoldeman $

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

	if(num)
		sprintf(name, "%c%04x", (unsigned char)tag, num);
	else {
		*name = (unsigned char)tag;
		name[1] = 0;
	}
}
