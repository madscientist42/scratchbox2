/*
 * sb_l10n.c -- exec_policy based localization support.
 *
 * Copyright (C) 2008 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsb2.h"
#include "exported.h"

static void check_textdomain(const char *);

char *bindtextdomain_gate(char *(*realfn)(const char *, const char *),
    const char *realfn_name, const char *domainname, const char *dirname)
{
	mapping_results_t res;
	const char *mapped_dirname = dirname;
	char *result = NULL;

	clear_mapping_results_struct(&res);

	if (dirname != NULL) { 
		sbox_map_path(realfn_name, dirname, 0, &res);
		assert(res.mres_result_path != NULL);

		mapped_dirname = res.mres_result_path;

		SB_LOG(SB_LOGLEVEL_DEBUG, "binding domain %s to %s",
		    domainname, mapped_dirname);
	}

	result = (*realfn)(domainname, mapped_dirname);
	free_mapping_results(&res);
	return (result);
}

static void check_textdomain(const char *domainname)
{
	/*
	 * We read back current bindings for given domainname and
	 * call bindtextdomain() which then goes into our gate
	 * function that maps the path if exec policy says so.
	 */
	(void) bindtextdomain(domainname, bindtextdomain(domainname, NULL));
}

/*ARGSUSED*/
char *textdomain_gate(char *(*realfn)(const char *), const char *realfn_name,
    const char *domainname)
{
	(void)realfn_name;

	check_textdomain(domainname);
	return (*realfn)(domainname);
}

/*ARGSUSED*/
char *dgettext_gate(char *(*realfn)(const char *, const char *),
    const char *realfn_name, const char *domainname, const char *msgid)
{
	(void)realfn_name;

	check_textdomain(domainname);
	return (*realfn)(domainname, msgid);
}

/*ARGSUSED*/
char *dcgettext_gate(char *(*realfn)(const char *, const char *, int),
    const char *realfn_name, const char *domainname, const char *msgid,
    int category)
{
	(void)realfn_name;

	check_textdomain(domainname);
	return (*realfn)(domainname, msgid, category);
}

/*ARGSUSED*/
char *dngettext_gate(char *(*realfn)(const char *, const char *, const char *,
    unsigned long int n), const char *realfn_name,
    const char *domainname, const char *msgid,
    const char *msgid_plural, unsigned long int n)
{
	(void)realfn_name;

	check_textdomain(domainname);
	return (*realfn)(domainname, msgid, msgid_plural, n);
}

/*ARGSUSED*/
char *dcngettext_gate(char *(*realfn)(const char *, const char *, const char *,
    unsigned long int, int), const char *realfn_name,
    const char *domainname, const char *msgid,
    const char *msgid_plural, unsigned long int n, int category)
{
	(void)realfn_name;

	check_textdomain(domainname);
	return (*realfn)(domainname, msgid, msgid_plural, n, category);
}
