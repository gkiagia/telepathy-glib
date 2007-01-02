/* Generated from the Telepathy spec, version 0.14.0

Copyright (C) 2005, 2006 Collabora Limited
Copyright (C) 2005, 2006 Nokia Corporation
Copyright (C) 2006 INdT

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


*/

#ifdef __cplusplus
extern "C" {
#endif


/* TpConnMgrParamFlags (bitfield/set of flags, 0 for none) */

typedef enum {
    TP_CONN_MGR_PARAM_FLAG_REQUIRED = 1,
#   define TP_CONN_MGR_PARAM_FLAG_REQUIRED (TP_CONN_MGR_PARAM_FLAG_REQUIRED)
    TP_CONN_MGR_PARAM_FLAG_REGISTER = 2,
#   define TP_CONN_MGR_PARAM_FLAG_REGISTER (TP_CONN_MGR_PARAM_FLAG_REGISTER)
    TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT = 4,
#   define TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT (TP_CONN_MGR_PARAM_FLAG_HAS_DEFAULT)
} TpConnMgrParamFlags;


/* TpConnectionHandleType (enum) */

typedef enum {
    TP_HANDLE_TYPE_NONE = 0,
#   define TP_HANDLE_TYPE_NONE (TP_HANDLE_TYPE_NONE)
    TP_HANDLE_TYPE_CONTACT = 1,
#   define TP_HANDLE_TYPE_CONTACT (TP_HANDLE_TYPE_CONTACT)
    TP_HANDLE_TYPE_ROOM = 2,
#   define TP_HANDLE_TYPE_ROOM (TP_HANDLE_TYPE_ROOM)
    TP_HANDLE_TYPE_LIST = 3,
#   define TP_HANDLE_TYPE_LIST (TP_HANDLE_TYPE_LIST)
    TP_HANDLE_TYPE_GROUP = 4,
#   define TP_HANDLE_TYPE_GROUP (TP_HANDLE_TYPE_GROUP)
#   define LAST_TP_HANDLE_TYPE (4)
} TpConnectionHandleType;


/* TpConnectionStatus (enum) */

typedef enum {
    TP_CONNECTION_STATUS_CONNECTED = 0,
#   define TP_CONNECTION_STATUS_CONNECTED (TP_CONNECTION_STATUS_CONNECTED)
    TP_CONNECTION_STATUS_CONNECTING = 1,
#   define TP_CONNECTION_STATUS_CONNECTING (TP_CONNECTION_STATUS_CONNECTING)
    TP_CONNECTION_STATUS_DISCONNECTED = 2,
#   define TP_CONNECTION_STATUS_DISCONNECTED (TP_CONNECTION_STATUS_DISCONNECTED)
#   define LAST_TP_CONNECTION_STATUS (2)
} TpConnectionStatus;


/* TpConnectionStatusReason (enum) */

typedef enum {
    TP_CONNECTION_STATUS_REASON_NONE_SPECIFIED = 0,
#   define TP_CONNECTION_STATUS_REASON_NONE_SPECIFIED (TP_CONNECTION_STATUS_REASON_NONE_SPECIFIED)
    TP_CONNECTION_STATUS_REASON_REQUESTED = 1,
#   define TP_CONNECTION_STATUS_REASON_REQUESTED (TP_CONNECTION_STATUS_REASON_REQUESTED)
    TP_CONNECTION_STATUS_REASON_NETWORK_ERROR = 2,
#   define TP_CONNECTION_STATUS_REASON_NETWORK_ERROR (TP_CONNECTION_STATUS_REASON_NETWORK_ERROR)
    TP_CONNECTION_STATUS_REASON_AUTHENTICATION_FAILED = 3,
#   define TP_CONNECTION_STATUS_REASON_AUTHENTICATION_FAILED (TP_CONNECTION_STATUS_REASON_AUTHENTICATION_FAILED)
    TP_CONNECTION_STATUS_REASON_ENCRYPTION_ERROR = 4,
#   define TP_CONNECTION_STATUS_REASON_ENCRYPTION_ERROR (TP_CONNECTION_STATUS_REASON_ENCRYPTION_ERROR)
    TP_CONNECTION_STATUS_REASON_NAME_IN_USE = 5,
#   define TP_CONNECTION_STATUS_REASON_NAME_IN_USE (TP_CONNECTION_STATUS_REASON_NAME_IN_USE)
    TP_CONNECTION_STATUS_REASON_CERT_NOT_PROVIDED = 6,
#   define TP_CONNECTION_STATUS_REASON_CERT_NOT_PROVIDED (TP_CONNECTION_STATUS_REASON_CERT_NOT_PROVIDED)
    TP_CONNECTION_STATUS_REASON_CERT_UNTRUSTED = 7,
#   define TP_CONNECTION_STATUS_REASON_CERT_UNTRUSTED (TP_CONNECTION_STATUS_REASON_CERT_UNTRUSTED)
    TP_CONNECTION_STATUS_REASON_CERT_EXPIRED = 8,
#   define TP_CONNECTION_STATUS_REASON_CERT_EXPIRED (TP_CONNECTION_STATUS_REASON_CERT_EXPIRED)
    TP_CONNECTION_STATUS_REASON_CERT_NOT_ACTIVATED = 9,
#   define TP_CONNECTION_STATUS_REASON_CERT_NOT_ACTIVATED (TP_CONNECTION_STATUS_REASON_CERT_NOT_ACTIVATED)
    TP_CONNECTION_STATUS_REASON_CERT_HOSTNAME_MISMATCH = 10,
#   define TP_CONNECTION_STATUS_REASON_CERT_HOSTNAME_MISMATCH (TP_CONNECTION_STATUS_REASON_CERT_HOSTNAME_MISMATCH)
    TP_CONNECTION_STATUS_REASON_CERT_FINGERPRINT_MISMATCH = 11,
#   define TP_CONNECTION_STATUS_REASON_CERT_FINGERPRINT_MISMATCH (TP_CONNECTION_STATUS_REASON_CERT_FINGERPRINT_MISMATCH)
    TP_CONNECTION_STATUS_REASON_CERT_SELF_SIGNED = 12,
#   define TP_CONNECTION_STATUS_REASON_CERT_SELF_SIGNED (TP_CONNECTION_STATUS_REASON_CERT_SELF_SIGNED)
    TP_CONNECTION_STATUS_REASON_CERT_OTHER_ERROR = 13,
#   define TP_CONNECTION_STATUS_REASON_CERT_OTHER_ERROR (TP_CONNECTION_STATUS_REASON_CERT_OTHER_ERROR)
#   define LAST_TP_CONNECTION_STATUS_REASON (13)
} TpConnectionStatusReason;


/* TpConnectionAliasFlags (bitfield/set of flags, 0 for none) */

typedef enum {
    TP_CONNECTION_ALIAS_FLAG_USER_SET = 1,
#   define TP_CONNECTION_ALIAS_FLAG_USER_SET (TP_CONNECTION_ALIAS_FLAG_USER_SET)
} TpConnectionAliasFlags;


/* TpConnectionCapabilityFlags (bitfield/set of flags, 0 for none) */

typedef enum {
    TP_CONNECTION_CAPABILITY_FLAG_CREATE = 1,
#   define TP_CONNECTION_CAPABILITY_FLAG_CREATE (TP_CONNECTION_CAPABILITY_FLAG_CREATE)
    TP_CONNECTION_CAPABILITY_FLAG_INVITE = 2,
#   define TP_CONNECTION_CAPABILITY_FLAG_INVITE (TP_CONNECTION_CAPABILITY_FLAG_INVITE)
} TpConnectionCapabilityFlags;


/* TpConnectionPresenceType (enum) */

typedef enum {
    TP_CONNECTION_PRESENCE_TYPE_UNSET = 0,
#   define TP_CONNECTION_PRESENCE_TYPE_UNSET (TP_CONNECTION_PRESENCE_TYPE_UNSET)
    TP_CONNECTION_PRESENCE_TYPE_OFFLINE = 1,
#   define TP_CONNECTION_PRESENCE_TYPE_OFFLINE (TP_CONNECTION_PRESENCE_TYPE_OFFLINE)
    TP_CONNECTION_PRESENCE_TYPE_AVAILABLE = 2,
#   define TP_CONNECTION_PRESENCE_TYPE_AVAILABLE (TP_CONNECTION_PRESENCE_TYPE_AVAILABLE)
    TP_CONNECTION_PRESENCE_TYPE_AWAY = 3,
#   define TP_CONNECTION_PRESENCE_TYPE_AWAY (TP_CONNECTION_PRESENCE_TYPE_AWAY)
    TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY = 4,
#   define TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY (TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY)
    TP_CONNECTION_PRESENCE_TYPE_HIDDEN = 5,
#   define TP_CONNECTION_PRESENCE_TYPE_HIDDEN (TP_CONNECTION_PRESENCE_TYPE_HIDDEN)
} TpConnectionPresenceType;


/* TpChannelContactSearchState (enum) */

typedef enum {
    TP_CHANNEL_CONTACT_SEARCH_STATE_BEFORE = 0,
#   define TP_CHANNEL_CONTACT_SEARCH_STATE_BEFORE (TP_CHANNEL_CONTACT_SEARCH_STATE_BEFORE)
    TP_CHANNEL_CONTACT_SEARCH_STATE_DURING = 1,
#   define TP_CHANNEL_CONTACT_SEARCH_STATE_DURING (TP_CHANNEL_CONTACT_SEARCH_STATE_DURING)
    TP_CHANNEL_CONTACT_SEARCH_STATE_AFTER = 2,
#   define TP_CHANNEL_CONTACT_SEARCH_STATE_AFTER (TP_CHANNEL_CONTACT_SEARCH_STATE_AFTER)
} TpChannelContactSearchState;


/* TpMediaStreamType (enum) */

typedef enum {
    TP_MEDIA_STREAM_TYPE_AUDIO = 0,
#   define TP_MEDIA_STREAM_TYPE_AUDIO (TP_MEDIA_STREAM_TYPE_AUDIO)
    TP_MEDIA_STREAM_TYPE_VIDEO = 1,
#   define TP_MEDIA_STREAM_TYPE_VIDEO (TP_MEDIA_STREAM_TYPE_VIDEO)
} TpMediaStreamType;


/* TpMediaStreamState (enum) */

typedef enum {
    TP_MEDIA_STREAM_STATE_DISCONNECTED = 0,
#   define TP_MEDIA_STREAM_STATE_DISCONNECTED (TP_MEDIA_STREAM_STATE_DISCONNECTED)
    TP_MEDIA_STREAM_STATE_CONNECTING = 1,
#   define TP_MEDIA_STREAM_STATE_CONNECTING (TP_MEDIA_STREAM_STATE_CONNECTING)
    TP_MEDIA_STREAM_STATE_CONNECTED = 2,
#   define TP_MEDIA_STREAM_STATE_CONNECTED (TP_MEDIA_STREAM_STATE_CONNECTED)
} TpMediaStreamState;


/* TpMediaStreamDirection (enum) */

typedef enum {
    TP_MEDIA_STREAM_DIRECTION_NONE = 0,
#   define TP_MEDIA_STREAM_DIRECTION_NONE (TP_MEDIA_STREAM_DIRECTION_NONE)
    TP_MEDIA_STREAM_DIRECTION_SEND = 1,
#   define TP_MEDIA_STREAM_DIRECTION_SEND (TP_MEDIA_STREAM_DIRECTION_SEND)
    TP_MEDIA_STREAM_DIRECTION_RECEIVE = 2,
#   define TP_MEDIA_STREAM_DIRECTION_RECEIVE (TP_MEDIA_STREAM_DIRECTION_RECEIVE)
    TP_MEDIA_STREAM_DIRECTION_BIDIRECTIONAL = 3,
#   define TP_MEDIA_STREAM_DIRECTION_BIDIRECTIONAL (TP_MEDIA_STREAM_DIRECTION_BIDIRECTIONAL)
} TpMediaStreamDirection;


/* TpMediaStreamPendingFlags (bitfield/set of flags, 0 for none) */

typedef enum {
    TP_MEDIA_STREAM_PENDING_LOCAL_SEND = 1,
#   define TP_MEDIA_STREAM_PENDING_LOCAL_SEND (TP_MEDIA_STREAM_PENDING_LOCAL_SEND)
    TP_MEDIA_STREAM_PENDING_REMOTE_SEND = 2,
#   define TP_MEDIA_STREAM_PENDING_REMOTE_SEND (TP_MEDIA_STREAM_PENDING_REMOTE_SEND)
} TpMediaStreamPendingFlags;


/* TpChannelMediaCapabilities (bitfield/set of flags, 0 for none) */
/* 
        The channel-type-specific capability flags used for
        Channel.Type.StreamedMedia in the Connection.Interface.Capabilities
        interface.
       */
typedef enum {
    TP_CHANNEL_MEDIA_CAPABILITY_AUDIO = 1,
#   define TP_CHANNEL_MEDIA_CAPABILITY_AUDIO (TP_CHANNEL_MEDIA_CAPABILITY_AUDIO)
    TP_CHANNEL_MEDIA_CAPABILITY_VIDEO = 2,
#   define TP_CHANNEL_MEDIA_CAPABILITY_VIDEO (TP_CHANNEL_MEDIA_CAPABILITY_VIDEO)
} TpChannelMediaCapabilities;


/* TpChannelTextSendError (enum) */

typedef enum {
    TP_CHANNEL_SEND_ERROR_UNKNOWN = 0,
#   define TP_CHANNEL_SEND_ERROR_UNKNOWN (TP_CHANNEL_SEND_ERROR_UNKNOWN)
    TP_CHANNEL_SEND_ERROR_OFFLINE = 1,
#   define TP_CHANNEL_SEND_ERROR_OFFLINE (TP_CHANNEL_SEND_ERROR_OFFLINE)
    TP_CHANNEL_SEND_ERROR_INVALID_CONTACT = 2,
#   define TP_CHANNEL_SEND_ERROR_INVALID_CONTACT (TP_CHANNEL_SEND_ERROR_INVALID_CONTACT)
    TP_CHANNEL_SEND_ERROR_PERMISSION_DENIED = 3,
#   define TP_CHANNEL_SEND_ERROR_PERMISSION_DENIED (TP_CHANNEL_SEND_ERROR_PERMISSION_DENIED)
    TP_CHANNEL_SEND_ERROR_TOO_LONG = 4,
#   define TP_CHANNEL_SEND_ERROR_TOO_LONG (TP_CHANNEL_SEND_ERROR_TOO_LONG)
    TP_CHANNEL_SEND_ERROR_NOT_IMPLEMENTED = 5,
#   define TP_CHANNEL_SEND_ERROR_NOT_IMPLEMENTED (TP_CHANNEL_SEND_ERROR_NOT_IMPLEMENTED)
} TpChannelTextSendError;


/* TpChannelTextMessageType (enum) */

typedef enum {
    TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL = 0,
#   define TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL (TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL)
    TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION = 1,
#   define TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION (TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION)
    TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE = 2,
#   define TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE (TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE)
    TP_CHANNEL_TEXT_MESSAGE_TYPE_AUTO_REPLY = 3,
#   define TP_CHANNEL_TEXT_MESSAGE_TYPE_AUTO_REPLY (TP_CHANNEL_TEXT_MESSAGE_TYPE_AUTO_REPLY)
} TpChannelTextMessageType;


/* TpChannelTextMessageFlags (bitfield/set of flags, 0 for none) */

typedef enum {
    TP_CHANNEL_TEXT_MESSAGE_FLAG_TRUNCATED = 1,
#   define TP_CHANNEL_TEXT_MESSAGE_FLAG_TRUNCATED (TP_CHANNEL_TEXT_MESSAGE_FLAG_TRUNCATED)
} TpChannelTextMessageFlags;


/* TpChannelGroupFlags (bitfield/set of flags, 0 for none) */

typedef enum {
    TP_CHANNEL_GROUP_FLAG_CAN_ADD = 1,
#   define TP_CHANNEL_GROUP_FLAG_CAN_ADD (TP_CHANNEL_GROUP_FLAG_CAN_ADD)
    TP_CHANNEL_GROUP_FLAG_CAN_REMOVE = 2,
#   define TP_CHANNEL_GROUP_FLAG_CAN_REMOVE (TP_CHANNEL_GROUP_FLAG_CAN_REMOVE)
    TP_CHANNEL_GROUP_FLAG_CAN_RESCIND = 4,
#   define TP_CHANNEL_GROUP_FLAG_CAN_RESCIND (TP_CHANNEL_GROUP_FLAG_CAN_RESCIND)
    TP_CHANNEL_GROUP_FLAG_MESSAGE_ADD = 8,
#   define TP_CHANNEL_GROUP_FLAG_MESSAGE_ADD (TP_CHANNEL_GROUP_FLAG_MESSAGE_ADD)
    TP_CHANNEL_GROUP_FLAG_MESSAGE_REMOVE = 16,
#   define TP_CHANNEL_GROUP_FLAG_MESSAGE_REMOVE (TP_CHANNEL_GROUP_FLAG_MESSAGE_REMOVE)
    TP_CHANNEL_GROUP_FLAG_MESSAGE_ACCEPT = 32,
#   define TP_CHANNEL_GROUP_FLAG_MESSAGE_ACCEPT (TP_CHANNEL_GROUP_FLAG_MESSAGE_ACCEPT)
    TP_CHANNEL_GROUP_FLAG_MESSAGE_REJECT = 64,
#   define TP_CHANNEL_GROUP_FLAG_MESSAGE_REJECT (TP_CHANNEL_GROUP_FLAG_MESSAGE_REJECT)
    TP_CHANNEL_GROUP_FLAG_MESSAGE_RESCIND = 128,
#   define TP_CHANNEL_GROUP_FLAG_MESSAGE_RESCIND (TP_CHANNEL_GROUP_FLAG_MESSAGE_RESCIND)
    TP_CHANNEL_GROUP_FLAG_CHANNEL_SPECIFIC_HANDLES = 256,
#   define TP_CHANNEL_GROUP_FLAG_CHANNEL_SPECIFIC_HANDLES (TP_CHANNEL_GROUP_FLAG_CHANNEL_SPECIFIC_HANDLES)
    TP_CHANNEL_GROUP_FLAG_ONLY_ONE_GROUP = 512,
#   define TP_CHANNEL_GROUP_FLAG_ONLY_ONE_GROUP (TP_CHANNEL_GROUP_FLAG_ONLY_ONE_GROUP)
} TpChannelGroupFlags;


/* TpChannelGroupChangeReason (enum) */

typedef enum {
    TP_CHANNEL_GROUP_CHANGE_REASON_NONE = 0,
#   define TP_CHANNEL_GROUP_CHANGE_REASON_NONE (TP_CHANNEL_GROUP_CHANGE_REASON_NONE)
    TP_CHANNEL_GROUP_CHANGE_REASON_OFFLINE = 1,
#   define TP_CHANNEL_GROUP_CHANGE_REASON_OFFLINE (TP_CHANNEL_GROUP_CHANGE_REASON_OFFLINE)
    TP_CHANNEL_GROUP_CHANGE_REASON_KICKED = 2,
#   define TP_CHANNEL_GROUP_CHANGE_REASON_KICKED (TP_CHANNEL_GROUP_CHANGE_REASON_KICKED)
    TP_CHANNEL_GROUP_CHANGE_REASON_BUSY = 3,
#   define TP_CHANNEL_GROUP_CHANGE_REASON_BUSY (TP_CHANNEL_GROUP_CHANGE_REASON_BUSY)
    TP_CHANNEL_GROUP_CHANGE_REASON_INVITED = 4,
#   define TP_CHANNEL_GROUP_CHANGE_REASON_INVITED (TP_CHANNEL_GROUP_CHANGE_REASON_INVITED)
    TP_CHANNEL_GROUP_CHANGE_REASON_BANNED = 5,
#   define TP_CHANNEL_GROUP_CHANGE_REASON_BANNED (TP_CHANNEL_GROUP_CHANGE_REASON_BANNED)
    TP_CHANNEL_GROUP_CHANGE_REASON_ERROR = 6,
#   define TP_CHANNEL_GROUP_CHANGE_REASON_ERROR (TP_CHANNEL_GROUP_CHANGE_REASON_ERROR)
} TpChannelGroupChangeReason;


/* TpChannelHoldState (enum) */

typedef enum {
    TP_CHANNEL_HOLD_STATE_NONE = 0,
#   define TP_CHANNEL_HOLD_STATE_NONE (TP_CHANNEL_HOLD_STATE_NONE)
    TP_CHANNEL_HOLD_STATE_SEND_ONLY = 1,
#   define TP_CHANNEL_HOLD_STATE_SEND_ONLY (TP_CHANNEL_HOLD_STATE_SEND_ONLY)
    TP_CHANNEL_HOLD_STATE_RECV_ONLY = 2,
#   define TP_CHANNEL_HOLD_STATE_RECV_ONLY (TP_CHANNEL_HOLD_STATE_RECV_ONLY)
    TP_CHANNEL_HOLD_STATE_BOTH = 3,
#   define TP_CHANNEL_HOLD_STATE_BOTH (TP_CHANNEL_HOLD_STATE_BOTH)
} TpChannelHoldState;


/* TpChannelPasswordFlags (bitfield/set of flags, 0 for none) */

typedef enum {
    TP_CHANNEL_PASSWORD_FLAG_PROVIDE = 8,
#   define TP_CHANNEL_PASSWORD_FLAG_PROVIDE (TP_CHANNEL_PASSWORD_FLAG_PROVIDE)
} TpChannelPasswordFlags;


/* TpMediaStreamError (enum) */

typedef enum {
    TP_MEDIA_STREAM_ERROR_UNKNOWN = 0,
#   define TP_MEDIA_STREAM_ERROR_UNKNOWN (TP_MEDIA_STREAM_ERROR_UNKNOWN)
    TP_MEDIA_STREAM_ERROR_EOS = 1,
#   define TP_MEDIA_STREAM_ERROR_EOS (TP_MEDIA_STREAM_ERROR_EOS)
} TpMediaStreamError;


/* TpMediaStreamBaseProto (enum) */

typedef enum {
    TP_MEDIA_STREAM_BASE_PROTO_UDP = 0,
#   define TP_MEDIA_STREAM_BASE_PROTO_UDP (TP_MEDIA_STREAM_BASE_PROTO_UDP)
    TP_MEDIA_STREAM_BASE_PROTO_TCP = 1,
#   define TP_MEDIA_STREAM_BASE_PROTO_TCP (TP_MEDIA_STREAM_BASE_PROTO_TCP)
} TpMediaStreamBaseProto;


/* TpMediaStreamTransportType (enum) */

typedef enum {
    TP_MEDIA_STREAM_TRANSPORT_TYPE_LOCAL = 0,
#   define TP_MEDIA_STREAM_TRANSPORT_TYPE_LOCAL (TP_MEDIA_STREAM_TRANSPORT_TYPE_LOCAL)
    TP_MEDIA_STREAM_TRANSPORT_TYPE_DERIVED = 1,
#   define TP_MEDIA_STREAM_TRANSPORT_TYPE_DERIVED (TP_MEDIA_STREAM_TRANSPORT_TYPE_DERIVED)
    TP_MEDIA_STREAM_TRANSPORT_TYPE_RELAY = 2,
#   define TP_MEDIA_STREAM_TRANSPORT_TYPE_RELAY (TP_MEDIA_STREAM_TRANSPORT_TYPE_RELAY)
} TpMediaStreamTransportType;


/* TpPropertyFlags (bitfield/set of flags, 0 for none) */

typedef enum {
    TP_PROPERTY_FLAG_READ = 1,
#   define TP_PROPERTY_FLAG_READ (TP_PROPERTY_FLAG_READ)
    TP_PROPERTY_FLAG_WRITE = 2,
#   define TP_PROPERTY_FLAG_WRITE (TP_PROPERTY_FLAG_WRITE)
} TpPropertyFlags;



#ifdef __cplusplus
}
#endif

