/* $Id: pr_prmpt.c 190 2001-04-12 00:36:10Z skaus $
 * print the command-line prompt
 *
 */

#include "../config.h"


#include "../include/command.h"
#include "../include/misc.h"

void printprompt(void)
{	char *pr;

	dbg_printmem();

	pr = getEnv(PROMPTVAR);        /* get PROMPT environment var. */

	displayPrompt(pr? pr: DEFAULT_PROMPT);
}

