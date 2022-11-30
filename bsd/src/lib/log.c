#include <assert.h> /* assert(3) */
#include <errno.h>  /* errno(3) */
#include <stdio.h>  /* fprintf(3) */
#include <stdlib.h> /* exit(3) */
#include <time.h>   /* time(3), strftime(3) */

#include "lib.h"


/*
 * Standard terminal colour codes.
 */
#define TTY_BLUE   	"\x1B[34m"
#define TTY_CYAN   	"\x1B[36m"
#define TTY_GREEN   	"\x1B[32m"
#define TTY_MAGENTA	"\x1B[35m"
#define TTY_RED   	"\x1B[31m"
#define TTY_RESET 	"\x1B[0m"
#define TTY_WHITE   	"\x1B[37m"
#define TTY_YELLOW   	"\x1B[33m"


/*
 * Captions indicating the severity level of messages logged to TTY. Each
 * caption is enclosed in brackets and colour coded.
 */
#define CPN_OK 		"[" TTY_GREEN "OK" TTY_RESET "]
#define CPN_INFO 	"[" TTY_BLUE "INFO" TTY_RESET "]
#define CPN_DEBUG	"[" TTY_CYAN "DEBUG" TTY_RESET "]
#define CPN_WARN 	"[" TTY_YELLOW "WARN" TTY_RESET "]
#define CPN_FAIL 	"[" TTY_RED "FAIL" TTY_RESET "]


/*
 * Helper function to print a coloured timestamped message to TTY along with a
 * caption.
 */
static void
tty_print(const char *cpn, const char *msg)
{
	char 	bfr[32];
	time_t	now;

	now = time(NULL);
	strftime(bfr, sizeof(bfr), "", localtime(&now));
	fprintf(stderr, "%s " TTY_MAGENTA "%s" TTY_RESET ": %s\n",
	    cpn, bfr, msg);
}


/*
 * Logs an OK message to stderr.
 */
void
rps_log_ok(const char *msg)
{
	assert(msg && *msg && "message must be valid string");
	tty_print(CPN_OK, msg);
}


/*
 * Logs an INFO message to stderr.
 */
void
rps_log_info(const char *msg)
{
	assert(msg && *msg && "message must be valid string");
	tty_print(CPN_INFO, msg);
}


/*
 * Logs a DEBUG message to stderr.
 */
void
rps_log_debug(const char *msg)
{
	assert(msg && *msg && "message must be valid string");
	tty_print(CPN_DEBUG, msg);
}


/*
 * Logs a WARN message to stderr.
 */
void
rps_log_warn(const char *msg)
{
	assert(msg && *msg && "message must be valid string");
	tty_print(CPN_WARN, msg);
}


/*
 * Logs a FAIL message to stderr.
 */
void
rps_log_fail(const char *msg, int erno)
{
	assert(msg && *msg && "message must be valid string");
	tty_print(CPN_FAIL, msg);
	errno = erno;
	exit(erno);
}
