/*
 * Simple implementation of an Observer
 *
 * Copyright © 2010 Collabora Ltd.
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

/**
 * SECTION: simple-observer
 * @title: TpSimpleObserver
 * @short_description: a subclass of #TpBaseClient implementing
 * a simple Observer
 *
 * This class makes it easier to write #TpSvcClient implementing the
 * TpSvcClientObserver interface.
 *
 * A typical simple observer would look liks this:
 * |[
 * static void
 * my_observe_channel (TpSimpleObserver *observer,
 *    TpAccount *account,
 *    TpConnection *connection,
 *    TpChannel *channel,
 *    TpChannelDispatchOperation *dispatch_operation,
 *    GList *requests,
 *    TpObserveChannelContext *context,
 *    gpointer user_data)
 * {
 *  /<!-- -->* do something useful with the channel here *<!-- -->/
 *
 *  tp_observe_channel_context_accept (context);
 * }
 *
 * client = tp_simple_observer_new (NULL, TRUE, "MyObserver",
 *     FALSE, my_observe_channel, user_data);
 *
 * tp_base_client_take_observer_filter (client,
 *      tp_channel_filter_new_for_text_chats ());
 *
 * tp_base_client_register (client, NULL);
 * ]|
 *
 * See examples/client/media-observer.c for a complete example.
 */

/**
 * TpSimpleObserver:
 *
 * Data structure representing a simple Observer implementation.
 *
 * Since: 0.11.5
 */

/**
 * TpSimpleObserverClass:
 *
 * The class of a #TpSimpleObserver.
 *
 * Since: 0.11.5
 */

/**
 * TpSimpleObserverObserveChannelImpl:
 * @observer: a #TpSimpleObserver instance
 * @account: a #TpAccount having %TP_ACCOUNT_FEATURE_CORE prepared if possible
 * @connection: a #TpConnection having %TP_CONNECTION_FEATURE_CORE prepared
 * if possible
 * @channel: a #TpChannel, having %TP_CHANNEL_FEATURE_CORE prepared if possible
 * @dispatch_operation: (allow-none): a #TpChannelDispatchOperation or %NULL;
 *  the dispatch_operation is not guaranteed to be prepared
 * @requests: (element-type TelepathyGLib.ChannelRequest): a #GList of
 *  #TpChannelRequest, all having their object-path defined but are not
 *  guaranteed to be prepared.
 * @context: a #TpObserveChannelContext representing the context of this
 *  D-Bus call
 * @user_data: arbitrary user-supplied data passed to tp_simple_observer_new()
 *
 * Signature of the implementation of the ObserveChannel method.
 *
 * This function must call either tp_observe_channel_context_accept(),
 * tp_observe_channel_context_delay() or tp_observe_channel_context_fail()
 * on @context before it returns.
 */

#include "config.h"

#include "telepathy-glib/simple-observer.h"

#define DEBUG_FLAG TP_DEBUG_CLIENT
#include "telepathy-glib/debug-internal.h"

G_DEFINE_TYPE(TpSimpleObserver, tp_simple_observer, TP_TYPE_BASE_CLIENT)

enum {
    PROP_RECOVER = 1,
    PROP_CALLBACK,
    PROP_USER_DATA,
    PROP_DESTROY,
    N_PROPS
};

struct _TpSimpleObserverPrivate
{
  TpSimpleObserverObserveChannelImpl callback;
  gpointer user_data;
  GDestroyNotify destroy;
};

static void
tp_simple_observer_init (TpSimpleObserver *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, TP_TYPE_SIMPLE_OBSERVER,
      TpSimpleObserverPrivate);
}

static void
tp_simple_observer_set_property (GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec)
{
  TpBaseClient *base = TP_BASE_CLIENT (object);
  TpSimpleObserver *self = TP_SIMPLE_OBSERVER (object);

  switch (property_id)
    {
      case PROP_RECOVER:
        tp_base_client_set_observer_recover (base, g_value_get_boolean (value));
        break;

      case PROP_CALLBACK:
        self->priv->callback = g_value_get_pointer (value);
        break;

      case PROP_USER_DATA:
        self->priv->user_data = g_value_get_pointer (value);
        break;

      case PROP_DESTROY:
        self->priv->destroy = g_value_get_pointer (value);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
  }
}

static void
tp_simple_observer_constructed (GObject *object)
{
  TpSimpleObserver *self = TP_SIMPLE_OBSERVER (object);
  void (*chain_up) (GObject *) =
    ((GObjectClass *) tp_simple_observer_parent_class)->constructed;

  g_assert (self->priv->callback != NULL);

  if (chain_up != NULL)
    chain_up (object);
}

static void
tp_simple_observer_dispose (GObject *object)
{
  TpSimpleObserver *self = TP_SIMPLE_OBSERVER (object);
  void (*dispose) (GObject *) =
    G_OBJECT_CLASS (tp_simple_observer_parent_class)->dispose;

  if (self->priv->destroy != NULL)
    {
      self->priv->destroy (self->priv->user_data);
      self->priv->destroy = NULL;
    }

  if (dispose != NULL)
    dispose (object);
}

