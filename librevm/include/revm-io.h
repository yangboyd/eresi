/*
** revm.h for librevm in elfsh
** 
** Started on  Thu Feb 22 07:19:04 2001 mayhem
** 
** Moved from elfsh to librevm on January 2007 -may
**
**
** $Id: revm-io.h,v 1.5 2007-03-17 13:06:46 may Exp $
**
*/
#ifndef __REVM_IO_H_
 #define __REVM_IO_H_

#include <regex.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "librevm-color.h"

#if defined(ELFSHNET)
 #include <libdump.h>
#endif

/* Network related defines */
#define REVM_PORT		4444
#define REVM_DUMP_PORT		4445
#define REVM_MAX_CLIENTS	4
#define REVM_MAX_SEND_SIZE	2048

/* DUMP related defines */
#define REVM_DUMP_CMD	1
#define REVM_DUMP_MSG	2

extern u_char quit_msg_setup;

char	quit_msg[512];
void	(*prompt_token_setup)(char *name, u_int size);
char	prompt_token[512];

#define ELFSH_SNAME	"elfsh"
#define	ELFSH_VERSION	"0.77"
#define	ELFSH_RELEASE	"b1"
#define ELFSH_EDITION	"dev"

/* Unused, feel free to try it, its awesome */
#define ELFSH_CIRCUS_PROMPT \
"\033[00;01;30m("        \
"\033[00;01;31me"        \
"\033[00;01;32ml"        \
"\033[00;01;33mf"        \
"\033[00;01;34ms"        \
"\033[00;01;35mh"        \
"\033[00;01;36m-"        \
"\033[00;01;30m"         \
 ELFSH_VERSION           \
"\033[00;01;35m-"        \
"\033[05;01;35mcircus"   \
"\033[00;01;35m-edition" \
"\033[00;01;30m)"        \
"\033[00m " 

/* The ELF shell crew prompt */
#define ELFSH_CREW_PROMPT  \
"\033[00;01;30m("          \
 "\033[00;01;31melfsh"     \
 "\033[00;01;30m-"         \
 "\033[00;01;33m"          \
 ELFSH_VERSION             \
 "\033[00;01;30m-"         \
 "\033[00;01;33m"          \
 ELFSH_RELEASE             \
 "\033[00;01;30m-"         \
 "\033[00;01;32mdevhell"   \
 "\033[00;01;30m)"         \
 "\033[00m " 

/* Be original, do your own */
#define  ELFSH_PROMPT ELFSH_CREW_PROMPT
//#define ELFSH_PROMPT ELFSH_CIRCUS_PROMPT

#define GVZ_COLOR_BLUE		"\"blue\""
#define GVZ_COLOR_CORAL		"\"coral\""
#define GVZ_COLOR_CYAN		"\"cyan\""
#define GVZ_COLOR_RED		"\"red\""
#define GVZ_COLOR_GREEN		"\"green\""
#define GVZ_COLOR_BLACK		"\"black\""

#define BLK_COLOR_FUNC		GVZ_COLOR_GREEN
#define BLK_COLOR_NORM		GVZ_COLOR_CYAN
#define BLK_COLOR_RET		GVZ_COLOR_CORAL
#define BLK_COLOR_TRUE		GVZ_COLOR_GREEN
#define BLK_COLOR_FALSE		GVZ_COLOR_RED
#define BLK_COLOR_CONT		GVZ_COLOR_BLACK
#define BLK_COLOR_CALL		GVZ_COLOR_BLUE
#define BLK_COLOR_JUMP		GVZ_COLOR_BLACK

/* Return of an input function in case of ignorable input */
#define ELFSH_VOID_INPUT	-1
#define ELFSH_EXIT_INPUT	-2

/* Elfsh Output Caching structure */
typedef struct          s_outbuf
{
  int			nblines;
  unsigned int          cols;
  unsigned int          lines;
  char                  ignore;
}			revmoutbuf_t;


/* Input / Output template for ELFsh */
typedef struct	s_io
{
  
#define		ELFSH_IOSTD     1
#define		ELFSH_IONET     2
#define		ELFSH_IODUMP    3
#define		ELFSH_IOFIFO	4
#define		ELFSH_IONUM     5
  char		type;                   /* IO type           */
  int		input_fd;               /* Input file        */
  int		output_fd;              /* Output file       */
  char		*(*input)();            /* Read Input data   */
  int		(*output)(char *buf);   /* Write output data */
  
  /* dump specific */
#if defined(ELFSHNET)
  pkt_t		*pkt;                   /* dump received pkt */
#endif

  int		new;                   /* 0 if already used */

#if defined(USE_READLN)
  char		*buf;                  /* readline line */
  char		*savebuf;
  int		rl_point;
  int		rl_end;
#endif

  revmoutbuf_t	outcache;
}               revmio_t;




