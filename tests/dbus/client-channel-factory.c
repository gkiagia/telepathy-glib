/* Tests of TpAutomaticChannelFactory and TpBasicProxyFactory
 *
 * Copyright © 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
 */

#include <string.h>

#include <telepathy-glib/telepathy-glib.h>

#include "tests/lib/util.h"
#include "tests/lib/simple-conn.h"
#include "tests/lib/stream-tube-chan.h"

typedef struct {
    GMainLoop *mainloop;
    TpDBusDaemon *dbus;

    /* Service side objects */
    TpBaseConnection *base_connection;
    TpTestsStreamTubeChannel *tube_chan_service;
    TpHandleRepoIface *contact_repo;

    /* Client side objects */
    TpConnection *connection;

    TpClientChannelFactoryInterface *factory;

    GError *error /* initialized where needed */;
    gint wait;
} Test;

static void
create_tube_service (Test *test)
{
  gchar *chan_path;
  TpHandle handle, alf_handle;
  GType type;

  tp_clear_object (&test->tube_chan_service);

  /* Create service-side tube channel object */
  chan_path = g_strdup_printf ("%s/Channel",
      tp_proxy_get_object_path (test->connection));

  test->contact_repo = tp_base_connection_get_handles (test->base_connection,
      TP_HANDLE_TYPE_CONTACT);
  g_assert (test->contact_repo != NULL);

  handle = tp_handle_ensure (test->contact_repo, "bob", NULL, &test->error);
  type = TP_TESTS_TYPE_CONTACT_STREAM_TUBE_CHANNEL;

  g_assert_no_error (test->error);

  alf_handle = tp_handle_ensure (test->contact_repo, "alf", NULL, &test->error);
  g_assert_no_error (test->error);

  test->tube_chan_service = g_object_new (
      type,
      "connection", test->base_connection,
      "handle", handle,
      "requested", TRUE,
      "object-path", chan_path,
      "initiator-handle", alf_handle,
      NULL);

  g_free (chan_path);

  tp_handle_unref (test->contact_repo, handle);
}

static void
setup (Test *test,
       gconstpointer data)
{
  test->mainloop = g_main_loop_new (NULL, FALSE);
  test->dbus = tp_tests_dbus_daemon_dup_or_die ();

  test->error = NULL;

  /* Create (service and client sides) connection objects */
  tp_tests_create_and_connect_conn (TP_TESTS_TYPE_SIMPLE_CONNECTION,
      "me@test.com", &test->base_connection, &test->connection);

  create_tube_service (test);
}

static void
teardown (Test *test,
          gconstpointer data)
{
  g_clear_error (&test->error);

  tp_clear_object (&test->dbus);
  g_main_loop_unref (test->mainloop);
  test->mainloop = NULL;

  tp_clear_object (&test->tube_chan_service);

  tp_cli_connection_run_disconnect (test->connection, -1, &test->error, NULL);
  g_assert_no_error (test->error);

  g_object_unref (test->connection);
  g_object_unref (test->base_connection);

  tp_clear_object (&test->factory);
}

static void
test_basic_creation (Test *test,
    gconstpointer data G_GNUC_UNUSED)
{
  test->factory = TP_CLIENT_CHANNEL_FACTORY (tp_basic_proxy_factory_new ());

  g_assert (TP_IS_BASIC_PROXY_FACTORY (test->factory));
  g_assert (TP_IS_CLIENT_CHANNEL_FACTORY (test->factory));
}

static void
test_auto_creation (Test *test,
    gconstpointer data G_GNUC_UNUSED)
{
  test->factory = TP_CLIENT_CHANNEL_FACTORY (
      tp_automatic_channel_factory_new ());

  g_assert (TP_IS_AUTOMATIC_CHANNEL_FACTORY (test->factory));
  g_assert (TP_IS_CLIENT_CHANNEL_FACTORY (test->factory));
}

int
main (int argc,
      char **argv)
{
  g_type_init ();
  tp_debug_set_flags ("all");

  g_test_init (&argc, &argv, NULL);
  g_test_bug_base ("http://bugs.freedesktop.org/show_bug.cgi?id=");

  g_test_add ("/client-channel-factory/basic/creation", Test, NULL, setup,
      test_basic_creation, teardown);
  g_test_add ("/client-channel-factory/auto/creation", Test, NULL, setup,
      test_auto_creation, teardown);

  return g_test_run ();
}
