/* $Id: err61.c 190 2001-04-12 00:36:10Z skaus $

	Wrapper for error strings
*/

#include "../config.h"

#include "../include/command.h"
#include "../err_fcts.h"
#include "../strings.h"

void error_history_size(const char s[])
{
  displayString(TEXT_ERROR_HISTORY_SIZE, s);
}
