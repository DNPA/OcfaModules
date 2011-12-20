/*
 * This file is part of tcpflow by Jeremy Elson <jelson@circlemud.org>
 * Initial Release: 7 April 1999.
 *
 * This source code is under the GNU Public License (GPL).  See
 * LICENSE for details.
 *
 * $Id: tcpflow.h,v 1.1 2007/07/24 08:06:08 oscar Exp $
 *
 * $Log: tcpflow.h,v $
 * Revision 1.1  2007/07/24 08:06:08  oscar
 * Initial sloppy checkin
 *
 * Revision 1.10  2001/08/08 19:39:40  jelson
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
 * Revision 1.9  2000/12/08 07:32:39  jelson
 * Took out the (broken) support for fgetpos/fsetpos.  Now we always simply
 * use fseek and ftell.
 *
 * Revision 1.8  1999/04/21 01:40:16  jelson
 * DLT_NULL fixes, u_char fixes, additions to configure.in, man page update
 *
 * Revision 1.7  1999/04/13 01:38:14  jelson
 * Added portability features with 'automake' and 'autoconf'.  Added AUTHORS,
 * NEWS, README, etc files (currently empty) to conform to GNU standards.
 *
 * Various portability fixes, including the FGETPOS/FSETPOS macros; detection
 * of header files using autoconf; restructuring of debugging code to not
 * need vsnprintf.
 *
 */

#ifndef __TCPFLOW_H__
#define __TCPFLOW_H__

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif
#include "conf.h" 
#include "sysdep.h"


#ifndef __SYSDEP_H__
#error something is messed up
#endif

/**************************** Constants ***********************************/

#define DEFAULT_DEBUG_LEVEL 1
#define MAX_FD_GUESS        64
#define NUM_RESERVED_FDS    5     /* number of FDs to set aside */
#define HASH_SIZE           1009  /* prime number near 1000 */
#define SNAPLEN             65536 /* largest possible MTU we'll see */


/**************************** Structures **********************************/

typedef struct {
  u_int32_t src;		/* Source IP address */
  u_int32_t dst;		/* Destination IP address */
  u_int16_t sport;		/* Source port number */
  u_int16_t dport;		/* Destination port number */
} flow_t;


typedef struct flow_state_struct {
  struct flow_state_struct *next; /* Link to next one */
  flow_t flow;			/* Description of this flow */
  tcp_seq isn;			/* Initial sequence number we've seen */
  FILE *fp;			/* Pointer to file storing this flow's data */
  long pos;			/* Current write position in fp */
  int flags;			/* Don't save any more data from this flow */
  int last_access;		/* "Time" of last access */
  char *fn;
  time_t faketime;
} flow_state_struct;

#define FLOW_FINISHED		(1 << 0)
#define FLOW_FILE_EXISTS	(1 << 1)

typedef struct flow_state_struct flow_state_t;

  
/***************************** Macros *************************************/

#define MALLOC(type, num)  (type *) check_malloc((num) * sizeof(type))

#ifndef __MAIN_C__
extern int debug_level;
#endif

#define DEBUG(message_level) if (debug_level >= message_level) debug_real

#define HASH_FLOW(flow) ( \
( (flow.sport & 0xff) | ((flow.dport & 0xff) << 8) | \
  ((flow.src & 0xff) << 16) | ((flow.dst & 0xff) << 24) \
) % HASH_SIZE)

#define IS_SET(vector, flag) ((vector) & (flag))
#define SET_BIT(vector, flag) ((vector) |= (flag))


/************************* Function prototypes ****************************/
#ifdef __cplusplus 
extern "C" {
#endif
/* main.c */

int entrypoint(const char *fn, const char *workdir);



/* util.c */
char *copy_argv(char *argv[]);
void init_debug(char *argv[]);
void *check_malloc(size_t size);
char *flow_filename(flow_t flow);
int get_max_fds(void);
void debug_real(char *fmt, ...)
#ifdef __GNUC__
                __attribute__ ((format (printf, 1, 2)))
#endif
;
void die(char *fmt, ...)
#ifdef __GNUC__
                __attribute__ ((format (printf, 1, 2)))
#endif
;

/* datalink.c */
pcap_handler find_handler(int datalink_type, char *device);

/* tcpip.c */
void process_ip(const u_char *data, u_int32_t length, struct timeval *ts);
void process_tcp(const u_char *data, u_int32_t length, u_int32_t src,
		 u_int32_t dst, struct timeval *ts);
void print_packet(flow_t flow, const u_char *data, u_int32_t length);
void store_packet(flow_t flow, const u_char *data, u_int32_t length,
		  u_int32_t seq, struct timeval *ts);
u_char *do_strip_nonprint(const u_char *data, u_int32_t length);

/* flow.c */
void touchflowfiles();	
void init_flow_state();
flow_state_t *find_flow_state(flow_t flow);
flow_state_t *create_flow_state(flow_t flow, tcp_seq isn, struct timeval *ts);
FILE *open_file(flow_state_t *flow_state, struct timeval *ts);
int close_file(flow_state_t *flow_state);
void sort_fds();
void contract_fd_ring();
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __TCPFLOW_H__ */
