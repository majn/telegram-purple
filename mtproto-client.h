/*
    This file is part of telegram-client.

    Telegram-client is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Telegram-client is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this telegram-client.  If not, see <http://www.gnu.org/licenses/>.

Copyright Nikolay Durov, Andrey Lopatin 2012-2013
    Copyright Vitaly Valtman 2013
*/
#ifndef __MTPROTO_CLIENT_H__
#define __MTPROTO_CLIENT_H__


/* 
 * COMMON
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>

#include "include.h"
#include "tools.h"
#include "constants.h"
#include "msglog.h"
#include "net.h"

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif


/* DH key exchange protocol data structures */
#define	CODE_req_pq			0x60469778
#define CODE_resPQ			0x05162463
#define CODE_req_DH_params		0xd712e4be
#define CODE_p_q_inner_data		0x83c95aec
#define CODE_server_DH_inner_data	0xb5890dba
#define CODE_server_DH_params_fail	0x79cb045d
#define CODE_server_DH_params_ok	0xd0e8075c
#define CODE_set_client_DH_params	0xf5045f1f
#define CODE_client_DH_inner_data	0x6643b654
#define CODE_dh_gen_ok			0x3bcbf734
#define CODE_dh_gen_retry		0x46dc1fb9
#define CODE_dh_gen_fail		0xa69dae02 

/* service messages */
#define CODE_rpc_result			0xf35c6d01
#define CODE_rpc_error			0x2144ca19
#define CODE_msg_container		0x73f1f8dc
#define CODE_msg_copy			0xe06046b2
#define CODE_msgs_ack			0x62d6b459
#define CODE_bad_msg_notification	0xa7eff811
#define	CODE_bad_server_salt		0xedab447b
#define CODE_msgs_state_req		0xda69fb52
#define CODE_msgs_state_info		0x04deb57d
#define CODE_msgs_all_info		0x8cc0d131
#define CODE_new_session_created	0x9ec20908
#define CODE_msg_resend_req		0x7d861a08
#define CODE_ping			0x7abe77ec
#define CODE_pong			0x347773c5
#define CODE_destroy_session		0xe7512126
#define CODE_destroy_session_ok		0xe22045fc
#define CODE_destroy_session_none      	0x62d350c9
#define CODE_destroy_sessions		0x9a6face8
#define CODE_destroy_sessions_res	0xa8164668
#define	CODE_get_future_salts		0xb921bd04
#define	CODE_future_salt		0x0949d9dc
#define	CODE_future_salts		0xae500895
#define	CODE_rpc_drop_answer		0x58e4a740
#define CODE_rpc_answer_unknown		0x5e2ad36e
#define CODE_rpc_answer_dropped_running	0xcd78e586
#define CODE_rpc_answer_dropped		0xa43ad8b7
#define	CODE_msg_detailed_info		0x276d3ec6
#define	CODE_msg_new_detailed_info	0x809db6df
#define CODE_ping_delay_disconnect	0xf3427b8c
#define CODE_gzip_packed 0x3072cfa1

#define CODE_input_peer_notify_settings_old 0x3cf4b1be
#define CODE_peer_notify_settings_old 0xddbcd4a5
#define CODE_user_profile_photo_old 0x990d1493
#define CODE_config_old 0x232d5905

#define CODE_msg_new_detailed_info 0x809db6df

#define CODE_msg_detailed_info 0x276d3ec6
/* not really a limit, for struct encrypted_message only */
// #define MAX_MESSAGE_INTS	16384
#define MAX_MESSAGE_INTS	1048576
#define MAX_PROTO_MESSAGE_INTS	1048576
#define	_FILE_OFFSET_BITS	64


#pragma pack(push,4)
struct encrypted_message {
  // unencrypted header
  long long auth_key_id;
  char msg_key[16];
  // encrypted part, starts with encrypted header
  long long server_salt;
  long long session_id;
  // long long auth_key_id2; // removed
  // first message follows
  long long msg_id;
  int seq_no;
  int msg_len;   // divisible by 4
  int message[MAX_MESSAGE_INTS];
};
#pragma pack(pop)

