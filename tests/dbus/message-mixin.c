/* Regression test for the message mixin and the echo-2 example CM.
 *
 * Copyright (C) 2007-2008 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2007-2008 Nokia Corporation
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
 */

#include <telepathy-glib/channel.h>
#include <telepathy-glib/connection.h>
#include <telepathy-glib/dbus.h>
#include <telepathy-glib/debug.h>
#include <telepathy-glib/gtypes.h>
#include <telepathy-glib/interfaces.h>

#include "examples/cm/echo-message-parts/chan.h"
#include "examples/cm/echo-message-parts/conn.h"
#include "tests/lib/myassert.h"

static int fail = 0;

static void
myassert_failed (void)
{
  fail = 1;
}

static guint received_count = 0;
static guint last_received_id = 0;
static guint last_received_sender = 0;
static guint last_received_type = 0;
static guint last_received_flags = 0;
static gchar *last_received_text = NULL;

static guint sent_count = 0;
static guint last_sent_type = 0;
static gchar *last_sent_text = NULL;

static void
on_sent (TpChannel *chan,
         guint timestamp,
         guint type,
         const gchar *text,
         gpointer data,
         GObject *object)
{
  g_print ("%p: Sent: time %u, type %u, text '%s'\n",
      chan, timestamp, type, text);

  sent_count++;
  last_sent_type = type;
  g_free (last_sent_text);
  last_sent_text = g_strdup (text);
}

static void
on_received (TpChannel *chan,
             guint id,
             guint timestamp,
             guint sender,
             guint type,
             guint flags,
             const gchar *text,
             gpointer data,
             GObject *object)
{
  TpHandleRepoIface *contact_repo = data;

  g_print ("%p: Received #%u: time %u, sender %u '%s', type %u, flags %u, "
      "text '%s'\n", chan, id, timestamp, sender,
      tp_handle_inspect (contact_repo, sender), type, flags, text);

  received_count++;
  last_received_id = id;
  last_received_sender = sender;
  last_received_type = type;
  last_received_flags = flags;
  g_free (last_received_text);
  last_received_text = g_strdup (text);
}

static guint message_received_count = 0;
static guint last_message_received_sender = 0;
static guint last_message_received_type = 0;
static guint last_message_received_n_parts = 0;

static guint message_sent_count = 0;
static guint last_message_sent_type = 0;
static gchar *last_message_sent_token = NULL;
static guint last_message_sent_n_parts = 0;

static void
print_part (gpointer k,
            gpointer v,
            gpointer user_data)
{
  const gchar *key = k;
  gchar *contents = g_strdup_value_contents (v);

  g_print ("        %s: %s\n", key, contents);
  g_free (contents);
}

static void
on_message_received (TpChannel *chan,
                     guint id,
                     guint timestamp,
                     guint sender,
                     guint type,
                     const GPtrArray *parts,
                     gpointer data,
                     GObject *object)
{
  TpHandleRepoIface *contact_repo = data;
  guint i;

  g_print ("%p: MessageReceived #%u: time %u, sender %u '%s', type %u, "
      "%u parts\n", chan, id, timestamp, sender,
      tp_handle_inspect (contact_repo, sender), type, parts->len);

  for (i = 0; i < parts->len; i++)
    {
      g_print ("    Part %u:\n", i);
      g_hash_table_foreach (g_ptr_array_index (parts, i), print_part, NULL);
    }

  message_received_count++;
  last_message_received_type = type;
  last_message_received_sender = sender;
  last_message_received_n_parts = parts->len;
}

static void
on_message_sent (TpChannel *chan,
                 guint type,
                 const GPtrArray *parts,
                 const gchar *token,
                 gpointer data,
                 GObject *object)
{
  guint i;

  g_print ("%p: MessageSent with token '%s': type %u, %u parts\n",
      chan, token, type, parts->len);

  for (i = 0; i < parts->len; i++)
    {
      g_print ("    Part %u:\n", i);
      g_hash_table_foreach (g_ptr_array_index (parts, i), print_part, NULL);
    }

  message_sent_count++;
  last_message_sent_type = type;
  last_message_sent_n_parts = parts->len;
  g_free (last_message_sent_token);
  last_message_sent_token = g_strdup (token);
}

