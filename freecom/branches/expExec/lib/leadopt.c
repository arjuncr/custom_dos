/*	$id$
	$Locker$	$Name$	$State$

 * Scan all leading options

	This file bases on CMDLINE.C of FreeCOM v0.81 beta 1.

	$Log$
	Revision 1.1.4.3  2001/07/16 20:28:38  skaus
	Update #9

	Revision 1.1.4.2  2001/07/05 22:18:34  skaus
	Update #5
	
	Revision 1.1.4.1  2001/07/01 22:04:31  skaus
	Update #3
	
	Revision 1.1  2001/04/12 00:33:53  skaus
	chg: new structure
	chg: If DEBUG enabled, no available commands are displayed on startup
	fix: PTCHSIZE also patches min extra size to force to have this amount
	   of memory available on start
	bugfix: CALL doesn't reset options
	add: PTCHSIZE to patch heap size
	add: VSPAWN, /SWAP switch, .SWP resource handling
	bugfix: COMMAND.COM A:\
	bugfix: CALL: if swapOnExec == ERROR, no change of swapOnExec allowed
	add: command MEMORY
	bugfix: runExtension(): destroys command[-2]
	add: clean.bat
	add: localized CRITER strings
	chg: use LNG files for hard-coded strings (hangForEver(), init.c)
		via STRINGS.LIB
	add: DEL.C, COPY.C, CBREAK.C: STRINGS-based prompts
	add: fixstrs.c: prompts & symbolic keys
	add: fixstrs.c: backslash escape sequences
	add: version IDs to DEFAULT.LNG and validation to FIXSTRS.C
	chg: splitted code apart into LIB\*.c and CMD\*.c
	bugfix: IF is now using error system & STRINGS to report errors
	add: CALL: /N
	
 */

#include "../config.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../include/command.h"
#include "../err_fcts.h"
#include "../include/cmdline.h"
#include "../include/ierror.h"

int leadOptions(char **Xline, optScanner fct, void * arg)
{ int ec = E_None;
  char *p, *q, *line;

  assert(Xline);

  p = *Xline;
  if(!p)
  	p = *Xline = "";

  while(*(line = skipdm(p))) {
    q = unquote(line, p = skip_word(line));
    if(!q) {
      error_out_of_memory();
      return E_NoMem;
    }

    if(!isoption(q)
     || ((ec = scanOption(fct, arg, q)) != E_None
          && ec != E_Ignore)) {
      myfree(q);
      break;
    }

    myfree(q);
  }

  *Xline = line;

  return ec;
}