/* $Id: err63.c 190 2001-04-12 00:36:10Z skaus $

	Wrapper for error strings
*/

#include "../config.h"

#include "../include/command.h"
#include "../err_fcts.h"
#include "../strings.h"

void error_no_context_after_swap(void)
{
	displayString(TEXT_ERROR_CONTEXT_AFTER_SWAP);
}
