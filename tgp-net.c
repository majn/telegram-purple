/* 
    This file is part of tgl-library

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    Copyright Vitaly Valtman, Matthias Jentsch 2013-2015
*/

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#ifndef WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif
#include <sys/fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "telegram-purple.h"
#include <mtproto-client.h>

#ifndef POLLRDHUP
#define POLLRDHUP 0
#endif

#define PING_TIMEOUT 15
#define CONNECT_TIMEOUT 5

static void fail_connection (struct connection *c);
static void restart_connection (struct connection *c);
static void start_ping_timer (struct connection *c);
static void conn_try_write (gpointer arg, gint source, PurpleInputCondition cond);
static void try_read (struct connection *c);
static void try_write (struct connection *c);

static int ping_alarm (gpointer arg) {
  struct connection *c = arg;
  debug ("ping alarm");
  assert (c->state == conn_failed || c->state == conn_ready || c->state == conn_connecting);
  if (tglt_get_double_time () - c->last_receive_time > 6 * PING_TIMEOUT) {
    warning ("fail connection: reason: ping timeout");
    c->state = conn_failed;
    fail_connection (c);
    return FALSE;
  } else if (tglt_get_double_time () - c->last_receive_time > 3 * PING_TIMEOUT && c->state == conn_ready) {
    tgl_do_send_ping (c->TLS, c);
    return TRUE;
  } else {
    return TRUE;
  }
}

static void stop_ping_timer (struct connection *c) {
  purple_timeout_remove (c->ping_ev);
  c->ping_ev = -1;
}

static void start_ping_timer (struct connection *c) {
  c->ping_ev = purple_timeout_add_seconds (PING_TIMEOUT, ping_alarm, c);
}

static int fail_alarm (gpointer arg) {
  struct connection *c = arg;
  c->in_fail_timer = 0;
  restart_connection (c);
  return FALSE;
}

static void start_fail_timer (struct connection *c) {
  if (c->in_fail_timer) { return; }
  c->in_fail_timer = 1;
  c->fail_ev = purple_timeout_add_seconds (CONNECT_TIMEOUT, fail_alarm, c);
}

static struct connection_buffer *new_connection_buffer (int size) {
  struct connection_buffer *b = malloc (sizeof (*b));
  memset (b, 0, sizeof (*b));
  b->start = malloc (size);
  b->end = b->start + size;
  b->rptr = b->wptr = b->start;
  return b;
}

static void delete_connection_buffer (struct connection_buffer *b) {
  free (b->start);
  free (b);
}

int tgln_write_out (struct connection *c, const void *_data, int len) {
  // debug ( "write_out: %d bytes\n", len);
  const unsigned char *data = _data;
  if (!len) { return 0; }
  assert (len > 0);
  int x = 0;
  if (!c->out_bytes) {
    assert (c->write_ev == -1);
    c->write_ev = purple_input_add (c->fd, PURPLE_INPUT_WRITE, conn_try_write, c);
  }
  if (!c->out_head) {
    struct connection_buffer *b = new_connection_buffer (1 << 20);
    c->out_head = c->out_tail = b;
  }
  while (len) {
    if (c->out_tail->end - c->out_tail->wptr >= len) {
      memcpy (c->out_tail->wptr, data, len);
      c->out_tail->wptr += len;
      c->out_bytes += len;
      return x + len;
    } else {
      int y = c->out_tail->end - c->out_tail->wptr;
      assert (y < len);
      memcpy (c->out_tail->wptr, data, y);
      x += y;
      len -= y;
      data += y;
      struct connection_buffer *b = new_connection_buffer (1 << 20);
      c->out_tail->next = b;
      b->next = 0;
      c->out_tail = b;
      c->out_bytes += y;
    }
  }
  return x;
}

int tgln_read_in (struct connection *c, void *_data, int len) {
  unsigned char *data = _data;
  if (!len) { return 0; }
  assert (len > 0);
  if (len > c->in_bytes) {
    len = c->in_bytes;
  }
  int x = 0;
  while (len) {
    int y = c->in_head->wptr - c->in_head->rptr;
    if (y > len) {
      memcpy (data, c->in_head->rptr, len);
      c->in_head->rptr += len;
      c->in_bytes -= len;
      return x + len;
    } else {
      memcpy (data, c->in_head->rptr, y);
      c->in_bytes -= y;
      x += y;
      data += y;
      len -= y;
      void *old = c->in_head;
      c->in_head = c->in_head->next;
      if (!c->in_head) {
        c->in_tail = 0;
      }
      delete_connection_buffer (old);
    }
  }
  return x;
}

int tgln_read_in_lookup (struct connection *c, void *_data, int len) {
  unsigned char *data = _data;
  if (!len || !c->in_bytes) { return 0; }
  assert (len > 0);
  if (len > c->in_bytes) {
    len = c->in_bytes;
  }
  int x = 0;
  struct connection_buffer *b = c->in_head;
  while (len) {
    int y = b->wptr - b->rptr;
    if (y >= len) {
      memcpy (data, b->rptr, len);
      return x + len;
    } else {
      memcpy (data, b->rptr, y);
      x += y;
      data += y;
      len -= y;
      b = b->next;
    }
  }
  return x;
}