/* 
 * CONNECTION STATES
 */

enum dc_state {
  st_init,
  st_reqpq_sent,
  st_reqdh_sent,
  st_client_dh_sent,
  st_authorized,
  st_error
};

/*
 * CLIENT
 */

// forward-declarations
struct timespec;
struct telegram;

#define DECRYPT_BUFFER_INTS 16384
#define ENCRYPT_BUFFER_INTS 16384
#define PACKET_BUFFER_SIZE	(16384 * 100 + 16) // temp fix

struct mtproto_connection {
    struct connection *connection;

    int auth_success;
    enum dc_state c_state;
    char nonce[256];
    char new_nonce[256];
    char server_nonce[256];
    
    int total_packets_sent;
    long long total_data_sent;

    unsigned long long what;
    unsigned p1, p2;

    int *in_ptr, *in_end;

    // common

    int __packet_buffer[PACKET_BUFFER_SIZE], *packet_ptr;
    int *packet_buffer;
    
    long long rsa_encrypted_chunks, rsa_decrypted_chunks;
    
    BN_CTX *BN_ctx;
   

    // DH

    int encrypt_buffer[ENCRYPT_BUFFER_INTS];
    int decrypt_buffer[ENCRYPT_BUFFER_INTS];
    char s_power [256];
    BIGNUM dh_prime, dh_g, g_a, dh_power, auth_key_num;

    struct {
        long long auth_key_id;
        long long out_msg_id;
        int msg_len;
    } unenc_msg_header;


    // AES IGE

    unsigned char aes_key_raw[32], aes_iv[32];
    AES_KEY aes_key;

    // authorized

    struct encrypted_message enc_msg;
    long long client_last_msg_id;
    long long server_last_msg_id;

    int longpoll_count, good_messages;

    int unread_messages;
    int our_id;
    int pts;
    int qts;
    int last_date;
    int seq;

    // extra (queries.c)

    int *encr_extra;
    int *encr_ptr;
    int *encr_end;

    // callbacks

    void (*on_ready)(struct mtproto_connection *self, void* data);
    void *on_ready_data;

    void (*on_error)(struct mtproto_connection *self, void *data);
    
    // the amount of currently outgoing messages that
    // have not yet received a response
    int queries_num;

    // binlog that consumes all updates and events of this connection
    struct binlog *bl;

    // marks this connection for destruction, so it
    // will be freed once all queries received a response or timed out
    int destroy;
};

void mtproto_connection_init (struct mtproto_connection *c);
struct mtproto_connection *mtproto_new(struct dc *DC, int fd, struct telegram *tg);
void mtproto_close(struct mtproto_connection *c);
void mtproto_connect(struct mtproto_connection *c);

void on_start (struct mtproto_connection *self);
long long encrypt_send_message (struct mtproto_connection *self, int *msg, int msg_ints, int useful);
void work_update (struct mtproto_connection *self, long long msg_id);
void work_update_binlog (struct mtproto_connection *self);
int check_g (unsigned char p[256], BIGNUM *g);
int check_g_bn (BIGNUM *p, BIGNUM *g);
int check_DH_params (struct mtproto_connection *self, BIGNUM *p, int g);
void secure_random (void *s, int l);

/*
 * Common 2
 */

void prng_seed (struct mtproto_connection *self, const char *password_filename, int password_length);
int serialize_bignum (BIGNUM *b, char *buffer, int maxlen);
long long compute_rsa_key_fingerprint (RSA *key);

static inline void out_ints (struct mtproto_connection *self, const int *what, int len) {
  assert (self->packet_ptr + len <= self->packet_buffer + PACKET_BUFFER_SIZE);
  memcpy (self->packet_ptr, what, len * 4);
  self->packet_ptr += len;
}