static void
on_messages_removed (TpChannel *chan,
                     const GArray *ids,
                     gpointer data,
                     GObject *object)
{
  guint i;

  g_print ("%p: PendingMessagesRemoved: %u messages\n", chan, ids->len);

  for (i = 0; i < ids->len; i++)
    {
      g_print ("    %u\n", g_array_index (ids, guint, i));
    }
}

static GValue *
slice_new_string (const char *string)
{
  GValue *ret = tp_g_value_slice_new (G_TYPE_STRING);

  g_value_set_string (ret, string);
  return ret;
}

static GValue *
slice_new_byte_array (gconstpointer content,
                      gsize length)
{
  GValue *ret = tp_g_value_slice_new (DBUS_TYPE_G_UCHAR_ARRAY);
  GArray *arr = g_array_sized_new (FALSE, FALSE, 1, length);

  g_array_append_vals (arr, content, length);
  g_value_set_boxed (ret, arr);
  g_array_free (arr, TRUE);
  return ret;
}

int
main (int argc,
      char **argv)
{
  ExampleEcho2Connection *service_conn;
  TpBaseConnection *service_conn_as_base;
  TpHandleRepoIface *contact_repo;
  ExampleEcho2Channel *service_chan;
  TpDBusDaemon *dbus;
  TpConnection *conn;
  TpChannel *chan;
  GError *error = NULL;
  gchar *name;
  gchar *conn_path;
  gchar *chan_path;
  TpHandle handle;

  g_type_init ();
  /* tp_debug_set_flags ("all"); */

  service_conn = EXAMPLE_ECHO_2_CONNECTION (g_object_new (
        EXAMPLE_TYPE_ECHO_2_CONNECTION,
        "account", "me@example.com",
        "protocol", "example",
        NULL));
  service_conn_as_base = TP_BASE_CONNECTION (service_conn);
  MYASSERT (service_conn != NULL, "");
  MYASSERT (service_conn_as_base != NULL, "");

  MYASSERT (tp_base_connection_register (service_conn_as_base, "example",
        &name, &conn_path, &error), "");
  MYASSERT_NO_ERROR (error);

  dbus = tp_dbus_daemon_new (tp_get_bus ());
  conn = tp_connection_new (dbus, name, conn_path, &error);
  MYASSERT (conn != NULL, "");
  MYASSERT_NO_ERROR (error);

  MYASSERT (tp_connection_run_until_ready (conn, TRUE, &error, NULL),
      "");
  MYASSERT_NO_ERROR (error);

  contact_repo = tp_base_connection_get_handles (service_conn_as_base,
      TP_HANDLE_TYPE_CONTACT);
  MYASSERT (contact_repo != NULL, "");

  handle = tp_handle_ensure (contact_repo, "them@example.org", NULL, &error);
  MYASSERT_NO_ERROR (error);

  /* FIXME: exercise RequestChannel rather than just pasting on a channel */

  chan_path = g_strdup_printf ("%s/Channel", conn_path);

  service_chan = EXAMPLE_ECHO_2_CHANNEL (g_object_new (
        EXAMPLE_TYPE_ECHO_2_CHANNEL,
        "connection", service_conn,
        "object-path", chan_path,
        "handle", handle,
        NULL));

  chan = tp_channel_new (conn, chan_path, TP_IFACE_CHANNEL_TYPE_TEXT,
      TP_HANDLE_TYPE_CONTACT, handle, &error);
  MYASSERT_NO_ERROR (error);

  tp_channel_run_until_ready (chan, &error, NULL);
  MYASSERT_NO_ERROR (error);

  MYASSERT (tp_cli_channel_type_text_connect_to_received (chan, on_received,
      g_object_ref (contact_repo), g_object_unref, NULL, NULL) != NULL, "");
  MYASSERT (tp_cli_channel_type_text_connect_to_sent (chan, on_sent,
      NULL, NULL, NULL, NULL) != NULL, "");

  MYASSERT (
      tp_cli_channel_interface_messages_connect_to_message_received (chan,
          on_message_received, g_object_ref (contact_repo), g_object_unref,
          NULL, NULL) != NULL, "");
  MYASSERT (tp_cli_channel_interface_messages_connect_to_message_sent (
        chan, on_message_sent, NULL, NULL, NULL, NULL) != NULL, "");
  MYASSERT (
      tp_cli_channel_interface_messages_connect_to_pending_messages_removed (
        chan, on_messages_removed, NULL, NULL, NULL, NULL) != NULL, "");

  /* Send three messages using the old Text API:
   *
   * (normal) Hello, world!
   * (action) /me drinks coffee
   * (notice) Printer on fire
   *
   * Verify that for each of them, we get a Sent signal in the old Text
   * API, and a Received signal for the echo; also a MessageSent signal
   * in the new Messages API, and a MessageReceived signal for the echo.
   */

  g_print ("\n\n==== Starting test: NORMAL ====\n");

  sent_count = 0;
  received_count = 0;
  message_sent_count = 0;
  message_received_count = 0;
  tp_cli_channel_type_text_run_send (chan, -1,
      TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL, "Hello, world!",
      &error, NULL);
  /* wait for pending events to be delivered */
  while (received_count < 1 || message_received_count < 1)
    g_main_context_iteration (NULL, TRUE);

  MYASSERT_NO_ERROR (error);
  MYASSERT (sent_count == 1, ": %u != 1", sent_count);
  MYASSERT (received_count == 1, ": %u != 1", received_count);
  MYASSERT (last_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_sent_type);
  MYASSERT (!tp_strdiff (last_sent_text, "Hello, world!"),
      ": '%s' != '%s'", last_sent_text, "Hello, world!");
  MYASSERT (last_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_received_type);
  MYASSERT (last_received_flags == 0, ": %u != 0", last_received_flags);
  MYASSERT (last_received_sender == handle,
      ": %u != %u", last_received_sender, handle);
  MYASSERT (!tp_strdiff (last_received_text, "Hello, world!"),
      ": '%s'", last_received_text);
  MYASSERT (message_sent_count == 1, ": %u != 1", message_sent_count);
  MYASSERT (message_received_count == 1, ": %u != 1", message_received_count);
  MYASSERT (last_message_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_sent_type);
  MYASSERT (last_message_sent_n_parts == 1,
      ": %u != 1", last_message_sent_n_parts);
  MYASSERT (last_message_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_received_type);
  MYASSERT (last_message_received_sender == handle,
      ": %u != %u", last_message_received_sender, handle);
  MYASSERT (last_message_received_n_parts == 1,
      ": %u != 1", last_message_received_n_parts);

  g_print ("\n\n==== Starting test: ACTION ====\n");

  sent_count = 0;
  received_count = 0;
  message_sent_count = 0;
  message_received_count = 0;
  tp_cli_channel_type_text_run_send (chan, -1,
      TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION, "drinks coffee",
      &error, NULL);
  /* wait for pending events to be delivered */
  while (received_count < 1 || message_received_count < 1)
    g_main_context_iteration (NULL, TRUE);

  MYASSERT_NO_ERROR (error);
  MYASSERT (sent_count == 1, ": %u != 1", sent_count);
  MYASSERT (received_count == 1, ": %u != 1", received_count);
  MYASSERT (last_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION,
      ": %u != ACTION", last_sent_type);
  MYASSERT (!tp_strdiff (last_sent_text, "drinks coffee"),
      ": '%s' != '%s'", last_sent_text, "drinks coffee");
  MYASSERT (last_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION,
      ": %u != ACTION", last_received_type);
  MYASSERT (last_received_flags == 0, ": %u != 0", last_received_flags);
  MYASSERT (last_received_sender == handle,
      ": %u != %u", last_received_sender, handle);
  MYASSERT (!tp_strdiff (last_received_text,
        "drinks coffee"),
      ": '%s'", last_received_text);
  MYASSERT (message_sent_count == 1, ": %u != 1", message_sent_count);
  MYASSERT (message_received_count == 1, ": %u != 1", message_received_count);
  MYASSERT (last_message_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION,
      ": %u != ACTION", last_message_sent_type);
  MYASSERT (last_message_sent_n_parts == 1,
      ": %u != 1", last_message_sent_n_parts);
  MYASSERT (last_message_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION,
      ": %u != ACTION", last_message_received_type);
  MYASSERT (last_message_received_sender == handle,
      ": %u != %u", last_message_received_sender, handle);
  MYASSERT (last_message_received_n_parts == 1,
      ": %u != 1", last_message_received_n_parts);

  g_print ("\n\n==== Starting test: NOTICE ====\n");

  sent_count = 0;
  received_count = 0;
  message_sent_count = 0;
  message_received_count = 0;
  tp_cli_channel_type_text_run_send (chan, -1,
      TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE, "Printer on fire",
      &error, NULL);
  /* wait for pending events to be delivered */
  while (received_count < 1 || message_received_count < 1)
    g_main_context_iteration (NULL, TRUE);

  MYASSERT_NO_ERROR (error);
  MYASSERT (sent_count == 1, ": %u != 1", sent_count);
  MYASSERT (received_count == 1, ": %u != 1", received_count);
  MYASSERT (last_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE,
      ": %u != NOTICE", last_sent_type);
  MYASSERT (!tp_strdiff (last_sent_text, "Printer on fire"),
      ": '%s' != '%s'", last_sent_text, "Printer on fire");
  MYASSERT (last_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE,
      ": %u != NOTICE", last_received_type);
  MYASSERT (last_received_flags == 0, ": %u != 0", last_received_flags);
  MYASSERT (last_received_sender == handle,
      ": %u != %u", last_received_sender, handle);
  MYASSERT (!tp_strdiff (last_received_text,
        "Printer on fire"),
      ": '%s'", last_received_text);
  MYASSERT (message_sent_count == 1, ": %u != 1", message_sent_count);
  MYASSERT (message_received_count == 1, ": %u != 1", message_received_count);
  MYASSERT (last_message_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE,
      ": %u != NOTICE", last_message_sent_type);
  MYASSERT (last_message_sent_n_parts == 1,
      ": %u != 1", last_message_sent_n_parts);
  MYASSERT (last_message_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE,
      ": %u != NOTICE", last_message_received_type);
  MYASSERT (last_message_received_sender == handle,
      ": %u != %u", last_message_received_sender, handle);
  MYASSERT (last_message_received_n_parts == 1,
      ": %u != 1", last_message_received_n_parts);

  g_print ("\n\n==== Starting test: lolcat ====\n");

  /* Send a multi-part message using the Messages API.
   *
   * Again, verify that we get a Sent signal in the old Text
   * API, and a Received signal for the echo; also a MessageSent signal
   * in the new Messages API, and a MessageReceived signal for the echo.
   *
   * Because this message contains an image, we must set the
   * Channel_Text_Message_Flag_Non_Text_Content.
   */

  sent_count = 0;
  received_count = 0;
  message_sent_count = 0;
  message_received_count = 0;

#undef EXPECTED_TEXT
#define EXPECTED_TEXT ("Here is a photo of a cat:\n"\
    "[IMG: lol!]\n"\
    "It's in ur regression tests verifying ur designs!")

    {
      GPtrArray *send_parts = g_ptr_array_sized_new (3);
      GHashTable *part;
      guint i;
      gchar *token = NULL;

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "alternative", slice_new_string ("main"));
      g_hash_table_insert (part, "type", slice_new_string ("text/html"));
      g_hash_table_insert (part, "content", slice_new_string (
            "Here is a photo of a cat:<br />"
            "<img src=\"cid:lolcat\" alt=\"lol!\" /><br />"
            "It's in ur regression tests verifying ur designs!"
            ));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "alternative", slice_new_string ("main"));
      g_hash_table_insert (part, "type", slice_new_string ("text/plain"));
      g_hash_table_insert (part, "content",
          slice_new_string (EXPECTED_TEXT));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "identifier", slice_new_string ("lolcat"));
      g_hash_table_insert (part, "type", slice_new_string ("image/jpeg"));
      g_hash_table_insert (part, "content", slice_new_byte_array (
            "\xff\xd8\xff\xe0\x00\x10JFIF\x00...", 14));
      g_ptr_array_add (send_parts, part);

      tp_cli_channel_interface_messages_run_send_message (chan, -1,
          TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL, send_parts, 0 /* flags */,
          &token, &error, NULL);
      MYASSERT_NO_ERROR (error);

      /* wait for pending events to be delivered */
      while (received_count < 1 || message_received_count < 1)
        g_main_context_iteration (NULL, TRUE);

      g_print ("Sent message, got token '%s'\n", token);
      g_free (token);

      for (i = 0; i < send_parts->len; i++)
        g_hash_table_destroy (g_ptr_array_index (send_parts, i));

      g_ptr_array_free (send_parts, TRUE);
    }

  MYASSERT (sent_count == 1, ": %u != 1", sent_count);
  MYASSERT (received_count == 1, ": %u != 1", received_count);
  MYASSERT (last_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_sent_type);
  MYASSERT (!tp_strdiff (last_sent_text, EXPECTED_TEXT),
      ": '%s' != '%s'", last_sent_text, EXPECTED_TEXT);
  MYASSERT (last_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_received_type);
  MYASSERT (last_received_flags ==
      TP_CHANNEL_TEXT_MESSAGE_FLAG_NON_TEXT_CONTENT,
      ": %u != NON_TEXT_CONTENT", last_received_flags);
  MYASSERT (last_received_sender == handle,
      ": %u != %u", last_received_sender, handle);
  MYASSERT (!tp_strdiff (last_received_text, EXPECTED_TEXT),
      ": '%s'", last_received_text);
  MYASSERT (message_sent_count == 1, ": %u != 1", message_sent_count);
  MYASSERT (message_received_count == 1, ": %u != 1", message_received_count);
  MYASSERT (last_message_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_sent_type);
  MYASSERT (last_message_sent_n_parts == 3,
      ": %u != 3", last_message_sent_n_parts);
  MYASSERT (last_message_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_received_type);
  MYASSERT (last_message_received_sender == handle,
      ": %u != %u", last_message_received_sender, handle);
  MYASSERT (last_message_received_n_parts == 3,
      ": %u != 3", last_message_received_n_parts);

  g_print ("\n\n==== Starting test: lolcat with PNG alternative ====\n");

  /* This time, the non-text content has an alternative. */

  sent_count = 0;
  received_count = 0;
  message_sent_count = 0;
  message_received_count = 0;

