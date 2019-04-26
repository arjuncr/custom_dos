/* $Id: is_icmd.c 254 2001-06-25 20:06:36Z skaus $

	Search for an internal command and, if present,
	return the pointer to the entry within cmdtable.
	The hidden status is ignored for this function.

	The name is already uppercased.
 */

#include "../config.h"

#include <assert.h>
#include <string.h>

#include "../include/command.h"

struct CMD *is_icmd(const char * const cname)
{	struct CMD *cmd;
	int len;

	if(cname) {
		len = strlen(cname) + 1;

		cmd = internalCommands;
		do if(memcmp(cmd->name, cname, len) == 0)
			return cmd;
		while((++cmd)->name);
	}

	return 0;
}