static inline void out_int (struct mtproto_connection *self, int x) {
  logprintf("out_int(): packet_ptr:%p, packet_buffer:%p\n", self->packet_ptr, self->packet_buffer);
  assert (self->packet_ptr + 1 <= self->packet_buffer + PACKET_BUFFER_SIZE);
  *self->packet_ptr++ = x;
}


static inline void out_long (struct mtproto_connection *self, long long x) {
  assert (self->packet_ptr + 2 <= self->packet_buffer + PACKET_BUFFER_SIZE);
  *(long long *)self->packet_ptr = x;
  self->packet_ptr += 2;
}

static inline void clear_packet (struct mtproto_connection *self) {
  self->packet_ptr = self->packet_buffer;
}

void out_cstring (struct mtproto_connection *self, const char *str, long len);
void out_cstring_careful (struct mtproto_connection *self, const char *str, long len);
void out_data (struct mtproto_connection *self, const void *data, long len);

static inline void out_string (struct mtproto_connection *self, const char *str) {
  out_cstring (self, str, strlen (str));
}

static inline void out_bignum (struct mtproto_connection *self, BIGNUM *n) {
  int l = serialize_bignum (n, (char *)self->packet_ptr, (PACKET_BUFFER_SIZE - (self->packet_ptr - self->packet_buffer)) * 4);
  assert (l > 0);
  self->packet_ptr += l >> 2;
}


void fetch_pts (struct mtproto_connection *self);
void fetch_qts (struct mtproto_connection *self);
void fetch_date (struct mtproto_connection *self);
void fetch_seq (struct mtproto_connection *self);
static inline int prefetch_strlen (struct mtproto_connection *self) {
  if (self->in_ptr >= self->in_end) { 
    return -1; 
  }
  unsigned l = *self->in_ptr;
  if ((l & 0xff) < 0xfe) { 
    l &= 0xff;
    return (self->in_end >= self->in_ptr + (l >> 2) + 1) ? (int)l : -1;
  } else if ((l & 0xff) == 0xfe) {
    l >>= 8;
    return (l >= 254 && self->in_end >= self->in_ptr + ((l + 7) >> 2)) ? (int)l : -1;
  } else {
    return -1;
  }
}

extern int verbosity;
static inline char *fetch_str (struct mtproto_connection *self, int len) {
  assert (len >= 0);
  if (verbosity > 6) {
    logprintf ("fetch_string: len = %d\n", len);
  }
  if (len < 254) {
    char *str = (char *) self->in_ptr + 1;
    self->in_ptr += 1 + (len >> 2);
    return str;
  } else {
    char *str = (char *) self->in_ptr + 4;
    self->in_ptr += (len + 7) >> 2;
    return str;
  }
} 

static inline char *fetch_str_dup (struct mtproto_connection *self) {
  int l = prefetch_strlen (self);
  assert (l >= 0);
  int i;
  char *s = fetch_str (self, l);
  for (i = 0; i < l; i++) {
    if (!s[i]) { break; }
  }
  char *r = talloc (i + 1);
  memcpy (r, s, i);
  r[i] = 0;
  return r;
}

static inline int fetch_update_str (struct mtproto_connection *self, char **s) {
  if (!*s) {
    *s = fetch_str_dup (self);
    return 1;
  }
  int l = prefetch_strlen (self);
  char *r = fetch_str (self, l);
  if (memcmp (*s, r, l) || (*s)[l]) {
    tfree_str (*s);
    *s = talloc (l + 1);
    memcpy (*s, r, l);
    (*s)[l] = 0;
    return 1;
  }
  return 0;
}

static inline int fetch_update_int (struct mtproto_connection *self, int *value) {
  if (*value == *self->in_ptr) {
    self->in_ptr ++;
    return 0;
  } else {
    *value = *(self->in_ptr ++);
    return 1;
  }
}

static inline int fetch_update_long (struct mtproto_connection *self, long long *value) {
  if (*value == *(long long *)self->in_ptr) {
    self->in_ptr += 2;
    return 0;
  } else {
    *value = *(long long *)(self->in_ptr);
    self->in_ptr += 2;
    return 1;
  }
}

