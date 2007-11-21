/*
 * proxy.c - Base class for Telepathy client proxies
 *
 * Copyright (C) 2007 Collabora Ltd.
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
 */

#include "telepathy-glib/proxy.h"

#include <telepathy-glib/errors.h>

#include "dbus-internal.h"
#define DEBUG_FLAG TP_DEBUG_PROXY
#include "debug-internal.h"
#include "proxy-internal.h"

#include "_gen/signals-marshal.h"

/**
 * SECTION:proxy
 * @title: TpProxy
 * @short_description: base class for Telepathy client proxy objects
 * @see_also: #TpChannel, #TpConnection, #TpConnectionManager
 *
 * #TpProxy is a base class for Telepathy client-side proxies, which represent
 * an object accessed via D-Bus and provide access to its methods and signals.
 *
 * The header proxy.h also includes auto-generated client wrappers for the
 * Properties interface, which can be implemented by any type of object.
 */

/**
 * TpProxy:
 *
 * Opaque structure representing a Telepathy client-side proxy.
 */

/**
 * TpProxyInterfaceAddedCb:
 * @self: the proxy
 * @quark: a quark whose string value is the interface being added
 * @proxy: the #DBusGProxy for the added interface
 * @unused: unused
 *
 * The signature of a #TpProxy::interface-added signal callback.
 */

/**
 * TpProxyClass:
 * @parent_class: The parent class structure
 * @interface: If set non-zero by a subclass, #TpProxy will
 *    automatically add this interface in its constructor
 * @must_have_unique_name: If set %TRUE by a subclass, the #TpProxy
 *    constructor will fail if a well-known bus name is given
 * @_reserved_flags: Reserved for future expansion
 * @_reserved: Reserved for future expansion
 * @priv: Opaque pointer for private data
 *
 * The class of a #TpProxy.
 */
/* priv is actually a GSList of callbacks */

/**
 * TpProxyPendingCall:
 * @proxy: the TpProxy
 * @callback: the user-supplied handler
 * @user_data: user-supplied data to be passed to the handler
 * @destroy: function used to free the user-supplied data
 * @pending_call: the underlying dbus-glib pending call
 * @priv: private data used by the TpProxy implementation
 *
 * Structure representing a pending D-Bus call.
 */

/**
 * TpProxySignalConnection:
 * @proxy: the TpProxy
 * @interface: GQuark representing the D-Bus interface
 * @member: the D-Bus signal name
 * @callback: the user-supplied handler
 * @user_data: user-supplied data to be used by the handler
 * @destroy: function used to free the user-supplied data
 * @priv: private data used by the TpProxy implementation
 *
 * Structure representing a D-Bus signal connection.
 */

G_DEFINE_TYPE (TpProxy,
    tp_proxy,
    G_TYPE_OBJECT);

enum
{
  PROP_DBUS_DAEMON = 1,
  PROP_DBUS_CONNECTION,
  PROP_BUS_NAME,
  PROP_OBJECT_PATH,
  N_PROPS
};

enum {
    SIGNAL_INTERFACE_ADDED,
    SIGNAL_DESTROYED,
    N_SIGNALS
};

static guint signals[N_SIGNALS] = {0};

/**
 * tp_proxy_borrow_interface_by_id:
 * @self: the TpProxy
 * @interface: quark representing the interface required
 * @error: used to raise TP_ERROR_NOT_IMPLEMENTED if this object does not have
 *    the required interface
 *
 * <!-- -->
 *
 * Returns: a borrowed reference to a #DBusGProxy
 * for which the bus name and object path are the same as for @self, but the
 * interface is as given (or %NULL if this proxy does not implement it).
 * The reference is only valid as long as @self is.
 */
DBusGProxy *
tp_proxy_borrow_interface_by_id (TpProxy *self,
                                 GQuark interface,
                                 GError **error)
{
  DBusGProxy *proxy;

  if (!self->valid)
    {
      g_set_error (error, TP_ERRORS, TP_ERROR_INVALID_ARGUMENT,
          "Object %s has become invalid", self->object_path);

      return NULL;
    }

  proxy = g_datalist_id_get_data (&(self->interfaces), interface);

  if (proxy != NULL)
    {
      return proxy;
    }

  g_set_error (error, TP_ERRORS, TP_ERROR_NOT_IMPLEMENTED,
      "Object %s does not have interface %s",
      self->object_path, g_quark_to_string (interface));

  return NULL;
}