void tgln_flush_out (struct connection *c) {
}

static void rotate_port (struct connection *c) {
  switch (c->port) {
  case 443:
    c->port = 80;
    break;
  case 80:
    c->port = 25;
    break;
  case 25:
    c->port = 443;
    break;
  }
}

static void conn_try_read (gpointer arg, gint source, PurpleInputCondition cond) {
  struct connection *c = arg;
  // debug ("Try read. Fd = %d\n", c->fd);
  try_read (c);
}

static void conn_try_write (gpointer arg, gint source, PurpleInputCondition cond) {
  struct connection *c = arg;
  struct tgl_state *TLS = c->TLS;
  if (c->state == conn_connecting) {
    c->state = conn_ready;
    c->methods->ready (TLS, c);
  }
  try_write (c);
  if (!c->out_bytes) {
    purple_input_remove (c->write_ev);
    c->write_ev = -1;
  }
}

static void net_on_connected (gpointer arg, gint fd, const gchar *error_message) {
  struct connection *c = arg;
  // debug ("connect result: %d\n", fd);

  if (c->fail_ev >= 0) {
    purple_timeout_remove (c->fail_ev);
    c->fail_ev = -1;
  }
  
  if (fd == -1) {
    const char *msg = "Connection not possible, either your network or a Telegram data center is down, or the"
    " Telegram network configuration has changed.";
    warning (msg);
    return;
  }

  c->fd = fd;
  c->read_ev = purple_input_add (fd, PURPLE_INPUT_READ, conn_try_read, c);
  
  unsigned char byte = 0xef;
  assert (tgln_write_out (c, &byte, 1) == 1);
  
  c->last_receive_time = tglt_get_double_time ();
  start_ping_timer (c);
}

static void net_on_connected_assert_success (gpointer arg, gint fd, const gchar *error_message) { 
  struct connection *c = arg;
  struct tgl_state *TLS = c->TLS;
  if (fd == -1) {
    info ("Connection to main data center (%d) %s:%d not possible\n", c->dc->id, c->ip, c->port);
    purple_connection_error_reason (tls_get_conn (TLS), PURPLE_CONNECTION_ERROR_NETWORK_ERROR, _("Cannot connect to main server"));
    return;
  }
  net_on_connected (arg, fd, error_message);
}

struct connection *tgln_create_connection (struct tgl_state *TLS, const char *host, int port, struct tgl_session *session, struct tgl_dc *dc, struct mtproto_methods *methods) {
  struct connection *c = malloc (sizeof (*c));
  memset (c, 0, sizeof (*c));
  c->TLS = TLS;

  
  c->fd = -1;
  c->state = conn_connecting;

  c->last_receive_time = tglt_get_double_time ();
  c->ip = strdup (host);
  c->flags = 0;
  c->port = port;

  c->ping_ev = -1;
  c->fail_ev = -1;
  c->write_ev = -1;
  c->read_ev = -1;

  c->dc = dc;
  c->session = session;
  c->methods = methods;

  c->prpl_data = purple_proxy_connect (tls_get_conn (TLS), tls_get_pa (TLS), host, port,
                    TLS->dc_working_num == dc->id ? net_on_connected_assert_success : net_on_connected, c);

  start_fail_timer (c);
  
  return c;
}

static void restart_connection (struct connection *c) {
  debug("restart_connection()");
  if (tglt_get_double_time () - c->last_receive_time > 6 * PING_TIMEOUT) {
    purple_connection_error_reason (tls_get_conn (c->TLS), PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
        _("Cannot connect to server: Ping timeout."));
    return;
  }
  purple_proxy_connect_cancel (c->prpl_data);
  c->prpl_data = purple_proxy_connect (tls_get_conn (c->TLS), tls_get_pa (c->TLS), c->ip, c->port, net_on_connected, c);
}

static void fail_connection (struct connection *c) {
  if (c->state == conn_ready) {
    stop_ping_timer (c);
  }
  if (c->write_ev >= 0) {
    purple_input_remove (c->write_ev);
    c->write_ev = -1;
  }
  if (c->read_ev >= 0) {
    purple_input_remove (c->read_ev);
    c->read_ev = -1;
  }
  
  rotate_port (c);

  struct connection_buffer *b = c->out_head;
  while (b) {
    struct connection_buffer *d = b;
    b = b->next;
    delete_connection_buffer (d);
  }
  b = c->in_head;
  while (b) {
    struct connection_buffer *d = b;
    b = b->next;
    delete_connection_buffer (d);
  }
  c->out_head = c->out_tail = c->in_head = c->in_tail = 0;
  c->state = conn_failed;
  c->out_bytes = c->in_bytes = 0;
  
  c->prpl_data = NULL;

  info ("Lost connection to server ... %s:%d\n", c->ip, c->port);
  purple_connection_error_reason (tls_get_conn (c->TLS), PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
      _("Lost connection to the server..."));
}