static inline int set_update_int (int *value, int new_value) {
  if (*value == new_value) {
    return 0;
  } else {
    *value = new_value;
    return 1;
  }
}

static inline void fetch_skip (struct mtproto_connection *self, int n) {
  self->in_ptr += n;
  assert (self->in_ptr <= self->in_end);
}

static inline void fetch_skip_str (struct mtproto_connection *self) {
  int l = prefetch_strlen (self);
  assert (l >= 0);
  fetch_str (self, l);
}

static inline long have_prefetch_ints (struct mtproto_connection *self) {
  return self->in_end - self->in_ptr;
}

int fetch_bignum (struct mtproto_connection *self, BIGNUM *x);

static inline int fetch_int (struct mtproto_connection *self) {
  assert (self->in_ptr + 1 <= self->in_end);
  if (verbosity > 6) {
    logprintf ("fetch_int: 0x%08x (%d)\n", *self->in_ptr, *self->in_ptr);
  }
  return *(self->in_ptr ++);
}

static inline int fetch_bool (struct mtproto_connection *self) {
  if (verbosity > 6) {
    logprintf ("fetch_bool: 0x%08x (%d)\n", *self->in_ptr, *self->in_ptr);
  }
  assert (self->in_ptr + 1 <= self->in_end);
  assert (*(self->in_ptr) == (int)CODE_bool_true || *(self->in_ptr) == (int)CODE_bool_false);
  return *(self->in_ptr ++) == (int)CODE_bool_true;
}

static inline int prefetch_int (struct mtproto_connection *self) {
  assert (self->in_ptr < self->in_end);
  return *(self->in_ptr);
}

static inline void prefetch_data (struct mtproto_connection *self, void *data, int size) {
  assert (self->in_ptr + (size >> 2) <= self->in_end);
  memcpy (data, self->in_ptr, size);
}

static inline void fetch_data (struct mtproto_connection *self, void *data, int size) {
  assert (self->in_ptr + (size >> 2) <= self->in_end);
  memcpy (data, self->in_ptr, size);
  assert (!(size & 3));
  self->in_ptr += (size >> 2);
}

static inline long long fetch_long (struct mtproto_connection *self) {
  assert (self->in_ptr + 2 <= self->in_end);
  long long r = *(long long *)self->in_ptr;
  self->in_ptr += 2;
  return r;
}

static inline double fetch_double (struct mtproto_connection *self) {
  assert (self->in_ptr + 2 <= self->in_end);
  double r = *(double *)self->in_ptr;
  self->in_ptr += 2;
  return r;
}

static inline void fetch_ints (struct mtproto_connection *self, void *data, int count) {
  assert (self->in_ptr + count <= self->in_end);
  memcpy (data, self->in_ptr, 4 * count);
  self->in_ptr += count;
}

int get_random_bytes (unsigned char *buf, int n);

int pad_rsa_encrypt (struct mtproto_connection *self, char *from, int from_len, char *to, int size, BIGNUM *N, BIGNUM *E);
int pad_rsa_decrypt (struct mtproto_connection *self, char *from, int from_len, char *to, int size, BIGNUM *N, BIGNUM *D);

void init_aes_unauth (struct mtproto_connection *self, const char server_nonce[16], const char hidden_client_nonce[32], int encrypt);
void init_aes_auth (struct mtproto_connection *self, char auth_key[192], char msg_key[16], int encrypt);
int pad_aes_encrypt (struct mtproto_connection *self, char *from, int from_len, char *to, int size);
int pad_aes_decrypt (struct mtproto_connection *self, char *from, int from_len, char *to, int size);

static inline void hexdump_in (struct mtproto_connection *self) {
  hexdump (self->in_ptr, self->in_end);
}

static inline void hexdump_out (struct mtproto_connection *self) {
  hexdump (self->packet_buffer, self->packet_ptr);
}

#ifdef __MACH__
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#endif
void my_clock_gettime (int clock_id, struct timespec *T);

#endif