void
tp_proxy_invalidated (TpProxy *self, const GError *error)
{
  if (self->valid)
    {
      self->valid = FALSE;

      g_signal_emit (self, signals[SIGNAL_DESTROYED], 0, error);
    }

  if (self->dbus_daemon != NULL)
    {
      g_object_unref (self->dbus_daemon);
      self->dbus_daemon = NULL;
    }

  if (self->dbus_connection != NULL)
    {
      dbus_g_connection_unref (self->dbus_connection);
      self->dbus_connection = NULL;
    }
}

static void
tp_proxy_iface_destroyed_cb (DBusGProxy *proxy,
                             TpProxy *self)
{
  GError e = { DBUS_GERROR, DBUS_GERROR_NAME_HAS_NO_OWNER,
      "Name owner lost (service crashed?)" };

  tp_proxy_invalidated (self, &e);
}

/**
 * tp_proxy_add_interface_by_id:
 * @self: the TpProxy
 * @interface: quark representing the interface to be added
 *
 * Declare that this proxy supports a given interface, and allocate a
 * #DBusGProxy to access it.
 *
 * To use methods and signals of that interface, either call
 * tp_proxy_borrow_interface_by_id() to get the #DBusGProxy, or use the
 * tp_cli_* wrapper functions.
 *
 * If the interface is the proxy's "main interface", or has already been
 * added, then do nothing.
 *
 * Returns: the borrowed DBusGProxy
 */
DBusGProxy *
tp_proxy_add_interface_by_id (TpProxy *self,
                              GQuark interface)
{
  DBusGProxy *iface_proxy = g_datalist_id_get_data (&(self->interfaces),
      interface);

  if (iface_proxy == NULL)
    {
      DEBUG ("%p: %s", self, g_quark_to_string (interface));
      iface_proxy = dbus_g_proxy_new_for_name (self->dbus_connection,
          self->bus_name, self->object_path, g_quark_to_string (interface));

      g_signal_connect (iface_proxy, "destroy",
          G_CALLBACK (tp_proxy_iface_destroyed_cb), self);

      g_datalist_id_set_data_full (&(self->interfaces), interface,
          iface_proxy, g_object_unref);

      g_signal_emit (self, signals[SIGNAL_INTERFACE_ADDED], 0,
          (guint) interface, iface_proxy);
    }

  return iface_proxy;
}

static const gchar * const pending_call_magic = "TpProxyPendingCall";
static const gchar * const signal_conn_magic = "TpProxySignalConnection";

TpProxyPendingCall *
tp_proxy_pending_call_new (TpProxy *self,
                           GCallback callback,
                           gpointer user_data,
                           GDestroyNotify destroy)
{
  TpProxyPendingCall *ret = g_slice_new (TpProxyPendingCall);

  ret->proxy = g_object_ref (self);
  ret->callback = callback;
  ret->user_data = user_data;
  ret->destroy = destroy;
  ret->pending_call = NULL;
  ret->priv = pending_call_magic;

  return ret;
}

void
tp_proxy_pending_call_free (gpointer self)
{
  TpProxyPendingCall *data = self;

  g_return_if_fail (data->priv == pending_call_magic);

  g_object_unref (TP_PROXY (data->proxy));

  if (data->destroy != NULL)
    data->destroy (data->user_data);

  g_slice_free (TpProxyPendingCall, data);
}

TpProxySignalConnection *
tp_proxy_signal_connection_new (TpProxy *self,
                                GQuark interface,
                                const gchar *member,
                                GCallback callback,
                                gpointer user_data,
                                GDestroyNotify destroy)
{
  TpProxySignalConnection *ret = g_slice_new (TpProxySignalConnection);

  ret->proxy = g_object_ref (self);
  ret->interface = interface;
  ret->member = g_strdup (member);
  ret->callback = callback;
  ret->user_data = user_data;
  ret->destroy = destroy;
  ret->priv = signal_conn_magic;

  return ret;
}

