/* Simple utility code used by the regression tests.
 *
 * Copyright © 2008-2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright © 2008 Nokia Corporation
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
 */

#ifndef TEST_LIB_UTIL_H
#define TEST_LIB_UTIL_H

#include <telepathy-glib/telepathy-glib.h>

void test_proxy_run_until_dbus_queue_processed (gpointer proxy);

void test_connection_run_until_dbus_queue_processed (TpConnection *connection);

TpHandle test_connection_run_request_contact_handle (TpConnection *connection,
    const gchar *id);

void test_connection_manager_run_until_ready (TpConnectionManager *cm);

#define test_assert_no_error(e) _test_assert_no_error (e, __FILE__, __LINE__)

void _test_assert_no_error (const GError *error, const char *file, int line);

#endif
