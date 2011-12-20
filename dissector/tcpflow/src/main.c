/*
 * This file is part of tcpflow by Jeremy Elson <jelson@circlemud.org>
 * Initial Release: 7 April 1999.
 *
 * This source code is under the GNU Public License (GPL).  See
 * LICENSE for details.
 *
 * $Id: main.c,v 1.1 2007/07/24 08:06:08 oscar Exp $
 *
 * $Log: main.c,v $
 * Revision 1.1  2007/07/24 08:06:08  oscar
 * Initial sloppy checkin
 *
 * Revision 1.15  2003/08/07 07:35:24  jelson
 * fixed format string attack
 *
 * Revision 1.14  2001/08/08 19:39:40  jelson
 * ARGH!  These are changes that made up tcpflow 0.20, which for some reason I
 * did not check into the repository until now.  (Which of couse means
 * I never tagged v0.20.... argh.)
 *
 * Changes include:
 *
 *   -- portable signal handlers now used to do proper termination
 *
 *   -- patch to allow tcpflow to read from tcpdump stored captures
 *
 * Revision 1.13  2001/02/26 23:01:30  jelson
 * Added patch for -r option
 *
 * Revision 1.12  1999/04/21 01:40:14  jelson
 * DLT_NULL fixes, u_char fixes, additions to configure.in, man page update
 *
 * Revision 1.11  1999/04/20 19:39:18  jelson
 * changes to fix broken localhost (DLT_NULL) handling
 *
 * Revision 1.10  1999/04/14 22:19:56  jelson
 * cosmetic change to help screen
 *
 * Revision 1.9  1999/04/14 00:20:45  jelson
 * documentation updates, and added -h option to print usage information
 *
 * Revision 1.8  1999/04/13 23:17:55  jelson
 * More portability fixes.  All system header files now conditionally
 * included from sysdep.h.
 *
 * Integrated patch from Johnny Tevessen <j.tevessen@gmx.net> for Linux
 * systems still using libc5.
 *
 * Revision 1.7  1999/04/13 03:17:45  jelson
 * documentation updates
 *
 * Revision 1.6  1999/04/13 01:38:12  jelson
 * Added portability features with 'automake' and 'autoconf'.  Added AUTHORS,
 * NEWS, README, etc files (currently empty) to conform to GNU standards.
 *
 * Various portability fixes, including the FGETPOS/FSETPOS macros; detection
 * of header files using autoconf; restructuring of debugging code to not
 * need vsnprintf.
 *
 */

static char *cvsid = "$Id: main.c,v 1.1 2007/07/24 08:06:08 oscar Exp $";

#define __MAIN_C__

#include "tcpflow.h"
#include <sys/stat.h>
#include <sys/types.h>

int debug_level = DEFAULT_DEBUG_LEVEL;
int no_promisc = 0;
int bytes_per_flow = 0;
int max_flows = 0;
int max_desired_fds = 0;
int console_only = 0;
int strip_nonprint = 0;

char error[PCAP_ERRBUF_SIZE];


int entrypoint(const char *fname, const char *workdir)
{
  extern int optind;
  extern int opterr;
  extern int optopt;
  extern char *optarg;
  int arg, dlt, user_expression = 0;
  int need_usage = 0;

  char *device = NULL;
  char *infile = NULL;
  char *expression = NULL;
  pcap_t *pd;
  struct bpf_program fcode;
  pcap_handler handler;

  opterr = 0;
  infile = fname;

  /* hello, world */
  /* Credits: %s version %s by Jeremy Elson <jelson@circlemud.org>", PACKAGE, VERSION); */

    /* open the capture file */
    if ((pd = pcap_open_offline(infile, error)) == NULL)
      die("%s", error);

    /* get the handler for this kind of packets */
    dlt = pcap_datalink(pd);
    handler = find_handler(dlt, infile);
  

  /* add 'ip' to the user-specified filtering expression (if any) to
   * prevent non-ip packets from being delivered. */
  expression = "ip";
  user_expression = 0;

  /* If DLT_NULL is "broken", giving *any* expression to the pcap
   * library when we are using a device of type DLT_NULL causes no
   * packets to be delivered.  In this case, we use no expression, and
   * print a warning message if there is a user-specified expression */
#ifdef DLT_NULL_BROKEN
  if (dlt == DLT_NULL && expression != NULL) {
    free(expression);
    expression = NULL;
  }
#endif /* DLT_NULL_BROKEN */

  /* install the filter expression in libpcap */
  if (pcap_compile(pd, &fcode, expression, 1, 0) < 0)
    die("%s", pcap_geterr(pd));

  if (pcap_setfilter(pd, &fcode) < 0)
    die("%s", pcap_geterr(pd));

  /* initialize our flow state structures */
  init_flow_state();

  
  /* OZZFIX Try to create the output directory */

  chdir(workdir);
  if (mkdir("output", 0777) == -1){
     fprintf(stderr, "Could not create output directory. Exiting.\n");
     return 1;
  }
  
  if (pcap_loop(pd, -1, handler, NULL) < 0)
    die("%s", pcap_geterr(pd));

  touchflowfiles();

  /* ALSO CLEAN HASH TABLE !!!! */
  
  return 0;
}