#undef EXPECTED_TEXT
#define EXPECTED_TEXT ("Here is a photo of a cat:\n"\
    "[IMG: lol!]\n"\
    "It's in ur regression tests verifying ur designs!")

    {
      GPtrArray *send_parts = g_ptr_array_sized_new (3);
      GHashTable *part;
      guint i;
      gchar *token = NULL;

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "alternative", slice_new_string ("main"));
      g_hash_table_insert (part, "identifier", slice_new_string ("html"));
      g_hash_table_insert (part, "type", slice_new_string ("text/html"));
      g_hash_table_insert (part, "content", slice_new_string (
            "Here is a photo of a cat:<br />"
            "<img src=\"cid:lolcat\" alt=\"lol!\" /><br />"
            "It's in ur regression tests verifying ur designs!"
            ));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "alternative", slice_new_string ("main"));
      g_hash_table_insert (part, "identifier", slice_new_string ("text"));
      g_hash_table_insert (part, "type", slice_new_string ("text/plain"));
      g_hash_table_insert (part, "content",
          slice_new_string (EXPECTED_TEXT));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "identifier", slice_new_string ("jpeg"));
      g_hash_table_insert (part, "alternative", slice_new_string ("lolcat"));
      g_hash_table_insert (part, "type", slice_new_string ("image/jpeg"));
      g_hash_table_insert (part, "content", slice_new_byte_array (
            "\xff\xd8\xff\xe0\x00\x10JFIF\x00...", 14));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "identifier", slice_new_string ("png"));
      g_hash_table_insert (part, "alternative", slice_new_string ("lolcat"));
      g_hash_table_insert (part, "type", slice_new_string ("image/png"));
      g_hash_table_insert (part, "content", slice_new_byte_array (
            "\x89PNG\x0d\x0a\x1a\x0a\x00...", 12));
      g_ptr_array_add (send_parts, part);

      tp_cli_channel_interface_messages_run_send_message (chan, -1,
          TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL, send_parts, 0 /* flags */,
          &token, &error, NULL);
      MYASSERT_NO_ERROR (error);

      /* wait for pending events to be delivered */
      while (received_count < 1 || message_received_count < 1)
        g_main_context_iteration (NULL, TRUE);

      g_print ("Sent message, got token '%s'\n", token);
      g_free (token);

      for (i = 0; i < send_parts->len; i++)
        g_hash_table_destroy (g_ptr_array_index (send_parts, i));

      g_ptr_array_free (send_parts, TRUE);
    }

  MYASSERT (sent_count == 1, ": %u != 1", sent_count);
  MYASSERT (received_count == 1, ": %u != 1", received_count);
  MYASSERT (last_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_sent_type);
  MYASSERT (!tp_strdiff (last_sent_text, EXPECTED_TEXT),
      ": '%s' != '%s'", last_sent_text, EXPECTED_TEXT);
  MYASSERT (last_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_received_type);
  MYASSERT (last_received_flags ==
      TP_CHANNEL_TEXT_MESSAGE_FLAG_NON_TEXT_CONTENT,
      ": %u != NON_TEXT_CONTENT", last_received_flags);
  MYASSERT (last_received_sender == handle,
      ": %u != %u", last_received_sender, handle);
  MYASSERT (!tp_strdiff (last_received_text, EXPECTED_TEXT),
      ": '%s'", last_received_text);
  MYASSERT (message_sent_count == 1, ": %u != 1", message_sent_count);
  MYASSERT (message_received_count == 1, ": %u != 1", message_received_count);
  MYASSERT (last_message_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_sent_type);
  MYASSERT (last_message_sent_n_parts == 4,
      ": %u != 4", last_message_sent_n_parts);
  MYASSERT (last_message_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_received_type);
  MYASSERT (last_message_received_sender == handle,
      ": %u != %u", last_message_received_sender, handle);
  MYASSERT (last_message_received_n_parts == 4,
      ": %u != 4", last_message_received_n_parts);

  g_print ("\n\n==== Starting test: defragment ====\n");

  /* Send a multi-part message using the Messages API.
   * This one has multiple text/plain parts, which the Text API needs to
   * concatenate.
   */

  sent_count = 0;
  received_count = 0;
  message_sent_count = 0;
  message_received_count = 0;

