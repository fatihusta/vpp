/*
 * Copyright (c) 2017-2019 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SRC_VNET_SESSION_SESSION_LOOKUP_H_
#define SRC_VNET_SESSION_SESSION_LOOKUP_H_

#include <vnet/session/session_table.h>
#include <vnet/session/session_types.h>
#include <vnet/session/application_namespace.h>
#include <vnet/fib/fib_table.h>
#include <vnet/fib/fib_source.h>

#define HALF_OPEN_LOOKUP_INVALID_VALUE ((u64)~0)

typedef enum session_lookup_result_
{
  SESSION_LOOKUP_RESULT_NONE,
  SESSION_LOOKUP_RESULT_WRONG_THREAD,
  SESSION_LOOKUP_RESULT_FILTERED
} session_lookup_result_t;

typedef struct session_lookup_main_
{
  clib_spinlock_t st_alloc_lock;
  fib_source_t fib_src;
} session_lookup_main_t;

session_t *session_lookup_safe4 (u32 fib_index, ip4_address_t * lcl,
				 ip4_address_t * rmt, u16 lcl_port,
				 u16 rmt_port, u8 proto);
session_t *session_lookup_safe6 (u32 fib_index, ip6_address_t * lcl,
				 ip6_address_t * rmt, u16 lcl_port,
				 u16 rmt_port, u8 proto);
transport_connection_t *session_lookup_connection_wt4 (
  u32 fib_index, ip4_address_t *lcl, ip4_address_t *rmt, u16 lcl_port,
  u16 rmt_port, u8 proto, clib_thread_index_t thread_index, u8 *is_filtered);
transport_connection_t *session_lookup_connection4 (u32 fib_index,
						    ip4_address_t * lcl,
						    ip4_address_t * rmt,
						    u16 lcl_port,
						    u16 rmt_port, u8 proto);
transport_connection_t *session_lookup_connection_wt6 (
  u32 fib_index, ip6_address_t *lcl, ip6_address_t *rmt, u16 lcl_port,
  u16 rmt_port, u8 proto, clib_thread_index_t thread_index, u8 *is_filtered);
transport_connection_t *session_lookup_connection6 (u32 fib_index,
						    ip6_address_t * lcl,
						    ip6_address_t * rmt,
						    u16 lcl_port,
						    u16 rmt_port, u8 proto);
transport_connection_t *session_lookup_connection (u32 fib_index,
						   ip46_address_t * lcl,
						   ip46_address_t * rmt,
						   u16 lcl_port, u16 rmt_port,
						   u8 proto, u8 is_ip4);
transport_connection_t *
session_lookup_6tuple (u32 fib_index, ip46_address_t *lcl, ip46_address_t *rmt,
		       u16 lcl_port, u16 rmt_port, u8 proto, u8 is_ip4);
session_t *session_lookup_listener4 (u32 fib_index, ip4_address_t * lcl,
				     u16 lcl_port, u8 proto, u8 use_wildcard);
session_t *session_lookup_listener6 (u32 fib_index, ip6_address_t * lcl,
				     u16 lcl_port, u8 proto, u8 use_wildcard);
session_t *session_lookup_listener (u32 table_index,
				    session_endpoint_t * sep);
session_t *session_lookup_listener_wildcard (u32 table_index,
					     session_endpoint_t * sep);
int session_lookup_add_connection (transport_connection_t * tc, u64 value);
int session_lookup_del_connection (transport_connection_t * tc);
u64 session_lookup_endpoint_listener (u32 table_index,
				      session_endpoint_t * sepi,
				      u8 use_rules);
u64 session_lookup_local_endpoint (u32 table_index, session_endpoint_t * sep);
session_t *session_lookup_global_session_endpoint (session_endpoint_t *);
int session_lookup_add_session_endpoint (u32 table_index,
					 session_endpoint_t * sep, u64 value);
int session_lookup_del_session_endpoint (u32 table_index,
					 session_endpoint_t * sep);
int session_lookup_del_session_endpoint2 (session_endpoint_t * sep);
int session_lookup_del_session (session_t * s);
int session_lookup_del_half_open (transport_connection_t * tc);
int session_lookup_add_half_open (transport_connection_t * tc, u64 value);
u64 session_lookup_half_open_handle (transport_connection_t * tc);
transport_connection_t *session_lookup_half_open_connection (u64 handle,
							     u8 proto,
							     u8 is_ip4);
u32 session_lookup_get_index_for_fib (u32 fib_proto, u32 fib_index);
u32 session_lookup_get_or_alloc_index_for_fib (u32 fib_proto, u32 fib_index);

void session_lookup_show_table_entries (vlib_main_t * vm,
					session_table_t * table, u8 type,
					u8 is_local);

void session_lookup_dump_rules_table (u32 fib_index, u8 fib_proto,
				      u8 transport_proto);
void session_lookup_dump_local_rules_table (u32 fib_index, u8 fib_proto,
					    u8 transport_proto);

typedef enum _session_rule_scope
{
  SESSION_RULE_SCOPE_GLOBAL = 1,
  SESSION_RULE_SCOPE_LOCAL = 2,
} session_rule_scope_e;

typedef struct _session_rules_table_add_del_args
{
  fib_prefix_t lcl;
  fib_prefix_t rmt;
  u16 lcl_port;
  u16 rmt_port;
  u32 action_index;
  u8 *tag;
  u8 is_add;
} session_rule_table_add_del_args_t;

typedef struct _session_rule_add_del_args
{
  /**
   * Actual arguments to adding the rule to a session rules table
   */
  session_rule_table_add_del_args_t table_args;
  /**
   * Application namespace where rule should be applied. If 0,
   * default namespace is used.
   */
  u32 appns_index;
  /**
   * Rule scope flag.
   */
  u8 scope;
  /**
   * Transport protocol for the rule
   */
  u8 transport_proto;
} session_rule_add_del_args_t;

session_error_t vnet_session_rule_add_del (session_rule_add_del_args_t *args);
void session_lookup_set_tables_appns (app_namespace_t * app_ns);

void session_lookup_init (void);
session_table_t *session_table_get_for_fib_index (u32 fib_proto,
						  u32 fib_index);

#endif /* SRC_VNET_SESSION_SESSION_LOOKUP_H_ */

/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */
