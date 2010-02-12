/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2009 Collabora Ltd.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors: Cosimo Alfarano <cosimo.alfarano@collabora.co.uk>
 */

#include <config.h>

#include <glib.h>
#include <telepathy-glib/telepathy-glib.h>
#include <telepathy-glib/debug-sender.h>

#include <telepathy-logger/channel-factory.h>
#include <telepathy-logger/channel-text.h>
#include <telepathy-logger/observer.h>
#include <telepathy-logger/dbus-service.h>

#define DEBUG_FLAG TPL_DEBUG_MAIN
#include <telepathy-logger/debug.h>

static GMainLoop *loop = NULL;

#ifdef ENABLE_DEBUG
static TpDebugSender *debug_sender = NULL;
static gboolean stamp_logs = FALSE;


static void
log_to_debug_sender (const gchar *log_domain,
    GLogLevelFlags log_level,
    const gchar *string)
{
  GTimeVal now;

  g_return_if_fail (TP_IS_DEBUG_SENDER (debug_sender));

  g_get_current_time (&now);

  tp_debug_sender_add_message (debug_sender, &now, log_domain, log_level,
      string);
}


static void
log_handler (const gchar *log_domain,
    GLogLevelFlags log_level,
    const gchar *message,
    gpointer user_data)
{
  if (stamp_logs)
    {
      GTimeVal now;
      gchar now_str[32];
      gchar *tmp;
      struct tm tm;

      g_get_current_time (&now);
      localtime_r (&(now.tv_sec), &tm);
      strftime (now_str, 32, "%Y-%m-%d %H:%M:%S", &tm);
      tmp = g_strdup_printf ("%s.%06ld: %s",
        now_str, now.tv_usec, message);

      g_log_default_handler (log_domain, log_level, tmp, NULL);

      g_free (tmp);
    }
  else
    {
      g_log_default_handler (log_domain, log_level, message, NULL);
    }

  /* messages are already sent to the debug sender in gabble_debug. */
  if (log_level != G_LOG_LEVEL_DEBUG || tp_strdiff (log_domain, G_LOG_DOMAIN))
    log_to_debug_sender (log_domain, log_level, message);
}
#endif /* ENABLE_DEBUG */

static void
telepathy_logger_dbus_init (void)
{
  TplDBusService *dbus_srv = NULL;
  TpDBusDaemon *tp_bus = NULL;
  GError *error = NULL;


  tp_bus = tp_dbus_daemon_dup (&error);
  if (tp_bus == NULL)
    {
      g_critical ("Failed to acquire bus daemon: %s", error->message);
      goto out;
    }

  if (!tp_dbus_daemon_request_name (tp_bus, TPL_DBUS_SRV_WELL_KNOWN_BUS_NAME,
        FALSE, &error))
    {
      g_critical ("Failed to acquire bus name %s: %s",
          TPL_DBUS_SRV_WELL_KNOWN_BUS_NAME, error->message);
      goto out;
    }

  dbus_srv = tpl_dbus_service_new ();
  dbus_g_connection_register_g_object (tp_get_bus(), TPL_DBUS_SRV_OBJECT_PATH,
      G_OBJECT (dbus_srv));

out:
  if (error != NULL)
    g_error_free (error);
  g_object_unref (tp_bus);
  g_object_unref (dbus_srv);
}


int
main (int argc,
    char *argv[])
{
  TplObserver *observer;
  GError *error = NULL;

  g_type_init ();

  tp_debug_divert_messages (g_getenv ("TPL_LOGFILE"));

#ifdef ENABLE_DEBUG
  tpl_debug_set_flags_from_env ();

  stamp_logs = (g_getenv ("TPL_TIMING") != NULL);
  debug_sender = tp_debug_sender_dup ();

  g_log_set_default_handler (log_handler, NULL);
#endif /* ENABLE_DEBUG */

  tpl_channel_factory_init ();

  DEBUG ("Initialising TPL Channel Factory");
  tpl_channel_factory_add ("org.freedesktop.Telepathy.Channel.Type.Text",
      (TplChannelConstructor) tpl_channel_text_new);
  DEBUG ("- TplChannelText registred.");

  observer = tpl_observer_new ();
  DEBUG ("Registering channel factory into TplObserver");
  tpl_observer_set_channel_factory (observer, tpl_channel_factory_build);

  if (!tpl_observer_register_dbus (observer, &error))
    {
      DEBUG ("Error during D-Bus registration: %s", error->message);
      return 1;
    }

  telepathy_logger_dbus_init ();

  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  g_object_unref (observer);
  tpl_channel_factory_deinit ();

#ifdef ENABLE_DEBUG
  g_log_set_default_handler (g_log_default_handler, NULL);
  g_object_unref (debug_sender);
#endif /* ENABLE_DEBUG */

  return 0;
}
