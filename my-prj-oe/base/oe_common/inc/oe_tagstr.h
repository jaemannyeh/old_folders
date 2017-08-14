
#ifndef OE_TAGSTR_H
#define OE_TAGSTR_H

/* 1 description ******************************************/

/* 2 configuration ****************************************/

/* 3 include files ****************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* 4 defines **********************************************/

#ifdef OE_TAGSTR_C
    #define OE_STR(t,s)    s
#else
    #define OE_STR(t,s)    t
#endif

#define OE_S0(t)    oe_str0_text[t]
#define OE_S1(t)    oe_str1_text[t]
#define OE_S2(t)    oe_str2_text[t]
#define OE_S3(t)    oe_str3_text[t]
#define OE_S4(t)    oe_str4_text[t]
#define OE_S5(t)    oe_str5_text[t]
#define OE_S6(t)    oe_str6_text[t]
#define OE_S7(t)    oe_str7_text[t]
#define OE_S8(t)    oe_str8_text[t]
#define OE_S9(t)    oe_str9_text[t]

/* 5 types ************************************************/

#ifdef OE_TAGSTR_C
const char *oe_str0_text[] = {
#else
extern const char *oe_str0_text[];
enum {
#endif
    OE_STR( S0_begin, "begin" ),

    OE_STR( S0_success, "success" ),
    OE_STR( S0_failed,  "failed" ),

    OE_STR( S0_no_memory,         "no memory" ),
    OE_STR( S0_not_found,         "not found" ),
    OE_STR( S0_not_supported,     "not supported" ),
    OE_STR( S0_permission_denied, "permission denied" ),

    OE_STR( S0_already_open,      "already open" ),
    OE_STR( S0_already_closed,    "already closed" ),
    OE_STR( S0_already_installed, "already installed" ),
    OE_STR( S0_already_started,   "already started" ),

    OE_STR( S0_not_opened,    "not opened" ),
    OE_STR( S0_not_closed,    "not closed" ),
    OE_STR( S0_not_installed, "not installed" ),
    OE_STR( S0_not_started,   "not started" ),

    OE_STR( S0_invalid_argument,      "invalid argument" ),
    OE_STR( S0_invalid_configuration, "invalid configuration" ),
    OE_STR( S0_invalid_version,       "invalid version" ),
    OE_STR( S0_invalid_alignment,     "invalid alignment" ),
    OE_STR( S0_invalid_interface,     "invalid interface" ),

    OE_STR( S0_no_route,         "no route" ),
    OE_STR( S0_dead_route,       "dead route" ),
    OE_STR( S0_invalid_route,    "invalid route" ),
    OE_STR( S0_invalid_mask,     "invalid mask" ),
    OE_STR( S0_invalid_rtclient, "invalid rtclient" ),

    OE_STR( S0_duplicate_entry,  "duplicate entry" ),
    OE_STR( S0_locked,           "locked" ),
    OE_STR( S0_end_of_operation, "end of operation" ),
    OE_STR( S0_readonly_entry,   "read only entry" ),

    OE_STR( S0_invalid_tag_value, "invalid tag value" ),
    OE_STR( S0_unsupported_tag,   "unsupported tag" ),

    OE_STR( S0_invalid_packet, "invalid packet" ),
    OE_STR( S0_bad_packet,     "bad packet" ),
    OE_STR( S0_ignored_packet, "ignored packet" ),

    OE_STR( S0_socket_call_failed, "socket call failed" ),

    OE_STR( S0_reached_minimum, "reached minimum" ),
    OE_STR( S0_reached_maximum, "reached maximum" ),

    OE_STR( S0_timeout,               "timeout" ),
    OE_STR( S0_would_block,           "would block" ),
    OE_STR( S0_operation_interrupted, "operation interrupted" ),
    OE_STR( S0_pending,               "pending" ),

    OE_STR( S0_auth_failed,       "auth failed" ),
    OE_STR( S0_unknown_auth_user, "unknown auth user" ),

    OE_STR( S0_any_key_to_continue, "any key to continue" ),

    OE_STR( S0_end, "end" )
#ifdef OE_TAGSTR_C
};
#else
};
#endif

/* 6 data *************************************************/

/* 7 functions ********************************************/

#ifdef __cplusplus
}
#endif

#endif /* OE_TAGSTR_H */