void
tp_proxy_signal_connection_disconnect (TpProxySignalConnection *self)
{
  DBusGProxy *iface;

  g_return_if_fail (self->priv == pending_call_magic);

  iface = tp_proxy_borrow_interface_by_id (self->proxy, self->interface, NULL);

  if (iface == NULL)
    return;

  dbus_g_proxy_disconnect_signal (iface, self->member, self->callback,
      self);
}

void
tp_proxy_signal_connection_free_closure (gpointer self,
                                         GClosure *unused)
{
  TpProxySignalConnection *data = self;

  g_return_if_fail (data->priv == signal_conn_magic);

  g_object_unref (TP_PROXY (data->proxy));

  if (data->destroy != NULL)
    data->destroy (data->user_data);

  g_slice_free (TpProxySignalConnection, data);
}

static void
tp_proxy_get_property (GObject *object,
                       guint property_id,
                       GValue *value,
                       GParamSpec *pspec)
{
  TpProxy *self = TP_PROXY (object);

  switch (property_id)
    {
    case PROP_DBUS_DAEMON:
      g_value_set_object (value, self->dbus_daemon);
      break;
    case PROP_DBUS_CONNECTION:
      g_value_set_object (value, self->dbus_connection);
      break;
    case PROP_BUS_NAME:
      g_value_set_string (value, self->bus_name);
      break;
    case PROP_OBJECT_PATH:
      g_value_set_string (value, self->object_path);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
tp_proxy_set_property (GObject *object,
                       guint property_id,
                       const GValue *value,
                       GParamSpec *pspec)
{
  TpProxy *self = TP_PROXY (object);

  switch (property_id)
    {
    case PROP_DBUS_DAEMON:
      g_assert (self->dbus_daemon == NULL);
      self->dbus_daemon = g_value_dup_object (value);
      break;
    case PROP_DBUS_CONNECTION:
      g_assert (self->dbus_connection == NULL);
      self->dbus_connection = g_value_dup_boxed (value);
      break;
    case PROP_BUS_NAME:
      g_assert (self->bus_name == NULL);
      self->bus_name = g_value_dup_string (value);
      break;
    case PROP_OBJECT_PATH:
      g_assert (self->object_path == NULL);
      self->object_path = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
tp_proxy_init (TpProxy *self)
{
  DEBUG ("%p", self);
}

static GObject *
tp_proxy_constructor (GType type,
                      guint n_params,
                      GObjectConstructParam *params)
{
  GObjectClass *object_class = (GObjectClass *) tp_proxy_parent_class;
  TpProxy *self = TP_PROXY (object_class->constructor (type,
        n_params, params));
  TpProxyClass *klass = TP_PROXY_GET_CLASS (self);

  _tp_register_dbus_glib_marshallers ();

  if (klass->priv != NULL)
    {
      GSList *iter;

      for (iter = klass->priv;
           iter != NULL;
           iter = iter->next)
        g_signal_connect (self, "interface-added", G_CALLBACK (iter->data),
            NULL);
    }

  g_return_val_if_fail (self->dbus_connection != NULL, NULL);
  g_return_val_if_fail (self->object_path != NULL, NULL);
  g_return_val_if_fail (self->bus_name != NULL, NULL);

  if (klass->interface != 0)
    {
      tp_proxy_add_interface_by_id (self, klass->interface);
    }

  /* Some interfaces are stateful, so we only allow binding to a unique
   * name, like in dbus_g_proxy_new_for_name_owner() */
  if (klass->must_have_unique_name)
    {
      g_return_val_if_fail (self->bus_name[0] != ':', NULL);
    }

  self->valid = TRUE;

  return (GObject *) self;
}

static void
tp_proxy_dispose (GObject *object)
{
  TpProxy *self = TP_PROXY (object);
  GError e = { TP_ERRORS, TP_ERROR_NOT_AVAILABLE, "Proxy unreferenced" };

  if (self->dispose_has_run)
    return;
  self->dispose_has_run = TRUE;

  tp_proxy_invalidated (self, &e);

  G_OBJECT_CLASS (tp_proxy_parent_class)->dispose (object);
}

static void
tp_proxy_finalize (GObject *object)
{
  TpProxy *self = TP_PROXY (object);

  g_free (self->bus_name);
  g_free (self->object_path);

  G_OBJECT_CLASS (tp_proxy_parent_class)->finalize (object);
}

/**
 * tp_proxy_class_hook_on_interface_add:
 * @klass: A subclass of TpProxyClass
 * @callback: A signal handler for #TpProxy::interface-added
 *
 * Arrange for @callback to be connected to #TpProxy::interface-added
 * during the #TpProxy constructor. This is done sufficiently early that
 * it will see the signal for the default interface (@interface member of
 * #TpProxyClass), if any, being added.
 */
void
tp_proxy_class_hook_on_interface_add (TpProxyClass *klass,
                                      TpProxyInterfaceAddedCb callback)
{
  klass->priv = g_slist_prepend (klass->priv, callback);
}

static void
tp_proxy_class_init (TpProxyClass *klass)
{
  GParamSpec *param_spec;
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructor = tp_proxy_constructor;
  object_class->get_property = tp_proxy_get_property;
  object_class->set_property = tp_proxy_set_property;
  object_class->dispose = tp_proxy_dispose;
  object_class->finalize = tp_proxy_finalize;

  tp_proxy_class_hook_on_interface_add (klass, tp_cli_generic_add_signals);

  /**
   * TpProxy:dbus-daemon:
   *
   * The D-Bus daemon for this object. Read-only except during construction.
   */
  param_spec = g_param_spec_object ("dbus-daemon", "D-Bus daemon",
      "The D-Bus daemon used by this object", TP_TYPE_PROXY,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE |
      G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB);
  g_object_class_install_property (object_class, PROP_DBUS_DAEMON,
      param_spec);

  /**
   * TpProxy:dbus-connection:
   *
   * The D-Bus connection for this object. Read-only except during
   * construction.
   */
  param_spec = g_param_spec_boxed ("dbus-connection", "D-Bus connection",
      "The D-Bus connection used by this object", DBUS_TYPE_G_CONNECTION,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE |
      G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB);
  g_object_class_install_property (object_class, PROP_DBUS_CONNECTION,
      param_spec);

  /**
   * TpProxy:bus-name:
   *
   * The D-Bus bus name for this object. Read-only except during construction.
   */
  param_spec = g_param_spec_string ("bus-name", "D-Bus bus name",
      "The D-Bus bus name for this object", NULL,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE |
      G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB);
  g_object_class_install_property (object_class, PROP_BUS_NAME,
      param_spec);

  /**
   * TpProxy:object-path:
   *
   * The D-Bus object path for this object. Read-only except during
   * construction.
   */
  param_spec = g_param_spec_string ("object-path", "D-Bus object path",
      "The D-Bus object path for this object", NULL,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE |
      G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB);
  g_object_class_install_property (object_class, PROP_OBJECT_PATH,
      param_spec);

  /**
   * TpProxy::interface-added:
   * @self: the proxy object
   * @id: the GQuark representing the interface
   * @proxy: the dbus-glib proxy representing the interface
   *
   * Emitted when this proxy has gained an interface.
   */
  signals[SIGNAL_INTERFACE_ADDED] = g_signal_new ("interface-added",
      G_OBJECT_CLASS_TYPE (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
      0,
      NULL, NULL,
      _tp_marshal_VOID__UINT_OBJECT,
      G_TYPE_NONE, 2, G_TYPE_UINT, DBUS_TYPE_G_PROXY);

  /**
   * TpProxy::destroyed:
   * @self: the proxy object
   * @error: a GError indicating why this proxy was destroyed
   *
   * Emitted when this proxy has been destroyed and become invalid for
   * whatever reason. Any more specific signal should be emitted first.
   */
  signals[SIGNAL_DESTROYED] = g_signal_new ("destroyed",
      G_OBJECT_CLASS_TYPE (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
      0,
      NULL, NULL,
      g_cclosure_marshal_VOID__POINTER,
      G_TYPE_NONE, 1, G_TYPE_POINTER);
}
