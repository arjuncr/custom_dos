/*	This is an automatic generated file

	DO NOT EDIT! SEE ERR_FCTS.SRC and SCANERR.PL.

	Error printing function providing a wrapper for STRINGS
 */

#include "../config.h"

#include "../include/message.h"
#include "../err_fcts.h"
#include "../strings.h"

#undef error_dirfct_failed
void error_dirfct_failed(const char * const fctname, const char * const dirname)
{	displayError(TEXT_ERROR_DIRFCT_FAILED, fctname, dirname);
}