#undef EXPECTED_TEXT
#define EXPECTED_TEXT ("I'm on a roll\n"\
    "I'm on a roll this time\n"\
    "I feel my luck could change\n")

    {
      GPtrArray *send_parts = g_ptr_array_sized_new (3);
      GHashTable *part;
      guint i;
      gchar *token = NULL;

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "type", slice_new_string ("text/plain"));
      g_hash_table_insert (part, "content",
          slice_new_string ("I'm on a roll\n"));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "type", slice_new_string ("text/plain"));
      g_hash_table_insert (part, "content",
          slice_new_string ("I'm on a roll this time\n"));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "type", slice_new_string ("text/plain"));
      g_hash_table_insert (part, "content",
          slice_new_string ("I feel my luck could change\n"));
      g_ptr_array_add (send_parts, part);

      tp_cli_channel_interface_messages_run_send_message (chan, -1,
          TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL, send_parts, 0 /* flags */,
          &token, &error, NULL);
      MYASSERT_NO_ERROR (error);

      /* wait for pending events to be delivered */
      while (received_count < 1 || message_received_count < 1)
        g_main_context_iteration (NULL, TRUE);

      g_print ("Sent message, got token '%s'\n", token);
      g_free (token);

      for (i = 0; i < send_parts->len; i++)
        g_hash_table_destroy (g_ptr_array_index (send_parts, i));

      g_ptr_array_free (send_parts, TRUE);
    }

  MYASSERT (sent_count == 1, ": %u != 1", sent_count);
  MYASSERT (received_count == 1, ": %u != 1", received_count);
  MYASSERT (last_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_sent_type);
  MYASSERT (!tp_strdiff (last_sent_text, EXPECTED_TEXT),
      ": '%s' != '%s'", last_sent_text, EXPECTED_TEXT);
  MYASSERT (last_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_received_type);
  MYASSERT (last_received_flags == 0, ": %u != 0", last_received_flags);
  MYASSERT (last_received_sender == handle,
      ": %u != %u", last_received_sender, handle);
  MYASSERT (!tp_strdiff (last_received_text, EXPECTED_TEXT),
      ": '%s'", last_received_text);
  MYASSERT (message_sent_count == 1, ": %u != 1", message_sent_count);
  MYASSERT (message_received_count == 1, ": %u != 1", message_received_count);
  MYASSERT (last_message_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_sent_type);
  MYASSERT (last_message_sent_n_parts == 3,
      ": %u != 3", last_message_sent_n_parts);
  MYASSERT (last_message_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_received_type);
  MYASSERT (last_message_received_sender == handle,
      ": %u != %u", last_message_received_sender, handle);
  MYASSERT (last_message_received_n_parts == 3,
      ": %u != 3", last_message_received_n_parts);

  g_print ("\n\n==== Starting test: multilingual ====\n");

  /* Send a multi-part message using the Messages API.
   * This one has multiple text/plain parts, but they're alternatives,
   * so the old Text API picks the "best" (first) one.
   */

  sent_count = 0;
  received_count = 0;
  message_sent_count = 0;
  message_received_count = 0;