/* REVM socket structure */
typedef struct       s_socket
{
  struct sockaddr_in addr;        /* sockaddr_in struct */
  int                socket;      /* The socket */
  char               **recvd;     /* List of received buffer */
  
  /* XXX: NEW/OLD is not explicit enough, rename ... */
#define NEW   1
#define OLD   0
  int                recvd_f;     /* NEW if the buffer was not passed to the parser yet */
#define YES 1
#define NO  0
  int                ready_f;     /* Have we received the trailing \n ? */
}                    revmsock_t;



/* Screen cache for each workspace */
typedef struct        s_screen
{
  unsigned int        x;              /* Screen height */
  unsigned int        y;              /* Screen width */
  char                *buf;           /* Screen buffer */
  char                *head;          /* Buffer's beginning */
  char                *tail;          /* Buffer's end */
}                     revmscreen_t;


/* REVM job structure, one per client */
typedef struct		s_workspace
{
#define			ELFSH_INPUT     0
#define			ELFSH_OUTPUT    1
  revmio_t		io;		  /* Current IO for this job */
  revmsock_t		sock;		  /* Unused in initial job */
  u_char		active;		  /* Is the workspace active ? */
  time_t		createtime;       /* Workspace creation time */
  int			logfd;            /* Log file descriptor */
  revmscreen_t		screen;           /* Last printed screen */
  char			*oldline;	  /* Previous command line */
#define       ELFSH_JOB_LOGGED (1 << 0)
  u_char                state;            /* Job state flags */
  char			*name;		  /* Name of the job */
}			revmworkspace_t;


/* Hold all the VM flags, sort of global context */
typedef struct        s_state
{
  char                vm_quiet;       /* Quiet mode : 0 or 1 */
  char                vm_gvl;         /* Graph verbose level : 0 or 1 */
  char                vm_force;       /* Force mode : 0 or 1 */
  char                vm_use_regx;    /* Is a global regx available ? */
  regex_t             vm_regx;        /* Global regx */
  char                *vm_sregx;      /* Global regx in string format */
  char                vm_use_alert;   /* Is an alert regx available ? */
  regex_t	      vm_alert;       /* Alert regx */
  char                *vm_salert;     /* ALert regx in string format */
  char                *sort;          /* Actual sorting choice */
  char                *input;         /* Implicit File input (-f option) */
  char                *output;        /* Implicit File output (-w option) */
#define       REVM_STATE_CMDLINE	0
#define       REVM_STATE_SCRIPT		1
#define       REVM_STATE_INTERACTIVE    2
#define       REVM_STATE_DEBUGGER	3
  char                vm_mode;        /* Command line, scripting, interactive, debugger ? */
#define	      REVM_SIDE_CLIENT		0
#define	      REVM_SIDE_SERVER		1
  char		      vm_side;	      /* Useful for the bi-partite debugger */
  char		      vm_sourcing;    /* Are we sourcing a script ? */
  char                vm_stopped;     /* We are in a signal handler */
  char                vm_shared;      /* Next opened object must be shared */
  char                vm_net;         /* We are a node connected to the elf network */
  u_int               lastid;         /* Last Object ID */
}		       revmstate_t;

extern int		 elfsh_net_client_count;


/* Parsing, Scanning, I/O functions */
char            *vm_getln();
int		vm_initio();
char            **vm_input(int *argc);
char		*vm_stdinput();
int		vm_flush();
int		vm_output(char *str);
int		vm_output_nolog(char *str);
int		vm_outerr(char *str);
int		vm_output_bcast(char *str);
int		vm_stdoutput(char *str);
int		vm_display_prompt();
void		vm_ln_handler (char *c);
void		vm_setinput(revmworkspace_t *j, int fd);
void		vm_setoutput(revmworkspace_t *j, int fd);
void		vm_log(char *str);
int             vm_closelog();
char		*vm_get_prompt();
void		vm_set_prompt(void (*func) (char *name, u_int size));

/* Network related functions */
int		vm_net_init();
void		vm_check_net_select(fd_set *sel_sockets, int sock);
int		vm_select();
char*		vm_net_input();
int		vm_net_output(char *buf);
int		vm_net_recvd();
int		vm_net_accept();
int		vm_dump_accept();
int		vm_socket_del(char *inet_addr);
int		vm_socket_get_nb_recvd(char *inet);
int		vm_update_recvd(revmsock_t *socket);
char*		vm_socket_merge_recvd(revmsock_t *socket);
int             vm_getmaxfd();
int             vm_clean_jobs();


#endif