static void
observe_channel (
    TpBaseClient *client,
    TpAccount *account,
    TpConnection *connection,
    TpChannel *channel,
    TpChannelDispatchOperation *dispatch_operation,
    GList *requests,
    TpObserveChannelContext *context)
{
  TpSimpleObserver *self = TP_SIMPLE_OBSERVER (client);

  self->priv->callback (self, account, connection, channel,
      dispatch_operation, requests, context, self->priv->user_data);
}

static void
tp_simple_observer_class_init (TpSimpleObserverClass *cls)
{
  GObjectClass *object_class = G_OBJECT_CLASS (cls);
  TpBaseClientClass *base_clt_cls = TP_BASE_CLIENT_CLASS (cls);
  GParamSpec *param_spec;

  g_type_class_add_private (cls, sizeof (TpSimpleObserverPrivate));

  object_class->set_property = tp_simple_observer_set_property;
  object_class->constructed = tp_simple_observer_constructed;
  object_class->dispose = tp_simple_observer_dispose;

  /**
   * TpSimpleObserver:recover:
   *
   * The value of the Observer.Recover D-Bus property.
   *
   * Since: 0.11.5
   */
  param_spec = g_param_spec_boolean ("recover", "Recover",
      "Observer.Recover",
      FALSE,
      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_RECOVER,
      param_spec);

  /**
   * TpSimpleObserver:callback:
   *
   * The TpSimpleObserverObserveChannelImpl callback implementing the
   * ObserveChannel D-Bus method.
   *
   * This property can't be %NULL.
   *
   * Since: 0.11.5
   */
  param_spec = g_param_spec_pointer ("callback",
      "Callback",
      "Function called when ObserveChannel is called",
      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_CALLBACK,
      param_spec);

  /**
   * TpSimpleObserver:user-data:
   *
   * The user-data pointer passed to the callback implementing the
   * ObserveChannel D-Bus method.
   *
   * Since: 0.11.5
   */
  param_spec = g_param_spec_pointer ("user-data", "user data",
      "pointer passed as user-data when ObserveChannel is called",
      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_USER_DATA,
      param_spec);

  /**
   * TpSimpleObserver:destroy:
   *
   * The #GDestroyNotify function called to free the user-data pointer when
   * the #TpSimpleObserver is destroyed.
   *
   * Since: 0.11.5
   */
  param_spec = g_param_spec_pointer ("destroy", "destroy",
      "function called to destroy the user-data when destroying the observer",
      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_DESTROY,
      param_spec);

  base_clt_cls->observe_channel = observe_channel;
}

/**
 * tp_simple_observer_new:
 * @factory: a #TpClientFactory, or %NULL
 * @recover: the value of the Observer.Recover D-Bus property
 * @name: the name of the Observer (see #TpBaseClient:name: for details)
 * @uniquify: the value of the #TpBaseClient:uniquify-name: property
 * @callback: the function called when ObserveChannel is called
 * @user_data: arbitrary user-supplied data passed to @callback
 * @destroy: called with the user_data as argument, when the #TpSimpleObserver
 * is destroyed
 *
 * Convenient function to create a new #TpSimpleObserver instance.
 *
 * If @factory is %NULL a new #TpAutomaticClientFactory will be used.
 *
 * Returns: (type TelepathyGLib.SimpleObserver): a new #TpSimpleObserver
 *
 * Since: 0.99.1
 */
TpBaseClient *
tp_simple_observer_new (TpClientFactory *factory,
    gboolean recover,
    const gchar *name,
    gboolean uniquify,
    TpSimpleObserverObserveChannelImpl callback,
    gpointer user_data,
    GDestroyNotify destroy)
{
  return g_object_new (TP_TYPE_SIMPLE_OBSERVER,
      "factory", factory,
      "recover", recover,
      "name", name,
      "uniquify-name", uniquify,
      "callback", callback,
      "user-data", user_data,
      "destroy", destroy,
      NULL);
}

/**
 * tp_simple_observer_new_with_am:
 * @account_manager: an account manager, which may not be %NULL
 * @recover: the value of the Observer.Recover D-Bus property
 * @name: the name of the Observer (see #TpBaseClient:name: for details)
 * @uniquify: the value of the #TpBaseClient:uniquify-name: property
 * @callback: the function called when ObserveChannel is called
 * @user_data: arbitrary user-supplied data passed to @callback
 * @destroy: called with the user_data as argument, when the #TpSimpleObserver
 * is destroyed
 *
 * Convenient function to create a new #TpSimpleObserver instance with a
 * specified #TpAccountManager.
 *
 * It is not necessary to prepare any features on @account_manager before
 * calling this function.
 *
 * Returns: (type TelepathyGLib.SimpleObserver): a new #TpSimpleObserver
 *
 * Since: 0.11.14
 */
TpBaseClient *
tp_simple_observer_new_with_am (TpAccountManager *account_manager,
    gboolean recover,
    const gchar *name,
    gboolean uniquify,
    TpSimpleObserverObserveChannelImpl callback,
    gpointer user_data,
    GDestroyNotify destroy)
{
  return tp_simple_observer_new (tp_proxy_get_factory (account_manager),
      recover, name, uniquify, callback, user_data, destroy);
}