#undef EXPECTED_TEXT
#define EXPECTED_TEXT "nous badgerez le coleur du stream de la video"

    {
      GPtrArray *send_parts = g_ptr_array_sized_new (3);
      GHashTable *part;
      guint i;
      gchar *token = NULL;

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "type", slice_new_string ("text/plain"));
      g_hash_table_insert (part, "alternative", slice_new_string ("alt"));
      g_hash_table_insert (part, "lang", slice_new_string ("fr_CA@collabora"));
      g_hash_table_insert (part, "content", slice_new_string (EXPECTED_TEXT));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "type", slice_new_string ("text/plain"));
      g_hash_table_insert (part, "alternative", slice_new_string ("alt"));
      g_hash_table_insert (part, "lang", slice_new_string ("en_GB"));
      g_hash_table_insert (part, "content",
          slice_new_string ("we're fixing the colour of the video stream"));
      g_ptr_array_add (send_parts, part);

      part = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
          (GDestroyNotify) tp_g_value_slice_free);
      g_hash_table_insert (part, "type", slice_new_string ("text/plain"));
      g_hash_table_insert (part, "alternative", slice_new_string ("alt"));
      g_hash_table_insert (part, "lang", slice_new_string ("en_US"));
      g_hash_table_insert (part, "content",
          slice_new_string ("we're fixing the color of the video stream"));
      g_ptr_array_add (send_parts, part);

      tp_cli_channel_interface_messages_run_send_message (chan, -1,
          TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL, send_parts, 0 /* flags */,
          &token, &error, NULL);
      MYASSERT_NO_ERROR (error);

      /* wait for pending events to be delivered */
      while (received_count < 1 || message_received_count < 1)
        g_main_context_iteration (NULL, TRUE);

      g_print ("Sent message, got token '%s'\n", token);
      g_free (token);

      for (i = 0; i < send_parts->len; i++)
        g_hash_table_destroy (g_ptr_array_index (send_parts, i));

      g_ptr_array_free (send_parts, TRUE);
    }

  MYASSERT (sent_count == 1, ": %u != 1", sent_count);
  MYASSERT (received_count == 1, ": %u != 1", received_count);
  MYASSERT (last_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_sent_type);
  MYASSERT (!tp_strdiff (last_sent_text, EXPECTED_TEXT),
      ": '%s' != '%s'", last_sent_text, EXPECTED_TEXT);
  MYASSERT (last_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_received_type);
  MYASSERT (last_received_flags == 0, ": %u != 0", last_received_flags);
  MYASSERT (last_received_sender == handle,
      ": %u != %u", last_received_sender, handle);
  MYASSERT (!tp_strdiff (last_received_text, EXPECTED_TEXT),
      ": '%s'", last_received_text);
  MYASSERT (message_sent_count == 1, ": %u != 1", message_sent_count);
  MYASSERT (message_received_count == 1, ": %u != 1", message_received_count);
  MYASSERT (last_message_sent_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_sent_type);
  MYASSERT (last_message_sent_n_parts == 3,
      ": %u != 3", last_message_sent_n_parts);
  MYASSERT (last_message_received_type == TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      ": %u != NORMAL", last_message_received_type);
  MYASSERT (last_message_received_sender == handle,
      ": %u != %u", last_message_received_sender, handle);
  MYASSERT (last_message_received_n_parts == 3,
      ": %u != 3", last_message_received_n_parts);

  g_print ("\n\n==== Listing messages ====\n");

    {
      GPtrArray *messages;

      tp_cli_channel_type_text_run_list_pending_messages (chan, -1,
          FALSE, &messages, &error, NULL);
      MYASSERT_NO_ERROR (error);

      g_print ("Freeing\n");
      g_boxed_free (TP_ARRAY_TYPE_PENDING_TEXT_MESSAGE_LIST, messages);
    }

  g_print ("\n\n==== Acknowledging messages using a wrong ID ====\n");

    {
      GArray *ids = g_array_sized_new (FALSE, FALSE, sizeof (guint), 2);
      /* we assume this ID won't be valid (implementation detail: message
       * IDs are increasing integers) */
      guint bad_id = 31337;

      g_array_append_val (ids, last_received_id);
      g_array_append_val (ids, bad_id);

      MYASSERT (
          !tp_cli_channel_type_text_run_acknowledge_pending_messages (chan, -1,
          ids, &error, NULL),
          "");
      MYASSERT (error != NULL, "");
      MYASSERT (error->domain == TP_ERRORS, "%s",
          g_quark_to_string (error->domain));
      MYASSERT (error->code == TP_ERROR_INVALID_ARGUMENT, "%u", error->code);
      g_error_free (error);
      error = NULL;

      g_array_free (ids, TRUE);

      /* The next test, "Acknowledging one message", will fail if the
       * last_received_id was acknowledged despite the error */
    }

  g_print ("\n\n==== Acknowledging one message ====\n");

    {
      GArray *ids = g_array_sized_new (FALSE, FALSE, sizeof (guint), 1);

      g_array_append_val (ids, last_received_id);

      tp_cli_channel_type_text_run_acknowledge_pending_messages (chan, -1,
          ids, &error, NULL);
      MYASSERT_NO_ERROR (error);

      g_array_free (ids, TRUE);
    }

  g_print ("\n\n==== Acknowledging all remaining messages using deprecated "
      "API ====\n");

    {
      GPtrArray *messages;

      tp_cli_channel_type_text_run_list_pending_messages (chan, -1,
          TRUE, &messages, &error, NULL);
      MYASSERT_NO_ERROR (error);

      g_print ("Freeing\n");
      g_boxed_free (TP_ARRAY_TYPE_PENDING_TEXT_MESSAGE_LIST, messages);
    }

  g_print ("\n\n==== Closing channel ====\n");

    {
      gboolean dead;

      MYASSERT (tp_cli_channel_run_close (chan, -1, &error, NULL), "");
      MYASSERT_NO_ERROR (error);
      MYASSERT (tp_proxy_get_invalidated (chan) != NULL, "");

      g_object_get (service_chan,
          "channel-destroyed", &dead,
          NULL);

      MYASSERT (dead, "");
    }

  g_print ("\n\n==== End of tests ====\n");

  MYASSERT (tp_cli_connection_run_disconnect (conn, -1, &error, NULL), "");
  MYASSERT_NO_ERROR (error);

  tp_handle_unref (contact_repo, handle);
  g_object_unref (chan);
  g_object_unref (conn);
  g_object_unref (service_chan);

  service_conn_as_base = NULL;
  g_object_unref (service_conn);
  g_object_unref (dbus);
  g_free (name);
  g_free (conn_path);
  g_free (chan_path);

  g_free (last_sent_text);
  g_free (last_received_text);
  g_free (last_message_sent_token);

  return fail;
}
