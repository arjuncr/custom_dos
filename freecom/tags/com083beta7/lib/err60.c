/* $Id: err60.c 190 2001-04-12 00:36:10Z skaus $

	Wrapper for error strings
*/

#include "../config.h"

#include "../include/command.h"
#include "../err_fcts.h"
#include "../strings.h"

void error_no_alias_name(const char name[])
{
  displayString(TEXT_ALIAS_INVALID_NAME, name);
}
