/*	$NetBSD: mkfifo.c,v 1.2 2009/11/05 15:08:19 stacktic Exp $	*/
/* from src/usr.bin/mkfifo/mkfifo.c */
/*	NetBSD: mkfifo.c,v 1.12 2008/07/21 14:19:24 lukem Exp	*/

/*
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "fs-utils.h"

#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1990, 1993\
 The Regents of the University of California.  All rights reserved.");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)mkfifo.c	8.2 (Berkeley) 1/5/94";
#endif
__RCSID("$NetBSD: mkfifo.c,v 1.2 2009/11/05 15:08:19 stacktic Exp $");
#endif /* not lint */

#if HAVE_NBCOMPAT_H
#include <nbcompat.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>

#ifdef USE_RUMP
#include <rump/rump_syscalls.h>

#include <fsu_utils.h>
#include <fsu_mount.h>

#define mkfifo(a, b) rump_sys_mkfifo(a, b)

#endif /* USE_RUMP */

int		main __P((int, char **));

static void usage __P((void));

int
main(int argc, char *argv[])
{
	int ch, exitval;
	void *set;
	mode_t mode;

	setprogname(argv[0]);
	setlocale (LC_ALL, "");

#ifdef USE_RUMP
	if (fsu_mount(&argc, &argv, MOUNT_READWRITE) != 0)
		errx(-1, NULL);
#endif

	/* The default mode is the value of the bitwise inclusive or of
	   S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP, S_IROTH, and S_IWOTH
	   modified by the file creation mask */
	mode = 0666 & ~umask(0);

	while ((ch = getopt(argc, argv, "m:")) != -1)
		switch(ch) {
		case 'm':
			if (!(set = setmode(optarg))) {
				err(1, "Cannot set file mode `%s'", optarg);
				/* NOTREACHED */
			}
			/* In symbolic mode strings, the + and - operators are
			   interpreted relative to an assumed initial mode of
			   a=rw. */
			mode = getmode(set, 0666);
			free(set);
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;
	if (argv[0] == NULL)
		usage();

	for (exitval = 0; *argv; ++argv) {
		if (mkfifo(*argv, mode) < 0) {
			warn("%s", *argv);
			exitval = 1;
		}
	}
	exit(exitval);
}

void
usage()
{

#ifdef USE_RUMP
	(void)fprintf(stderr, "usage: %s %s [-m mode] fifoname ...\n",
		      getprogname(), fsu_mount_usage());
#else
	(void)fprintf(stderr, "usage: %s [-m mode] fifoname ...\n",
	              getprogname());
#endif /* USE_RUMP */

	exit(1);
}
