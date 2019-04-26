/* $Id: pr_prmpt.c 1788 2018-01-28 22:21:38Z bartoldeman $
 * print the command-line prompt
 *
 */

#include "../config.h"

#include <stdlib.h>

#include "../include/command.h"
#include "../include/misc.h"

void printprompt(void)
{	char *pr;

	dbg_printmem();

	pr = getEnv(PROMPTVAR);        /* get PROMPT environment var. */

	displayPrompt(pr? pr: DEFAULT_PROMPT);
	free(pr);
}