//extern FILE *log_net_f;
static void try_write (struct connection *c) {
  // debug ("try write: fd = %d\n", c->fd);
  int x = 0;
  while (c->out_head) {
    int r = send (c->fd, (const char *)c->out_head->rptr, c->out_head->wptr - c->out_head->rptr, 0);
    if (r >= 0) {
      x += r;
      c->out_head->rptr += r;
      if (c->out_head->rptr != c->out_head->wptr) {
        break;
      }
      struct connection_buffer *b = c->out_head;
      c->out_head = b->next;
      if (!c->out_head) {
        c->out_tail = 0;
      }
      delete_connection_buffer (b);
    } else {
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        info ("fail_connection: write_error %s\n", g_strerror(errno));
        fail_connection (c);
        return;
      } else {
        break;
      }
    }
  }
  // debug ("Sent %d bytes to %d\n", x, c->fd);
  c->out_bytes -= x;
}

static void try_rpc_read (struct connection *c) {
  assert (c->in_head);
  struct tgl_state *TLS = c->TLS;

  while (1) {
    if (c->in_bytes < 1) { return; }
    unsigned len = 0;
    unsigned t = 0;
    assert (tgln_read_in_lookup (c, &len, 1) == 1);
    if (len >= 1 && len <= 0x7e) {
      if (c->in_bytes < (int)(1 + 4 * len)) { return; }
    } else {
      if (c->in_bytes < 4) { return; }
      assert (tgln_read_in_lookup (c, &len, 4) == 4);
      len = (len >> 8);
      if (c->in_bytes < (int)(4 + 4 * len)) { return; }
      len = 0x7f;
    }

    if (len >= 1 && len <= 0x7e) {
      assert (tgln_read_in (c, &t, 1) == 1);    
      assert (t == len);
      assert (len >= 1);
    } else {
      assert (len == 0x7f);
      assert (tgln_read_in (c, &len, 4) == 4);
      len = (len >> 8);
      assert (len >= 1);
    }
    len *= 4;
    int op;
    assert (tgln_read_in_lookup (c, &op, 4) == 4);
    if (c->methods->execute (TLS, c, op, len) < 0) { 
      return;
    }
  }
}

static void try_read (struct connection *c) {
  // debug ( "try read: fd = %d\n", c->fd);
  if (!c->in_tail) {
    c->in_head = c->in_tail = new_connection_buffer (1 << 20);
  }
  #ifdef EVENT_V1
    struct timeval tv = {5, 0};
    event_add (c->read_ev, &tv);
  #endif
  int x = 0;
  while (1) {
    int r = recv (c->fd, (char *)c->in_tail->wptr, c->in_tail->end - c->in_tail->wptr, 0);
    if (r > 0) {
      c->last_receive_time = tglt_get_double_time ();
      stop_ping_timer (c);
      start_ping_timer (c);
    }
    if (r >= 0) {
      c->in_tail->wptr += r;
      x += r;
      if (c->in_tail->wptr != c->in_tail->end) {
        break;
      }
      struct connection_buffer *b = new_connection_buffer (1 << 20);
      c->in_tail->next = b;
      c->in_tail = b;
    } else {
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        debug ("fail_connection: read_error %s\n", strerror(errno));
        fail_connection (c);
        return;
      } else {
        break;
      }
    }
  }
  // debug ("Received %d bytes from %d\n", x, c->fd);
  c->in_bytes += x;
  if (x) {
    try_rpc_read (c);
  }
}

static void incr_out_packet_num (struct connection *c) {
  c->out_packet_num ++;
}

static struct tgl_dc *get_dc (struct connection *c) {
  return c->dc;
}

static struct tgl_session *get_session (struct connection *c) {
  return c->session;
}

static void tgln_free (struct connection *c) {
  if (c->ip) { free (c->ip); }
  struct connection_buffer *b = c->out_head;
  while (b) {
    struct connection_buffer *d = b;
    b = b->next;
    delete_connection_buffer (d);
  }
  b = c->in_head;
  while (b) {
    struct connection_buffer *d = b;
    b = b->next;
    delete_connection_buffer (d);
  }

  if (c->ping_ev >= 0) { 
    purple_timeout_remove (c->ping_ev);
    c->ping_ev = -1;
  }
  if (c->fail_ev >= 0) { 
    purple_timeout_remove (c->fail_ev);
    c->fail_ev = -1;
  }
    
  if (c->read_ev >= 0) { 
    purple_input_remove (c->read_ev);
  }
  if (c->write_ev >= 0) { 
    purple_input_remove (c->write_ev);
  }

  if (c->fd >= 0) { close (c->fd); }
  c->fd = -1;
}

struct tgl_net_methods tgp_conn_methods = {
  .write_out = tgln_write_out,
  .read_in = tgln_read_in,
  .read_in_lookup = tgln_read_in_lookup,
  .flush_out = tgln_flush_out,
  .incr_out_packet_num = incr_out_packet_num,
  .get_dc = get_dc,
  .get_session = get_session,
  .create_connection = tgln_create_connection,
  .free = tgln_free
};
