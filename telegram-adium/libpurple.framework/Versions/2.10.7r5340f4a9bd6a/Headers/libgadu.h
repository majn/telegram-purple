/* $Id: libgadu.h.in 1105 2011-05-25 21:34:50Z wojtekka $ */

/*
 *  (C) Copyright 2001-2009 Wojtek Kaniewski <wojtekka@irc.pl>
 *                          Robert J. Woźny <speedy@ziew.org>
 *                          Arkadiusz Miśkiewicz <arekm@pld-linux.org>
 *                          Tomasz Chiliński <chilek@chilan.com>
 *                          Piotr Wysocki <wysek@linux.bydg.org>
 *                          Dawid Jarosz <dawjar@poczta.onet.pl>
 *                          Jakub Zawadzki <darkjames@darkjames.ath.cx>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License Version
 *  2.1 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

/**
 * \file libgadu.h
 *
 * \brief Główny plik nagłówkowy biblioteki
 */

#ifndef __GG_LIBGADU_H
#define __GG_LIBGADU_H

#ifdef _WIN32
#pragma pack(push, 1)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>

/** \cond ignore */

/* Defined if libgadu was compiled for bigendian machine. */
#undef GG_CONFIG_BIGENDIAN

/* Defined if this machine has gethostbyname_r(). */
#undef GG_CONFIG_HAVE_GETHOSTBYNAME_R

/* Defined if libgadu was compiled and linked with pthread support. */
#undef GG_CONFIG_HAVE_PTHREAD

/* Defined if pthread resolver is the default one. */
#undef GG_CONFIG_PTHREAD_DEFAULT 

/* Defined if this machine has C99-compiliant vsnprintf(). */
#undef GG_CONFIG_HAVE_C99_VSNPRINTF

/* Defined if this machine has va_copy(). */
#undef GG_CONFIG_HAVE_VA_COPY

/* Defined if this machine has __va_copy(). */
#undef GG_CONFIG_HAVE___VA_COPY

/* Defined if this machine supports long long. */
#undef GG_CONFIG_HAVE_LONG_LONG

/* Defined if libgadu was compiled and linked with GnuTLS support. */
#undef GG_CONFIG_HAVE_GNUTLS

/* Defined if libgadu was compiled and linked with OpenSSL support. */
#undef GG_CONFIG_HAVE_OPENSSL

/* Defined if libgadu was compiled and linked with zlib support. */
#undef GG_CONFIG_HAVE_ZLIB

/* Defined if uintX_t types are defined in <stdint.h>. */
#undef GG_CONFIG_HAVE_STDINT_H

/* Defined if uintX_t types are defined in <inttypes.h>. */
#undef GG_CONFIG_HAVE_INTTYPES_H

/* Defined if uintX_t types are defined in <sys/inttypes.h>. */
#undef GG_CONFIG_HAVE_SYS_INTTYPES_H

/* Defined if uintX_t types are defined in <sys/int_types.h>. */
#undef GG_CONFIG_HAVE_SYS_INT_TYPES_H

/* Defined if uintX_t types are defined in <sys/types.h>. */
#undef GG_CONFIG_HAVE_SYS_TYPES_H

#ifdef GG_CONFIG_HAVE_OPENSSL
#include <openssl/ssl.h>
#endif

#ifdef GG_CONFIG_HAVE_STDINT_H
#include <stdint.h>
#else
#  ifdef GG_CONFIG_HAVE_INTTYPES_H
#  include <inttypes.h>
#  else
#    ifdef GG_CONFIG_HAVE_SYS_INTTYPES_H
#    include <sys/inttypes.h>
#    else
#      ifdef GG_CONFIG_HAVE_SYS_INT_TYPES_H
#      include <sys/int_types.h>
#      else
#        ifdef GG_CONFIG_HAVE_SYS_TYPES_H
#        include <sys/types.h>
#        else

#ifndef __AC_STDINT_H
#define __AC_STDINT_H

/* ISO C 9X: 7.18 Integer types <stdint.h> */

typedef unsigned char   uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#if !defined(__CYGWIN__) && !defined(__SunOS) && !defined(_INCLUDE_HPUX_SOURCE)
#define __int8_t_defined
typedef   signed char    int8_t;
typedef   signed short  int16_t;
typedef   signed int    int32_t;
#endif

#endif /* __AC_STDINT_H */

#        endif
#      endif
#    endif
#  endif
#endif

/** \endcond */

/**
 * Numer Gadu-Gadu.
 */
typedef uint32_t uin_t;

/**
 * Identyfikator połączenia bezpośredniego Gadu-Gadu 7.x.
 */
typedef struct {
	uint8_t id[8];
} gg_dcc7_id_t;

/**
 * Identyfikator sesji multilogowania.
 */
typedef struct {
	uint8_t id[8];
} gg_multilogon_id_t;

/**
 * Makro deklarujące pola wspólne dla struktur sesji.
 */
#define gg_common_head(x) \
	int fd;			/**< Obserwowany deskryptor */ \
	int check;		/**< Informacja o żądaniu odczytu/zapisu (patrz \ref gg_check_t) */ \
	int state;		/**< Aktualny stan połączenia (patrz \ref gg_state_t) */ \
	int error;		/**< Kod błędu dla \c GG_STATE_ERROR (patrz \ref gg_error_t) */ \
	int type;		/**< Rodzaj sesji (patrz \ref gg_session_t) */ \
	int id;			/**< Identyfikator sesji */ \
	int timeout;		/**< Czas pozostały do zakończenia stanu */ \
	int (*callback)(x*); 	/**< Funkcja zwrotna */ \
	void (*destroy)(x*); 	/**< Funkcja zwalniania zasobów */

/**
 * Struktura wspólna dla wszystkich sesji i połączeń. Pozwala na proste
 * rzutowanie niezależne od rodzaju połączenia.
 */
struct gg_common {
	gg_common_head(struct gg_common)
};

struct gg_image_queue;

struct gg_dcc7;

struct gg_dcc7_relay;

/**
 * Sposób rozwiązywania nazw serwerów.
 */
typedef enum {
	GG_RESOLVER_DEFAULT = 0,	/**< Domyślny sposób rozwiązywania nazw (jeden z poniższych) */
	GG_RESOLVER_FORK,		/**< Rozwiązywanie nazw bazujące na procesach */
	GG_RESOLVER_PTHREAD,		/**< Rozwiązywanie nazw bazujące na wątkach */
	GG_RESOLVER_WIN32,
	GG_RESOLVER_CUSTOM,		/**< Funkcje rozwiązywania nazw dostarczone przed aplikację */
	GG_RESOLVER_INVALID = -1	/**< Nieprawidłowy sposób rozwiązywania nazw (wynik \c gg_session_get_resolver) */
} gg_resolver_t;

/**
 * Rodzaj kodowania znaków.
 */
typedef enum {
	GG_ENCODING_CP1250 = 0,		/**< Kodowanie CP1250 */
	GG_ENCODING_UTF8,		/**< Kodowanie UTF-8 */
	GG_ENCODING_INVALID = -1	/**< Nieprawidłowe kodowanie */
} gg_encoding_t;

/**
 * Sesja Gadu-Gadu.
 *
 * Tworzona przez funkcję \c gg_login(), zwalniana przez \c gg_free_session().
 *
 * \ingroup login
 */
struct gg_session {
	gg_common_head(struct gg_session)

	int async;      	/**< Flaga połączenia asynchronicznego */
	int pid;        	/**< Numer procesu rozwiązującego nazwę serwera */
	int port;       	/**< Port serwera */
	int seq;        	/**< Numer sekwencyjny ostatniej wiadomości */
	int last_pong;  	/**< Czas otrzymania ostatniej ramki utrzymaniowej */
	int last_event;		/**< Czas otrzymania ostatniego pakietu */

	struct gg_event *event;	/**< Zdarzenie po wywołaniu \c callback */

	uint32_t proxy_addr;	/**< Adres serwera pośredniczącego */
	uint16_t proxy_port;	/**< Port serwera pośredniczącego */

	uint32_t hub_addr;	/**< Adres huba po rozwiązaniu nazwy */
	uint32_t server_addr;	/**< Adres serwera otrzymany od huba */

	uint32_t client_addr;	/**< Adres gniazda dla połączeń bezpośrednich */
	uint16_t client_port;	/**< Port gniazda dla połączeń bezpośrednich */

	uint32_t external_addr;	/**< Publiczny adres dla połączeń bezpośrednich */
	uint16_t external_port;	/**< Publiczny port dla połączeń bezpośrednich */

	uin_t uin;		/**< Własny numer Gadu-Gadu */
	char *password;		/**< Hasło (zwalniane po użyciu) */

	int initial_status;	/**< Początkowy status */
	int status;		/**< Aktualny status */

	char *recv_buf;		/**< Bufor na odbierany pakiety */
	int recv_done;		/**< Liczba wczytanych bajtów pakietu */
	int recv_left;		/**< Liczba pozostałych do wczytania bajtów pakietu */

	int protocol_version;	/**< Wersja protokołu (bez flag) */
	char *client_version;	/**< Wersja klienta */
	int last_sysmsg;	/**< Numer ostatniej wiadomości systemowej */

	char *initial_descr;	/**< Początkowy opis statusu */

	void *resolver;		/**< Dane prywatne procesu lub wątku rozwiązującego nazwę serwera */

	char *header_buf;	/**< Bufor na początek nagłówka pakietu */
	unsigned int header_done;	/**< Liczba wczytanych bajtów nagłówka pakietu */

#ifdef GG_CONFIG_HAVE_OPENSSL
	SSL *ssl;		/**< Struktura TLS */
	SSL_CTX *ssl_ctx;	/**< Kontekst sesji TLS */
#else
	void *ssl;		/**< Struktura TLS */
	void *ssl_ctx;		/**< Kontekst sesji TLS */
#endif

	int image_size;		/**< Maksymalny rozmiar obsługiwanych obrazków w KiB */

	char *userlist_reply;	/**< Bufor z odbieraną listą kontaktów */

	int userlist_blocks;	/**< Liczba części listy kontaktów */

	struct gg_image_queue *images;	/**< Lista wczytywanych obrazków */

	int hash_type;		/**< Rodzaj funkcji skrótu hasła (\c GG_LOGIN_HASH_GG32 lub \c GG_LOGIN_HASH_SHA1) */

	char *send_buf;		/**< Bufor z danymi do wysłania */
	int send_left;		/**< Liczba bajtów do wysłania */

	struct gg_dcc7 *dcc7_list;	/**< Lista połączeń bezpośrednich skojarzonych z sesją */
	
	int soft_timeout;	/**< Flaga mówiąca, że po przekroczeniu \c timeout należy wywołać \c gg_watch_fd() */

	int protocol_flags;	/**< Flagi protokołu */

	gg_encoding_t encoding;		/**< Rodzaj kodowania znaków */

	gg_resolver_t resolver_type;	/**< Sposób rozwiązywania nazw serwerów */
	int (*resolver_start)(int *fd, void **private_data, const char *hostname);	/**< Funkcja rozpoczynająca rozwiązywanie nazwy */
	void (*resolver_cleanup)(void **private_data, int force);	/**< Funkcja zwalniająca zasoby po rozwiązaniu nazwy */

	int protocol_features;	/**< Opcje protokołu */
	int status_flags;	/**< Flagi statusu */
	int recv_msg_count;	/**< Liczba odebranych wiadomości */
};

/**
 * Połączenie HTTP.
 *
 * Tworzone przez \c gg_http_connect(), zwalniane przez \c gg_http_free().
 *
 * \ingroup http
 */
struct gg_http {
	gg_common_head(struct gg_http)

	int async;              /**< Flaga połączenia asynchronicznego */
	int pid;                /**< Identyfikator procesu rozwiązującego nazwę serwera */
	int port;               /**< Port */

	char *query;            /**< Zapytanie HTTP */
	char *header;           /**< Odebrany nagłówek */
	int header_size;        /**< Rozmiar wczytanego nagłówka */
	char *body;             /**< Odebrana strona */
	unsigned int body_size; /**< Rozmiar strony */

	void *data;             /**< Dane prywatne usługi HTTP */

	char *user_data;	/**< Dane prywatne użytkownika (nie są zwalniane) */

	void *resolver;		/**< Dane prywatne procesu lub wątku rozwiązującego nazwę */

	unsigned int body_done;	/**< Liczba odebranych bajtów strony */

	gg_resolver_t resolver_type;	/**< Sposób rozwiązywania nazw serwerów */
	int (*resolver_start)(int *fd, void **private_data, const char *hostname);	/**< Funkcja rozpoczynająca rozwiązywanie nazwy */
	void (*resolver_cleanup)(void **private_data, int force);	/**< Funkcja zwalniająca zasoby po rozwiązaniu nazwy */
};

/** \cond ignore */

#ifdef __GNUC__
#define GG_PACKED __attribute__ ((packed))
#ifndef GG_IGNORE_DEPRECATED
#define GG_DEPRECATED __attribute__ ((deprecated))
#else
#define GG_DEPRECATED
#endif
#else
#define GG_PACKED
#define GG_DEPRECATED
#endif

/** \endcond */

#define GG_MAX_PATH 276		/**< Maksymalny rozmiar nazwy pliku w strukturze \c gg_file_info */

/**
 * Odpowiednik struktury WIN32_FIND_DATA z API WIN32.
 *
 * Wykorzystywana przy połączeniach bezpośrednich do wersji Gadu-Gadu 6.x.
 */
struct gg_file_info {
	uint32_t mode;			/**< dwFileAttributes */
	uint32_t ctime[2];		/**< ftCreationTime */
	uint32_t atime[2];		/**< ftLastAccessTime */
	uint32_t mtime[2];		/**< ftLastWriteTime */
	uint32_t size_hi;		/**< nFileSizeHigh */
	uint32_t size;			/**< nFileSizeLow */
	uint32_t reserved0;		/**< dwReserved0 */
	uint32_t reserved1;		/**< dwReserved1 */
	unsigned char filename[GG_MAX_PATH - 14];	/**< cFileName */
	unsigned char short_filename[14];		/**< cAlternateFileName */
} /** \cond ignore */ GG_PACKED /** \endcond */;

/**
 * Połączenie bezpośrednie do wersji Gadu-Gadu 6.x.
 *
 * Tworzone przez \c gg_dcc_socket_create(), \c gg_dcc_get_file(),
 * \c gg_dcc_send_file() lub \c gg_dcc_voice_chat(), zwalniane przez
 * \c gg_dcc_free().
 *
 * \ingroup dcc6
 */
struct gg_dcc {
	gg_common_head(struct gg_dcc)

	struct gg_event *event;	/**< Zdarzenie po wywołaniu \c callback */

	int active;		/**< Flaga połączenia aktywnego (nieużywana) */
	int port;		/**< Port gniazda nasłuchującego */
	uin_t uin;		/**< Własny numer Gadu-Gadu */
	uin_t peer_uin;		/**< Numer Gadu-Gadu drugiej strony połączenia */
	int file_fd;		/**< deskryptor pliku */
	unsigned int offset;	/**< Położenie w pliku */
	unsigned int chunk_size;
				/**< Rozmiar kawałka pliku */
	unsigned int chunk_offset;
				/**< Położenie w aktualnym kawałku pliku */
	struct gg_file_info file_info;
				/**< Informacje o pliku */
	int established;	/**< Flaga ustanowienia połączenia */
	char *voice_buf;	/**< Bufor na pakiet połączenia głosowego */
	int incoming;		/**< Flaga połączenia przychodzącego */
	char *chunk_buf;	/**< Bufor na fragment danych */
	uint32_t remote_addr;	/**< Adres drugiej strony */
	uint16_t remote_port;	/**< Port drugiej strony */
};

#define GG_DCC7_HASH_LEN	20	/**< Maksymalny rozmiar skrótu pliku w połączeniach bezpośrenich */
#define GG_DCC7_FILENAME_LEN	255	/**< Maksymalny rozmiar nazwy pliku w połączeniach bezpośrednich */
#define GG_DCC7_INFO_LEN	32	/**< Maksymalny rozmiar informacji o połączeniach bezpośrednich */
#define GG_DCC7_INFO_HASH_LEN	32	/**< Maksymalny rozmiar skrótu ip informacji o połączeniach bezpośrednich */

/**
 * Połączenie bezpośrednie od wersji Gadu-Gadu 7.x.
 *
 * \ingroup dcc7
 */
struct gg_dcc7 {
	gg_common_head(struct gg_dcc7)

	gg_dcc7_id_t cid;	/**< Identyfikator połączenia */

	struct gg_event *event;	/**< Struktura zdarzenia */

	uin_t uin;		/**< Własny numer Gadu-Gadu */
	uin_t peer_uin;		/**< Numer Gadu-Gadu drugiej strony połączenia */

	int file_fd;		/**< Deskryptor przesyłanego pliku */
	unsigned int offset;	/**< Aktualne położenie w przesyłanym pliku */
	unsigned int size;	/**< Rozmiar przesyłanego pliku */
	unsigned char filename[GG_DCC7_FILENAME_LEN + 1];
				/**< Nazwa przesyłanego pliku */
	unsigned char hash[GG_DCC7_HASH_LEN];
				/**< Skrót SHA1 przesyłanego pliku */

	int dcc_type;		/**< Rodzaj połączenia bezpośredniego */
	int established;	/**< Flaga ustanowienia połączenia */
	int incoming;		/**< Flaga połączenia przychodzącego */
	int reverse;		/**< Flaga połączenia zwrotnego */

	uint32_t local_addr;	/**< Adres lokalny */
	uint16_t local_port;	/**< Port lokalny */

	uint32_t remote_addr;	/**< Adres drugiej strony */
	uint16_t remote_port;	/**< Port drugiej strony */

	struct gg_session *sess;
				/**< Sesja do której przypisano połączenie */
	struct gg_dcc7 *next;	/**< Następne połączenie w liście */

	int soft_timeout;	/**< Flaga mówiąca, że po przekroczeniu \c timeout należy wywołać \c gg_dcc7_watch_fd() */
	int seek;		/**< Flaga mówiąca, że można zmieniać położenie w wysyłanym pliku */

	void *resolver;		/**< Dane prywatne procesu lub wątku rozwiązującego nazwę serwera */

	int relay;		/**< Flaga mówiąca, że laczymy sie przez serwer */
	int relay_index;	/**< Numer serwera pośredniczącego, do którego się łączymy */
	int relay_count;	/**< Rozmiar listy serwerów pośredniczących */
	struct gg_dcc7_relay *relay_list;	/**< Lista serwerów pośredniczących */
};

/**
 * Rodzaj sesji.
 */
enum gg_session_t {
	GG_SESSION_GG = 1,	/**< Połączenie z serwerem Gadu-Gadu */
	GG_SESSION_HTTP,	/**< Połączenie HTTP */
	GG_SESSION_SEARCH,	/**< Wyszukiwanie w katalogu publicznym (nieaktualne) */
	GG_SESSION_REGISTER,	/**< Rejestracja nowego konta */
	GG_SESSION_REMIND,	/**< Przypominanie hasła */
	GG_SESSION_PASSWD,	/**< Zmiana hasła */
	GG_SESSION_CHANGE,	/**< Zmiana informacji w katalogu publicznym (nieaktualne) */
	GG_SESSION_DCC,		/**< Połączenie bezpośrednie (do wersji 6.x) */
	GG_SESSION_DCC_SOCKET,	/**< Gniazdo nasłuchujące (do wersji 6.x) */
	GG_SESSION_DCC_SEND,	/**< Wysyłanie pliku (do wersji 6.x) */
	GG_SESSION_DCC_GET,	/**< Odbieranie pliku (do wersji 6.x) */
	GG_SESSION_DCC_VOICE,	/**< Rozmowa głosowa (do wersji 6.x) */
	GG_SESSION_USERLIST_GET,	/**< Import listy kontaktów z serwera (nieaktualne) */
	GG_SESSION_USERLIST_PUT,	/**< Eksport listy kontaktów do serwera (nieaktualne) */
	GG_SESSION_UNREGISTER,	/**< Usuwanie konta */
	GG_SESSION_USERLIST_REMOVE,	/**< Usuwanie listy kontaktów z serwera (nieaktualne) */
	GG_SESSION_TOKEN,	/**< Pobieranie tokenu */
	GG_SESSION_DCC7_SOCKET,	/**< Gniazdo nasłuchujące (od wersji 7.x) */
	GG_SESSION_DCC7_SEND,	/**< Wysyłanie pliku (od wersji 7.x) */
	GG_SESSION_DCC7_GET,	/**< Odbieranie pliku (od wersji 7.x) */
	GG_SESSION_DCC7_VOICE,	/**< Rozmowa głosowa (od wersji 7.x) */

	GG_SESSION_USER0 = 256,	/**< Rodzaj zadeklarowany dla użytkownika */
	GG_SESSION_USER1,	/**< Rodzaj zadeklarowany dla użytkownika */
	GG_SESSION_USER2,	/**< Rodzaj zadeklarowany dla użytkownika */
	GG_SESSION_USER3,	/**< Rodzaj zadeklarowany dla użytkownika */
	GG_SESSION_USER4,	/**< Rodzaj zadeklarowany dla użytkownika */
	GG_SESSION_USER5,	/**< Rodzaj zadeklarowany dla użytkownika */
	GG_SESSION_USER6,	/**< Rodzaj zadeklarowany dla użytkownika */
	GG_SESSION_USER7	/**< Rodzaj zadeklarowany dla użytkownika */
};

/**
 * Aktualny stan sesji.
 */
enum gg_state_t {
	/* wspólne */
	GG_STATE_IDLE = 0,		/**< Nie dzieje się nic */
	GG_STATE_RESOLVING,             /**< Oczekiwanie na rozwiązanie nazwy serwera */
	GG_STATE_CONNECTING,            /**< Oczekiwanie na połączenie */
	GG_STATE_READING_DATA,		/**< Oczekiwanie na dane */
	GG_STATE_ERROR,			/**< Kod błędu w polu \c error */

	/* gg_session */
	GG_STATE_CONNECTING_HUB,	/**< Oczekiwanie na połączenie z hubem */
	GG_STATE_CONNECTING_GG,         /**< Oczekiwanie na połączenie z serwerem */
	GG_STATE_READING_KEY,           /**< Oczekiwanie na klucz */
	GG_STATE_READING_REPLY,         /**< Oczekiwanie na odpowiedź serwera */
	GG_STATE_CONNECTED,             /**< Połączono z serwerem */

	/* gg_http */
	GG_STATE_SENDING_QUERY,		/**< Wysłano zapytanie HTTP */
	GG_STATE_READING_HEADER,	/**< Oczekiwanie na nagłówek HTTP */
	GG_STATE_PARSING,               /**< Przetwarzanie danych */
	GG_STATE_DONE,                  /**< Połączenie zakończone */

	/* gg_dcc */
	GG_STATE_LISTENING,		/* czeka na połączenia */
	GG_STATE_READING_UIN_1,		/* czeka na uin peera */
	GG_STATE_READING_UIN_2,		/* czeka na swój uin */
	GG_STATE_SENDING_ACK,		/* wysyła potwierdzenie dcc */
	GG_STATE_READING_ACK,		/* czeka na potwierdzenie dcc */
	GG_STATE_READING_REQUEST,	/* czeka na komendę */
	GG_STATE_SENDING_REQUEST,	/* wysyła komendę */
	GG_STATE_SENDING_FILE_INFO,	/* wysyła informacje o pliku */
	GG_STATE_READING_PRE_FILE_INFO,	/* czeka na pakiet przed file_info */
	GG_STATE_READING_FILE_INFO,	/* czeka na informacje o pliku */
	GG_STATE_SENDING_FILE_ACK,	/* wysyła potwierdzenie pliku */
	GG_STATE_READING_FILE_ACK,	/* czeka na potwierdzenie pliku */
	GG_STATE_SENDING_FILE_HEADER,	/* wysyła nagłówek pliku */
	GG_STATE_READING_FILE_HEADER,	/* czeka na nagłówek */
	GG_STATE_GETTING_FILE,		/* odbiera plik */
	GG_STATE_SENDING_FILE,		/* wysyła plik */
	GG_STATE_READING_VOICE_ACK,	/* czeka na potwierdzenie voip */
	GG_STATE_READING_VOICE_HEADER,	/* czeka na rodzaj bloku voip */
	GG_STATE_READING_VOICE_SIZE,	/* czeka na rozmiar bloku voip */
	GG_STATE_READING_VOICE_DATA,	/* czeka na dane voip */
	GG_STATE_SENDING_VOICE_ACK,	/* wysyła potwierdzenie voip */
	GG_STATE_SENDING_VOICE_REQUEST,	/* wysyła żądanie voip */
	GG_STATE_READING_TYPE,		/* czeka na typ połączenia */

	/* nowe. bez sensu jest to API. */
	GG_STATE_TLS_NEGOTIATION,	/**< Negocjacja połączenia szyfrowanego */

	GG_STATE_REQUESTING_ID,		/**< Oczekiwanie na nadanie identyfikatora połączenia bezpośredniego */
	GG_STATE_WAITING_FOR_ACCEPT,	/**< Oczekiwanie na potwierdzenie lub odrzucenie połączenia bezpośredniego */
	GG_STATE_WAITING_FOR_INFO,	/**< Oczekiwanie na informacje o połączeniu bezpośrednim */

	GG_STATE_READING_ID,		/**< Odebranie identyfikatora połączenia bezpośredniego */
	GG_STATE_SENDING_ID,		/**< Wysłano identyfikator połączenia bezpośredniego */
	GG_STATE_RESOLVING_GG,		/**< Oczekiwanie na rozwiązanie nazwy serwera Gadu-Gadu */

	GG_STATE_RESOLVING_RELAY,	/**< Oczekiwanie na rozwiązanie nazwy serwera pośredniczącego */
	GG_STATE_CONNECTING_RELAY,	/**< Oczekiwanie na połączenie z serwerem pośredniczącym */
	GG_STATE_READING_RELAY,		/**< Odbieranie danych */

	GG_STATE_DISCONNECTING,		/**< Oczekiwanie na potwierdzenie rozłączenia */
};

/**
 * Informacja o tym, czy biblioteka chce zapisywać i/lub czytać
 * z deskryptora. Maska bitowa.
 *
 * \ingroup events
 */
enum gg_check_t {
	GG_CHECK_NONE = 0,		/**< Nie sprawdzaj niczego */
	GG_CHECK_WRITE = 1,		/**< Sprawdź możliwość zapisu */
	GG_CHECK_READ = 2		/**< Sprawdź możliwość odczytu */
};

/**
 * Flaga połączenia szyfrowanego.
 *
 * \ingroup login
 */
typedef enum {
	GG_SSL_DISABLED = 0,	/**< Połączenie SSL wyłączone */
	GG_SSL_ENABLED,		/**< Połączenie SSL włączone gdy dostępne */
	GG_SSL_REQUIRED		/**< Połączenie SSL wymagane */
} gg_ssl_t;

/**
 * Parametry połączenia z serwerem Gadu-Gadu. Parametry zostały przeniesione
 * do struktury, by uniknąć zmian API po rozszerzeniu protokołu i dodaniu
 * kolejnych opcji połączenia. Część parametrów, które nie są już aktualne
 * lub nie mają znaczenia, została usunięta z dokumentacji.
 *
 * \ingroup login
 */
struct gg_login_params {
	uin_t uin;			/**< Numer Gadu-Gadu */
	char *password;			/**< Hasło */
	int async;			/**< Flaga asynchronicznego połączenia (domyślnie nie) */
	int status;			/**< Początkowy status użytkownika (domyślnie \c GG_STATUS_AVAIL) */
	char *status_descr;		/**< Początkowy opis użytkownika (domyślnie brak) */
	uint32_t server_addr;		/**< Adres serwera Gadu-Gadu (domyślnie pobierany automatycznie) */
	uint16_t server_port;		/**< Port serwera Gadu-Gadu (domyślnie pobierany automatycznie) */
	uint32_t client_addr;		/**< Adres połączeń bezpośrednich (domyślnie dobierany automatycznie) */
	uint16_t client_port;		/**< Port połączeń bezpośrednich (domyślnie dobierany automatycznie) */
	int protocol_version;		/**< Wersja protokołu wysyłana do serwera (domyślnie najnowsza obsługiwana) */
	char *client_version;		/**< Wersja klienta wysyłana do serwera (domyślnie najnowsza znana) */
	int has_audio;			/**< Flaga obsługi połączeń głosowych */
	int last_sysmsg;		/**< Numer ostatnio odebranej wiadomości systemowej */
	uint32_t external_addr;		/**< Adres publiczny dla połączeń bezpośrednich (domyślnie dobierany automatycznie) */
	uint16_t external_port;		/**< Port publiczny dla połączeń bezpośrednich (domyślnie dobierany automatycznie) */
	int tls;			/**< Flaga połączenia szyfrowanego (patrz \ref gg_ssl_t) */
	int image_size;			/**< Maksymalny rozmiar obsługiwanych obrazków w kilobajtach */
#ifndef DOXYGEN
	int era_omnix;			/**< Flaga udawania klienta Era Omnix (nieaktualna) */
#endif
	int hash_type;			/**< Rodzaj skrótu hasła (\c GG_LOGIN_HASH_GG32 lub \c GG_LOGIN_HASH_SHA1, domyślnie SHA1) */
	gg_encoding_t encoding;		/**< Rodzaj kodowania używanego w sesji (domyślnie CP1250) */
	gg_resolver_t resolver;		/**< Sposób rozwiązywania nazw (patrz \ref build-resolver) */
	int protocol_features;		/**< Opcje protokołu (flagi GG_FEATURE_*). */
	int status_flags;		/**< Flagi statusu (flagi GG_STATUS_FLAG_*, patrz \ref status). */

#ifndef DOXYGEN
	char dummy[1 * sizeof(int)];	/**< \internal Miejsce na kilka kolejnych
					  parametrów, żeby wraz z dodawaniem kolejnych
					  parametrów nie zmieniał się rozmiar struktury */
#endif

};

struct gg_session *gg_login(const struct gg_login_params *p);
void gg_free_session(struct gg_session *sess);
void gg_logoff(struct gg_session *sess);
int gg_change_status(struct gg_session *sess, int status);
int gg_change_status_descr(struct gg_session *sess, int status, const char *descr);
int gg_change_status_descr_time(struct gg_session *sess, int status, const char *descr, int time);
int gg_change_status_flags(struct gg_session *sess, int flags);
int gg_send_message(struct gg_session *sess, int msgclass, uin_t recipient, const unsigned char *message);
int gg_send_message_richtext(struct gg_session *sess, int msgclass, uin_t recipient, const unsigned char *message, const unsigned char *format, int formatlen);
int gg_send_message_confer(struct gg_session *sess, int msgclass, int recipients_count, uin_t *recipients, const unsigned char *message);
int gg_send_message_confer_richtext(struct gg_session *sess, int msgclass, int recipients_count, uin_t *recipients, const unsigned char *message, const unsigned char *format, int formatlen);
int gg_send_message_ctcp(struct gg_session *sess, int msgclass, uin_t recipient, const unsigned char *message, int message_len);
int gg_ping(struct gg_session *sess);
int gg_userlist_request(struct gg_session *sess, char type, const char *request);
int gg_userlist100_request(struct gg_session *sess, char type, unsigned int version, char format_type, const char *request);
int gg_image_request(struct gg_session *sess, uin_t recipient, int size, uint32_t crc32);
int gg_image_reply(struct gg_session *sess, uin_t recipient, const char *filename, const char *image, int size);
int gg_typing_notification(struct gg_session *sess, uin_t recipient, int length);

uint32_t gg_crc32(uint32_t crc, const unsigned char *buf, int len);

int gg_session_set_resolver(struct gg_session *gs, gg_resolver_t type);
gg_resolver_t gg_session_get_resolver(struct gg_session *gs);
int gg_session_set_custom_resolver(struct gg_session *gs, int (*resolver_start)(int*, void**, const char*), void (*resolver_cleanup)(void**, int));

int gg_http_set_resolver(struct gg_http *gh, gg_resolver_t type);
gg_resolver_t gg_http_get_resolver(struct gg_http *gh);
int gg_http_set_custom_resolver(struct gg_http *gh, int (*resolver_start)(int*, void**, const char*), void (*resolver_cleanup)(void**, int));

int gg_global_set_resolver(gg_resolver_t type);
gg_resolver_t gg_global_get_resolver(void);
int gg_global_set_custom_resolver(int (*resolver_start)(int*, void**, const char*), void (*resolver_cleanup)(void**, int));

int gg_multilogon_disconnect(struct gg_session *gs, gg_multilogon_id_t conn_id);

/**
 * Rodzaj zdarzenia.
 *
 * \ingroup events
 */
enum gg_event_t {
	GG_EVENT_NONE = 0,		/**< Nie wydarzyło się nic wartego uwagi */
	GG_EVENT_MSG,			/**< \brief Otrzymano wiadomość. Przekazuje również wiadomości systemowe od numeru 0. */
	GG_EVENT_NOTIFY,		/**< \brief Informacja o statusach osób z listy kontaktów (przed 6.0). Zdarzenie należy obsługiwać, jeśli planuje się używać protokołu w wersji starszej niż domyślna. Ostatni element tablicy zawiera uin równy 0, a pozostałe pola są niezainicjowane. */
	GG_EVENT_NOTIFY_DESCR,		/**< \brief Informacja o statusie opisowym osoby z listy kontaktów (przed 6.0). Zdarzenie należy obsługiwać, jeśli planuje się używać protokołu w wersji starszej niż domyślna. */
	GG_EVENT_STATUS,		/**< \brief Zmiana statusu osoby z listy kontaktów (przed 6.0). Zdarzenie należy obsługiwać, jeśli planuje się używać protokołu w wersji starszej niż domyślna. */
	GG_EVENT_ACK,			/**< Potwierdzenie doręczenia wiadomości */
	GG_EVENT_PONG,			/**< \brief Utrzymanie połączenia. Obecnie serwer nie wysyła już do klienta ramek utrzymania połączenia, polega wyłącznie na wysyłaniu ramek przez klienta. */
	GG_EVENT_CONN_FAILED,		/**< \brief Nie udało się połączyć */
	GG_EVENT_CONN_SUCCESS,		/**< \brief Połączono z serwerem. Pierwszą rzeczą, jaką należy zrobić jest wysłanie listy kontaktów. */
	GG_EVENT_DISCONNECT,		/**< \brief Serwer zrywa połączenie. Zdarza się, gdy równolegle do serwera podłączy się druga sesja i trzeba zerwać połączenie z pierwszą. */

	GG_EVENT_DCC_NEW,		/**< Nowe połączenie bezpośrednie (6.x) */
	GG_EVENT_DCC_ERROR,		/**< Błąd połączenia bezpośredniego (6.x) */
	GG_EVENT_DCC_DONE,		/**< Zakończono połączenie bezpośrednie (6.x) */
	GG_EVENT_DCC_CLIENT_ACCEPT,	/**< Moment akceptacji klienta w połączeniu bezpośrednim (6.x) */
	GG_EVENT_DCC_CALLBACK,		/**< Zwrotne połączenie bezpośrednie (6.x) */
	GG_EVENT_DCC_NEED_FILE_INFO,	/**< Należy wypełnić \c file_info dla połączenia bezpośredniego (6.x) */
	GG_EVENT_DCC_NEED_FILE_ACK,	/**< Czeka na potwierdzenie pliku w połączeniu bezpośrednim (6.x) */
	GG_EVENT_DCC_NEED_VOICE_ACK,	/**< Czeka na potwierdzenie rozmowy w połączeniu bezpośrednim (6.x) */
	GG_EVENT_DCC_VOICE_DATA, 	/**< Dane bezpośredniego połączenia głosowego (6.x) */

	GG_EVENT_PUBDIR50_SEARCH_REPLY,	/**< Odpowiedź katalogu publicznego */
	GG_EVENT_PUBDIR50_READ,		/**< Odczytano własne dane z katalogu publicznego */
	GG_EVENT_PUBDIR50_WRITE,	/**< Zmieniono własne dane w katalogu publicznym */

	GG_EVENT_STATUS60,		/**< Zmiana statusu osoby z listy kontaktów */
	GG_EVENT_NOTIFY60,		/**< Informacja o statusach osób z listy kontaktów. Ostatni element tablicy zawiera uin równy 0, a pozostałe pola są niezainicjowane.  */
	GG_EVENT_USERLIST,		/**< Wynik importu lub eksportu listy kontaktów */
	GG_EVENT_IMAGE_REQUEST,		/**< Żądanie przesłania obrazka z wiadomości */
	GG_EVENT_IMAGE_REPLY,		/**< Przysłano obrazek z wiadomości */
	GG_EVENT_DCC_ACK,		/**< Potwierdzenie transmisji w połączeniu bezpośrednim (6.x) */

	GG_EVENT_DCC7_NEW,		/**< Nowe połączenie bezpośrednie (7.x) */
	GG_EVENT_DCC7_ACCEPT,		/**< Zaakceptowano połączenie bezpośrednie (7.x), nowy deskryptor */
	GG_EVENT_DCC7_REJECT,		/**< Odrzucono połączenie bezpośrednie (7.x) */
	GG_EVENT_DCC7_CONNECTED,	/**< Zestawiono połączenie bezpośrednie (7.x), nowy deskryptor */
	GG_EVENT_DCC7_ERROR,		/**< Błąd połączenia bezpośredniego (7.x) */
	GG_EVENT_DCC7_DONE,		/**< Zakończono połączenie bezpośrednie (7.x) */
	GG_EVENT_DCC7_PENDING,		/**< Trwa próba połączenia bezpośredniego (7.x), nowy deskryptor */

	GG_EVENT_XML_EVENT,		/**< Otrzymano komunikat systemowy (7.7) */
	GG_EVENT_DISCONNECT_ACK,	/**< \brief Potwierdzenie zakończenia sesji. Informuje o tym, że zmiana stanu na niedostępny z opisem dotarła do serwera i można zakończyć połączenie TCP. */
	GG_EVENT_TYPING_NOTIFICATION,	/**< Powiadomienie o pisaniu */
	GG_EVENT_USER_DATA,		/**< Informacja o kontaktach */
	GG_EVENT_MULTILOGON_MSG,	/**< Wiadomość wysłana z innej sesji multilogowania */
	GG_EVENT_MULTILOGON_INFO,	/**< Informacja o innych sesjach multilogowania */

	GG_EVENT_USERLIST100_VERSION,	/**< Otrzymano numer wersji listy kontaktów na serwerze (10.0) */
	GG_EVENT_USERLIST100_REPLY,	/**< Wynik importu lub eksportu listy kontaktów (10.0) */
};

#define GG_EVENT_SEARCH50_REPLY GG_EVENT_PUBDIR50_SEARCH_REPLY

/**
 * Powód nieudanego połączenia.
 */
enum gg_failure_t {
	GG_FAILURE_RESOLVING = 1,	/**< Nie znaleziono serwera */
	GG_FAILURE_CONNECTING,		/**< Błąd połączenia */
	GG_FAILURE_INVALID,		/**< Serwer zwrócił nieprawidłowe dane */
	GG_FAILURE_READING,		/**< Zerwano połączenie podczas odczytu */
	GG_FAILURE_WRITING,		/**< Zerwano połączenie podczas zapisu */
	GG_FAILURE_PASSWORD,		/**< Nieprawidłowe hasło */
	GG_FAILURE_404, 		/**< Nieużywane */
	GG_FAILURE_TLS,			/**< Błąd negocjacji szyfrowanego połączenia */
	GG_FAILURE_NEED_EMAIL, 		/**< Serwer rozłączył nas z prośbą o zmianę adresu e-mail */
	GG_FAILURE_INTRUDER,		/**< Zbyt wiele prób połączenia z nieprawidłowym hasłem */
	GG_FAILURE_UNAVAILABLE,		/**< Serwery są wyłączone */
	GG_FAILURE_PROXY,		/**< Błąd serwera pośredniczącego */
	GG_FAILURE_HUB,			/**< Błąd połączenia z hubem */
};

/**
 * Kod błędu danej operacji.
 *
 * Nie zawiera przesadnie szczegółowych informacji o powodach błędów, by nie
 * komplikować ich obsługi. Jeśli wymagana jest większa dokładność, należy
 * sprawdzić zawartość zmiennej systemowej \c errno.
 */
enum gg_error_t {
	GG_ERROR_RESOLVING = 1,		/**< Nie znaleziono hosta */
	GG_ERROR_CONNECTING,		/**< Błąd połączenia */
	GG_ERROR_READING,		/**< Błąd odczytu/odbierania */
	GG_ERROR_WRITING,		/**< Błąd zapisu/wysyłania */

	GG_ERROR_DCC_HANDSHAKE,		/**< Błąd negocjacji */
	GG_ERROR_DCC_FILE,		/**< Błąd odczytu/zapisu pliku */
	GG_ERROR_DCC_EOF,		/**< Przedwczesny koniec pliku */
	GG_ERROR_DCC_NET,		/**< Błąd wysyłania/odbierania */
	GG_ERROR_DCC_REFUSED, 		/**< Połączenie odrzucone */

	GG_ERROR_DCC7_HANDSHAKE,	/**< Błąd negocjacji */
	GG_ERROR_DCC7_FILE,		/**< Błąd odczytu/zapisu pliku */
	GG_ERROR_DCC7_EOF,		/**< Przedwczesny koniec pliku */
	GG_ERROR_DCC7_NET,		/**< Błąd wysyłania/odbierania */
	GG_ERROR_DCC7_REFUSED, 		/**< Połączenie odrzucone */
	GG_ERROR_DCC7_RELAY,		/**< Problem z serwerem pośredniczącym */
};

/**
 * Pole zapytania lub odpowiedzi katalogu publicznego.
 */
struct gg_pubdir50_entry {
	int num;	/**< Numer wyniku */
	char *field;	/**< Nazwa pola */
	char *value;	/**< Wartość pola */
} /* GG_DEPRECATED */;

/**
 * Zapytanie lub odpowiedź katalogu publicznego.
 *
 * Patrz \c gg_pubdir50_t.
 */
struct gg_pubdir50_s {
	int count;	/**< Liczba wyników odpowiedzi */
	uin_t next;	/**< Numer początkowy następnego zapytania */
	int type;	/**< Rodzaj zapytania */
	uint32_t seq;	/**< Numer sekwencyjny */
	struct gg_pubdir50_entry *entries;	/**< Pola zapytania lub odpowiedzi */
	int entries_count;	/**< Liczba pól */
} /* GG_DEPRECATED */;

/**
 * Zapytanie lub odpowiedź katalogu publicznego.
 *
 * Do pól nie należy się odwoływać bezpośrednio -- wszystkie niezbędne
 * informacje są dostępne za pomocą funkcji \c gg_pubdir50_*
 */
typedef struct gg_pubdir50_s *gg_pubdir50_t;

/**
 * Opis zdarzeń \c GG_EVENT_MSG i \c GG_EVENT_MULTILOGON_MSG.
 */
struct gg_event_msg {
	uin_t sender;		/**< Numer nadawcy/odbiorcy */
	int msgclass;		/**< Klasa wiadomości */
	time_t time;		/**< Czas nadania */
	unsigned char *message;	/**< Treść wiadomości */

	int recipients_count;	/**< Liczba odbiorców konferencji */
	uin_t *recipients;	/**< Odbiorcy konferencji */

	int formats_length;	/**< Długość informacji o formatowaniu tekstu */
	void *formats;		/**< Informacje o formatowaniu tekstu */
	uint32_t seq;		/**< Numer sekwencyjny wiadomości */

	char *xhtml_message;	/**< Treść wiadomości w formacie XHTML (może być równe \c NULL, jeśli wiadomość nie zawiera treści XHTML) */
};

/**
 * Opis zdarzenia \c GG_EVENT_NOTIFY_DESCR.
 */
struct gg_event_notify_descr {
	struct gg_notify_reply *notify;	/**< Informacje o liście kontaktów */
	char *descr;		/**< Opis status */
};

/**
 * Opis zdarzenia \c GG_EVENT_STATUS.
 */
struct gg_event_status {
	uin_t uin;		/**< Numer Gadu-Gadu */
	uint32_t status;	/**< Nowy status */
	char *descr;		/**< Opis */
};

/**
 * Opis zdarzenia \c GG_EVENT_STATUS60.
 */
struct gg_event_status60 {
	uin_t uin;		/**< Numer Gadu-Gadu */
	int status;		/**< Nowy status */
	uint32_t remote_ip;	/**< Adres IP dla połączeń bezpośrednich */
	uint16_t remote_port;	/**< Port dla połączeń bezpośrednich */
	int version;		/**< Wersja protokołu */
	int image_size;		/**< Maksymalny rozmiar obsługiwanych obrazków w KiB */
	char *descr;		/**< Opis statusu */
	time_t time;		/**< Czas powrotu */
};

/**
 * Opis zdarzenia \c GG_EVENT_NOTIFY_REPLY60.
 */
struct gg_event_notify60 {
	uin_t uin;		/**< Numer Gadu-Gadu. W ostatnim elemencie jest równy 0, a pozostałe pola są niezainicjowane. */
	int status;		/**< Nowy status */
	uint32_t remote_ip;	/**< Adres IP dla połączeń bezpośrednich */
	uint16_t remote_port;	/**< Port dla połączeń bezpośrednich */
	int version;		/**< Wersja protokołu */
	int image_size;		/**< Maksymalny rozmiar obsługiwanych obrazków w KiB */
	char *descr;		/**< Opis statusu */
	time_t time;		/**< Czas powrotu */
};

/**
 * Opis zdarzenia \c GG_EVENT_ACK.
 */
struct gg_event_ack {
	uin_t recipient;	/**< Numer odbiorcy */
	int status;		/**< Status doręczenia */
	int seq;		/**< Numer sekwencyjny wiadomości */
};

/**
 * Opis zdarzenia \c GG_EVENT_USERLIST.
 */
struct gg_event_userlist {
	char type;		/**< Rodzaj odpowiedzi */
	char *reply;		/**< Treść odpowiedzi */
};

/**
 * Opis zdarzenia \c GG_EVENT_DCC_VOICE_DATA.
 */
struct gg_event_dcc_voice_data {
	uint8_t *data;		/**< Dane dźwiękowe */
	int length;		/**< Rozmiar danych dźwiękowych */
};

/**
 * Opis zdarzenia \c GG_EVENT_IMAGE_REQUEST.
 */
struct gg_event_image_request {
	uin_t sender;		/**< Nadawca żądania */
	uint32_t size;		/**< Rozmiar obrazka */
	uint32_t crc32;		/**< Suma kontrolna CRC32 */
};

/**
 * Opis zdarzenia \c GG_EVENT_IMAGE_REPLY.
 */
struct gg_event_image_reply {
	uin_t sender;		/**< Nadawca obrazka */
	uint32_t size;		/**< Rozmiar obrazka */
	uint32_t crc32;		/**< Suma kontrolna CRC32 */
	char *filename;		/**< Nazwa pliku */
	char *image;		/**< Bufor z obrazkiem */
};

/**
 * Opis zdarzenia \c GG_EVENT_XML_EVENT.
 */
struct gg_event_xml_event {
	char *data;		/**< Bufor z komunikatem */
};

/**
 * Opis zdarzenia \c GG_EVENT_DCC7_CONNECTED.
 */
struct gg_event_dcc7_connected {
	struct gg_dcc7 *dcc7;	/**< Struktura połączenia */
};

/**
 * Opis zdarzenia \c GG_EVENT_DCC7_PENDING.
 */
struct gg_event_dcc7_pending {
	struct gg_dcc7 *dcc7;	/**< Struktura połączenia */
};

/**
 * Opis zdarzenia \c GG_EVENT_DCC7_REJECT.
 */
struct gg_event_dcc7_reject {
	struct gg_dcc7 *dcc7;	/**< Struktura połączenia */
	int reason;		/**< powód odrzucenia */
};

/**
 * Opis zdarzenia \c GG_EVENT_DCC7_ACCEPT.
 */
struct gg_event_dcc7_accept {
	struct gg_dcc7 *dcc7;	/**< Struktura połączenia */
	int type;		/**< Sposób połączenia (P2P, przez serwer) */
	uint32_t remote_ip;	/**< Adres zdalnego klienta */
	uint16_t remote_port;	/**< Port zdalnego klienta */
};

/**
 * Opis zdarzenia \c GG_EVENT_DCC7_DONE.
 */
struct gg_event_dcc7_done {
	struct gg_dcc7 *dcc7;	/**< Struktura połączenia */
};

/**
 * Opis zdarzenia \c GG_EVENT_TYPING_NOTIFICATION.
 */
struct gg_event_typing_notification {
	uin_t uin;		/**< Numer rozmówcy */
	int length;		/**< Długość tekstu */
};

/**
 * Atrybut użytkownika.
 */
struct gg_event_user_data_attr {
	int type;		/**< Typ atrybutu */
	char *key;		/**< Klucz */
	char *value;		/**< Wartość */
};

/**
 * Struktura opisująca kontakt w zdarzeniu GG_EVENT_USER_DATA.
 */
struct gg_event_user_data_user {
	uin_t uin;		/**< Numer kontaktu */
	size_t attr_count;	/**< Liczba atrybutów */
	struct gg_event_user_data_attr *attrs;	/**< Lista atrybutów */
};

/**
 * Opis zdarzenia \c GG_EVENT_USER_DATA.
 */
struct gg_event_user_data {
	int type;		/**< Rodzaj informacji o kontaktach */
	size_t user_count;	/**< Liczba kontaktów */
	struct gg_event_user_data_user *users;	/**< Lista kontaktów */
};

/**
 * Struktura opisująca sesję multilogowania.
 */
struct gg_multilogon_session {
	gg_multilogon_id_t id;		/**< Identyfikator sesji */
	char *name;			/**< Nazwa sesji (podana w \c gg_login_params.client_version) */
	uint32_t remote_addr;		/**< Adres sesji */
	int status_flags;		/**< Flagi statusu sesji */
	int protocol_features;		/**< Opcje protokolu sesji */
	time_t logon_time;		/**< Czas zalogowania */
};

/**
 * Opis zdarzenia \c GG_EVENT_MULTILOGON_INFO.
 */
struct gg_event_multilogon_info {
	int count;		/**< Liczba sesji */
	struct gg_multilogon_session *sessions;	/** Lista sesji */
};

/**
 * Opis zdarzenia \c GG_EVENT_USERLIST100_VERSION.
 */
struct gg_event_userlist100_version {
	uint32_t version;		/**< Numer wersji listy kontaktów na serwerze */
};

/**
 * Opis zdarzenia \c GG_EVENT_USERLIST100_REPLY.
 */
struct gg_event_userlist100_reply {
	char type;			/**< Rodzaj odpowiedzi */
	uint32_t version;		/**< Aktualna wersja listy kontaktów na serwerze */
	char format_type;		/**< Typ formatu listy kontaktów (żądany w \c gg_userlist100_request.format_type) */
	char *reply;			/**< Treść listy kontaktów w przesyłanej wersji i formacie */
};

/**
 * Unia wszystkich zdarzeń zwracanych przez funkcje \c gg_watch_fd(), 
 * \c gg_dcc_watch_fd() i \c gg_dcc7_watch_fd().
 *
 * \ingroup events
 */
union gg_event_union {
	enum gg_failure_t failure;	/**< Błąd połączenia (\c GG_EVENT_CONN_FAILED) */
	struct gg_notify_reply *notify;	/**< Zmiana statusu kontaktów (\c GG_EVENT_NOTIFY) */
	struct gg_event_notify_descr notify_descr;	/**< Zmiana statusu kontaktów (\c GG_EVENT_NOTIFY_DESCR) */
	struct gg_event_status status;	/**< Zmiana statusu kontaktów (\c GG_EVENT_STATUS) */
	struct gg_event_status60 status60;	/**< Zmiana statusu kontaktów (\c GG_EVENT_STATUS60) */
	struct gg_event_notify60 *notify60;	/**< Zmiana statusu kontaktów (\c GG_EVENT_NOTIFY60) */
	struct gg_event_msg msg;	/**< Otrzymano wiadomość (\c GG_EVENT_MSG) */
	struct gg_event_ack ack;	/**< Potwierdzenie wiadomości (\c GG_EVENT_ACK) */
	struct gg_event_image_request image_request;	/**< Żądanie wysłania obrazka (\c GG_EVENT_IMAGE_REQUEST) */
	struct gg_event_image_reply image_reply;	/**< Odpowiedź z obrazkiem (\c GG_EVENT_IMAGE_REPLY) */
	struct gg_event_userlist userlist;	/**< Odpowiedź listy kontaktów (\c GG_EVENT_USERLIST) */
	gg_pubdir50_t pubdir50;	/**< Odpowiedź katalogu publicznego (\c GG_EVENT_PUBDIR50_*) */
	struct gg_event_xml_event xml_event;	/**< Zdarzenie systemowe (\c GG_EVENT_XML_EVENT) */
	struct gg_dcc *dcc_new;	/**< Nowe połączenie bezpośrednie (\c GG_EVENT_DCC_NEW) */
	enum gg_error_t dcc_error;	/**< Błąd połączenia bezpośredniego (\c GG_EVENT_DCC_ERROR) */
	struct gg_event_dcc_voice_data dcc_voice_data;	/**< Dane połączenia głosowego (\c GG_EVENT_DCC_VOICE_DATA) */
	struct gg_dcc7 *dcc7_new;	/**< Nowe połączenie bezpośrednie (\c GG_EVENT_DCC7_NEW) */
	enum gg_error_t dcc7_error;	/**< Błąd połączenia bezpośredniego (\c GG_EVENT_DCC7_ERROR) */
	struct gg_event_dcc7_connected dcc7_connected;	/**< Informacja o zestawieniu połączenia bezpośredniego (\c GG_EVENT_DCC7_CONNECTED) */
	struct gg_event_dcc7_pending dcc7_pending;	/**< Trwa próba połączenia bezpośredniego (\c GG_EVENT_DCC7_PENDING) */
	struct gg_event_dcc7_reject dcc7_reject;	/**< Odrzucono połączenia bezpośredniego (\c GG_EVENT_DCC7_REJECT) */
	struct gg_event_dcc7_accept dcc7_accept;	/**< Zaakceptowano połączenie bezpośrednie (\c GG_EVENT_DCC7_ACCEPT) */
	struct gg_event_dcc7_done dcc7_done;	/**< Zakończono połączenie bezpośrednie (\c GG_EVENT_DCC7_DONE) */
	struct gg_event_typing_notification typing_notification;	/**< Powiadomienie o pisaniu */
	struct gg_event_user_data user_data;	/**< Informacje o kontaktach */
	struct gg_event_msg multilogon_msg;	/**< Inna sesja wysłała wiadomość (\c GG_EVENT_MULTILOGON_MSG) */
	struct gg_event_multilogon_info multilogon_info;	/**< Informacja o innych sesjach multilogowania (\c GG_EVENT_MULTILOGON_INFO) */
	struct gg_event_userlist100_version userlist100_version;	/**< Informacja o numerze wersji listy kontaktów na serwerze (\c GG_EVENT_USERLIST100_VERSION) */
	struct gg_event_userlist100_reply userlist100_reply;	/**< Odpowiedź listy kontaktów (10.0) (\c GG_EVENT_USERLIST100_REPLY) */
};

/**
 * Opis zdarzenia.
 *
 * Zwracany przez funkcje \c gg_watch_fd(), \c gg_dcc_watch_fd()
 * i \c gg_dcc7_watch_fd(). Po przeanalizowaniu należy zwolnić
 * za pomocą \c gg_event_free().
 *
 * \ingroup events
 */
struct gg_event {
	int type;			/**< Rodzaj zdarzenia */
	union gg_event_union event;	/**< Informacja o zdarzeniu */
};

struct gg_event *gg_watch_fd(struct gg_session *sess);
void gg_event_free(struct gg_event *e);

int gg_notify_ex(struct gg_session *sess, uin_t *userlist, char *types, int count);
int gg_notify(struct gg_session *sess, uin_t *userlist, int count);
int gg_add_notify_ex(struct gg_session *sess, uin_t uin, char type);
int gg_add_notify(struct gg_session *sess, uin_t uin);
int gg_remove_notify_ex(struct gg_session *sess, uin_t uin, char type);
int gg_remove_notify(struct gg_session *sess, uin_t uin);

struct gg_http *gg_http_connect(const char *hostname, int port, int async, const char *method, const char *path, const char *header);
int gg_http_watch_fd(struct gg_http *h);
void gg_http_stop(struct gg_http *h);
void gg_http_free(struct gg_http *h);

uint32_t gg_pubdir50(struct gg_session *sess, gg_pubdir50_t req);
gg_pubdir50_t gg_pubdir50_new(int type);
int gg_pubdir50_add(gg_pubdir50_t req, const char *field, const char *value);
int gg_pubdir50_seq_set(gg_pubdir50_t req, uint32_t seq);
const char *gg_pubdir50_get(gg_pubdir50_t res, int num, const char *field);
int gg_pubdir50_type(gg_pubdir50_t res);
int gg_pubdir50_count(gg_pubdir50_t res);
uin_t gg_pubdir50_next(gg_pubdir50_t res);
uint32_t gg_pubdir50_seq(gg_pubdir50_t res);
void gg_pubdir50_free(gg_pubdir50_t res);

#ifndef DOXYGEN

#define GG_PUBDIR50_UIN "FmNumber"
#define GG_PUBDIR50_STATUS "FmStatus"
#define GG_PUBDIR50_FIRSTNAME "firstname"
#define GG_PUBDIR50_LASTNAME "lastname"
#define GG_PUBDIR50_NICKNAME "nickname"
#define GG_PUBDIR50_BIRTHYEAR "birthyear"
#define GG_PUBDIR50_CITY "city"
#define GG_PUBDIR50_GENDER "gender"
#define GG_PUBDIR50_GENDER_FEMALE "1"
#define GG_PUBDIR50_GENDER_MALE "2"
#define GG_PUBDIR50_GENDER_SET_FEMALE "2"
#define GG_PUBDIR50_GENDER_SET_MALE "1"
#define GG_PUBDIR50_ACTIVE "ActiveOnly"
#define GG_PUBDIR50_ACTIVE_TRUE "1"
#define GG_PUBDIR50_START "fmstart"
#define GG_PUBDIR50_FAMILYNAME "familyname"
#define GG_PUBDIR50_FAMILYCITY "familycity"

#else

/** 
 * \ingroup pubdir50
 *
 * Rodzaj pola zapytania.
 */
enum {
	GG_PUBDIR50_UIN,	/**< Numer Gadu-Gadu */
	GG_PUBDIR50_STATUS,	/**< Status (tylko wynik wyszukiwania) */
	GG_PUBDIR50_FIRSTNAME,	/**< Imię */
	GG_PUBDIR50_LASTNAME,	/**< Nazwisko */
	GG_PUBDIR50_NICKNAME,	/**< Pseudonim */
	GG_PUBDIR50_BIRTHYEAR,	/**< Rok urodzenia lub przedział lat oddzielony spacją */
	GG_PUBDIR50_CITY,	/**< Miejscowość */
	GG_PUBDIR50_GENDER,	/**< Płeć */
	GG_PUBDIR50_ACTIVE,	/**< Osoba dostępna (tylko wyszukiwanie) */
	GG_PUBDIR50_START,	/**< Numer początkowy wyszukiwania (tylko wyszukiwanie) */
	GG_PUBDIR50_FAMILYNAME,	/**< Nazwisko rodowe (tylko wysyłanie informacji o sobie) */
	GG_PUBDIR50_FAMILYCITY,	/**< Miejscowość pochodzenia (tylko wysyłanie informacji o sobie) */
};

/**
 * \ingroup pubdir50
 *
 * Wartość pola GG_PUBDIR50_GENDER przy wyszukiwaniu. Brak pola oznacza dowolną płeć.
 */
enum {
	GG_PUBDIR50_GENDER_FEMALE,	/**< Kobieta */
	GG_PUBDIR50_GENDER_MALE,	/**< Mężczyzna */
};

/**
 * \ingroup pubdir50
 *
 * Wartość pola GG_PUBDIR50_GENDER przy wysyłaniu informacji o sobie.
 */
enum {
	GG_PUBDIR50_GENDER_SET_FEMALE,	/**< Kobieta */
	GG_PUBDIR50_GENDER_SET_MALE,	/**< Mężczyzna */
};

/**
 * \ingroup pubdir50
 *
 * Wartość pola GG_PUBDIR50_ACTIVE.
 */
enum {
	GG_PUBDIR50_ACTIVE_TRUE,	/**< Wyszukaj tylko osoby dostępne */
};

#endif	/* DOXYGEN */

/**
 * Wynik operacji na katalogu publicznym.
 *
 * \ingroup http
 */
struct gg_pubdir {
	int success;		/**< Flaga powodzenia operacji */
	uin_t uin;		/**< Otrzymany numer lub 0 w przypadku błędu */
};

int gg_pubdir_watch_fd(struct gg_http *f);
void gg_pubdir_free(struct gg_http *f);

/**
 * Token autoryzacji niektórych operacji HTTP.
 * 
 * \ingroup token
 */
struct gg_token {
	int width;		/**< Szerokość obrazka */
	int height;		/**< Wysokość obrazka */
	int length;		/**< Liczba znaków w tokenie */
	char *tokenid;		/**< Identyfikator tokenu */
};

struct gg_http *gg_token(int async);
int gg_token_watch_fd(struct gg_http *h);
void gg_token_free(struct gg_http *h);

struct gg_http *gg_register3(const char *email, const char *password, const char *tokenid, const char *tokenval, int async);
#ifndef DOXYGEN
#define gg_register_watch_fd gg_pubdir_watch_fd
#define gg_register_free gg_pubdir_free
#endif

struct gg_http *gg_unregister3(uin_t uin, const char *password, const char *tokenid, const char *tokenval, int async);
#ifndef DOXYGEN
#define gg_unregister_watch_fd gg_pubdir_watch_fd
#define gg_unregister_free gg_pubdir_free
#endif

struct gg_http *gg_remind_passwd3(uin_t uin, const char *email, const char *tokenid, const char *tokenval, int async);
#ifndef DOXYGEN
#define gg_remind_passwd_watch_fd gg_pubdir_watch_fd
#define gg_remind_passwd_free gg_pubdir_free
#endif

struct gg_http *gg_change_passwd4(uin_t uin, const char *email, const char *passwd, const char *newpasswd, const char *tokenid, const char *tokenval, int async);
#ifndef DOXYGEN
#define gg_change_passwd_watch_fd gg_pubdir_watch_fd
#define gg_change_passwd_free gg_pubdir_free
#endif

extern int gg_dcc_port;
extern unsigned long gg_dcc_ip;

int gg_dcc_request(struct gg_session *sess, uin_t uin);

struct gg_dcc *gg_dcc_send_file(uint32_t ip, uint16_t port, uin_t my_uin, uin_t peer_uin);
struct gg_dcc *gg_dcc_get_file(uint32_t ip, uint16_t port, uin_t my_uin, uin_t peer_uin);
struct gg_dcc *gg_dcc_voice_chat(uint32_t ip, uint16_t port, uin_t my_uin, uin_t peer_uin);
void gg_dcc_set_type(struct gg_dcc *d, int type);
int gg_dcc_fill_file_info(struct gg_dcc *d, const char *filename);
int gg_dcc_fill_file_info2(struct gg_dcc *d, const char *filename, const char *local_filename);
int gg_dcc_voice_send(struct gg_dcc *d, char *buf, int length);

#define GG_DCC_VOICE_FRAME_LENGTH 195		/**< Rozmiar pakietu głosowego przed wersją Gadu-Gadu 5.0.5 */
#define GG_DCC_VOICE_FRAME_LENGTH_505 326	/**< Rozmiar pakietu głosowego od wersji Gadu-Gadu 5.0.5 */

struct gg_dcc *gg_dcc_socket_create(uin_t uin, uint16_t port);
#ifndef DOXYGEN
#define gg_dcc_socket_free gg_dcc_free
#define gg_dcc_socket_watch_fd gg_dcc_watch_fd
#endif

struct gg_event *gg_dcc_watch_fd(struct gg_dcc *d);

void gg_dcc_free(struct gg_dcc *c);

struct gg_event *gg_dcc7_watch_fd(struct gg_dcc7 *d);
struct gg_dcc7 *gg_dcc7_send_file(struct gg_session *sess, uin_t rcpt, const char *filename, const char *filename1250, const char *hash);
struct gg_dcc7 *gg_dcc7_send_file_fd(struct gg_session *sess, uin_t rcpt, int fd, size_t size, const char *filename1250, const char *hash);
int gg_dcc7_accept(struct gg_dcc7 *dcc, unsigned int offset);
int gg_dcc7_reject(struct gg_dcc7 *dcc, int reason);
void gg_dcc7_free(struct gg_dcc7 *d);

extern int gg_debug_level;

extern void (*gg_debug_handler)(int level, const char *format, va_list ap);
extern void (*gg_debug_handler_session)(struct gg_session *sess, int level, const char *format, va_list ap);

extern FILE *gg_debug_file;

/**
 * \ingroup debug
 * @{
 */
#define GG_DEBUG_NET 1		/**< Rejestracja zdarzeń związanych z siecią */
#define GG_DEBUG_TRAFFIC 2	/**< Rejestracja ruchu sieciowego */
#define GG_DEBUG_DUMP 4		/**< Rejestracja zawartości pakietów */
#define GG_DEBUG_FUNCTION 8	/**< Rejestracja wywołań funkcji */
#define GG_DEBUG_MISC 16	/**< Rejestracja różnych informacji */
/** @} */

#ifdef GG_DEBUG_DISABLE
#define gg_debug(x, y...) do { } while(0)
#define gg_debug_session(z, x, y...) do { } while(0)
#else
void gg_debug(int level, const char *format, ...);
void gg_debug_session(struct gg_session *sess, int level, const char *format, ...);
#endif

const char *gg_libgadu_version(void);

/**
 * Lista funkcji biblioteki, które zależą od zewnętrznych bibliotek.
 *
 * \ingroup version
 */
typedef enum {
	GG_LIBGADU_FEATURE_SSL,		/**< Biblioteka obsługuje połączenia szyfrowane */
	GG_LIBGADU_FEATURE_PTHREAD,	/**< Biblioteka obsługuje rozwiązywanie nazw za pomocą wątków */
	GG_LIBGADU_FEATURE_USERLIST100,	/**< Biblioteka obsługuje listę kontaktów zgodną z Gadu-Gadu 10 */
} gg_libgadu_feature_t;

int gg_libgadu_check_feature(gg_libgadu_feature_t feature);

extern int gg_proxy_enabled;
extern char *gg_proxy_host;
extern int gg_proxy_port;
extern char *gg_proxy_username;
extern char *gg_proxy_password;
extern int gg_proxy_http_only;

extern unsigned long gg_local_ip;

#define GG_LOGIN_HASH_GG32 0x01	/**< Algorytm Gadu-Gadu */
#define GG_LOGIN_HASH_SHA1 0x02	/**< Algorytm SHA1 */

#ifndef DOXYGEN

#define GG_PUBDIR50_WRITE 0x01
#define GG_PUBDIR50_READ 0x02
#define GG_PUBDIR50_SEARCH 0x03
#define GG_PUBDIR50_SEARCH_REQUEST GG_PUBDIR50_SEARCH
#define GG_PUBDIR50_SEARCH_REPLY 0x05

#else

/**
 * \ingroup pubdir50
 * 
 * Rodzaj zapytania lub odpowiedzi katalogu publicznego.
 */
enum {
	GG_PUBDIR50_WRITE,	/**< Wysłanie do serwera informacji o sobie */
	GG_PUBDIR50_READ,	/**< Pobranie z serwera informacji o sobie */
	GG_PUBDIR50_SEARCH,	/**< Wyszukiwanie w katalogu publicznym */
	GG_PUBDIR50_SEARCH_REPLY,	/**< Wynik wyszukiwania w katalogu publicznym */
};

#endif	/* DOXYGEN */

/** \cond obsolete */

#define gg_free_event gg_event_free
#define gg_free_http gg_http_free
#define gg_free_pubdir gg_pubdir_free
#define gg_free_register gg_pubdir_free
#define gg_free_remind_passwd gg_pubdir_free
#define gg_free_dcc gg_dcc_free
#define gg_free_change_passwd gg_pubdir_free

struct gg_search_request {
	int active;
	unsigned int start;
	char *nickname;
	char *first_name;
	char *last_name;
	char *city;
	int gender;
	int min_birth;
	int max_birth;
	char *email;
	char *phone;
	uin_t uin;
} /* GG_DEPRECATED */;

struct gg_search {
	int count;
	struct gg_search_result *results;
} GG_DEPRECATED;

struct gg_search_result {
	uin_t uin;
	char *first_name;
	char *last_name;
	char *nickname;
	int born;
	int gender;
	char *city;
	int active;
} GG_DEPRECATED;

#define GG_GENDER_NONE 0
#define GG_GENDER_FEMALE 1
#define GG_GENDER_MALE 2

struct gg_http *gg_search(const struct gg_search_request *r, int async) GG_DEPRECATED;
int gg_search_watch_fd(struct gg_http *f) GG_DEPRECATED;
void gg_free_search(struct gg_http *f) GG_DEPRECATED;
#define gg_search_free gg_free_search

const struct gg_search_request *gg_search_request_mode_0(char *nickname, char *first_name, char *last_name, char *city, int gender, int min_birth, int max_birth, int active, int start) GG_DEPRECATED;
const struct gg_search_request *gg_search_request_mode_1(char *email, int active, int start) GG_DEPRECATED;
const struct gg_search_request *gg_search_request_mode_2(char *phone, int active, int start) GG_DEPRECATED;
const struct gg_search_request *gg_search_request_mode_3(uin_t uin, int active, int start) GG_DEPRECATED;
void gg_search_request_free(struct gg_search_request *r) GG_DEPRECATED;

struct gg_http *gg_register(const char *email, const char *password, int async) GG_DEPRECATED;
struct gg_http *gg_register2(const char *email, const char *password, const char *qa, int async) GG_DEPRECATED;

struct gg_http *gg_unregister(uin_t uin, const char *password, const char *email, int async) GG_DEPRECATED;
struct gg_http *gg_unregister2(uin_t uin, const char *password, const char *qa, int async) GG_DEPRECATED;

struct gg_http *gg_remind_passwd(uin_t uin, int async) GG_DEPRECATED;
struct gg_http *gg_remind_passwd2(uin_t uin, const char *tokenid, const char *tokenval, int async) GG_DEPRECATED;

struct gg_http *gg_change_passwd(uin_t uin, const char *passwd, const char *newpasswd, const char *newemail, int async) GG_DEPRECATED;
struct gg_http *gg_change_passwd2(uin_t uin, const char *passwd, const char *newpasswd, const char *email, const char *newemail, int async) GG_DEPRECATED;
struct gg_http *gg_change_passwd3(uin_t uin, const char *passwd, const char *newpasswd, const char *qa, int async) GG_DEPRECATED;

struct gg_change_info_request {
	char *first_name;
	char *last_name;
	char *nickname;
	char *email;
	int born;
	int gender;
	char *city;
} /* GG_DEPRECATED */;

struct gg_change_info_request *gg_change_info_request_new(const char *first_name, const char *last_name, const char *nickname, const char *email, int born, int gender, const char *city) GG_DEPRECATED;
void gg_change_info_request_free(struct gg_change_info_request *r) GG_DEPRECATED;

struct gg_http *gg_change_info(uin_t uin, const char *passwd, const struct gg_change_info_request *request, int async) GG_DEPRECATED;
#define gg_change_pubdir_watch_fd gg_pubdir_watch_fd
#define gg_change_pubdir_free gg_pubdir_free
#define gg_free_change_pubdir gg_pubdir_free

struct gg_http *gg_userlist_get(uin_t uin, const char *password, int async) GG_DEPRECATED;
int gg_userlist_get_watch_fd(struct gg_http *f) GG_DEPRECATED;
void gg_userlist_get_free(struct gg_http *f) GG_DEPRECATED;

struct gg_http *gg_userlist_put(uin_t uin, const char *password, const char *contacts, int async) GG_DEPRECATED;
int gg_userlist_put_watch_fd(struct gg_http *f) GG_DEPRECATED;
void gg_userlist_put_free(struct gg_http *f) GG_DEPRECATED;

struct gg_http *gg_userlist_remove(uin_t uin, const char *password, int async) GG_DEPRECATED;
int gg_userlist_remove_watch_fd(struct gg_http *f) GG_DEPRECATED;
void gg_userlist_remove_free(struct gg_http *f) GG_DEPRECATED;

int gg_pubdir50_handle_reply(struct gg_event *e, const char *packet, int length) GG_DEPRECATED;

/** \endcond */

int gg_file_hash_sha1(int fd, uint8_t *result) GG_DEPRECATED;

#undef printf
#ifdef __GNUC__
char *gg_saprintf(const char *format, ...) __attribute__ ((format (printf, 1, 2))) GG_DEPRECATED;
#else
char *gg_saprintf(const char *format, ...) GG_DEPRECATED;
#endif

char *gg_vsaprintf(const char *format, va_list ap) GG_DEPRECATED;

#define gg_alloc_sprintf gg_saprintf

char *gg_get_line(char **ptr) GG_DEPRECATED;

int gg_connect(void *addr, int port, int async) GG_DEPRECATED;
struct in_addr *gg_gethostbyname(const char *hostname) GG_DEPRECATED;
char *gg_read_line(int sock, char *buf, int length) GG_DEPRECATED;
void gg_chomp(char *line) GG_DEPRECATED;
char *gg_urlencode(const char *str) GG_DEPRECATED;
int gg_http_hash(const char *format, ...) GG_DEPRECATED;
void gg_http_free_fields(struct gg_http *h) GG_DEPRECATED;
int gg_read(struct gg_session *sess, char *buf, int length) GG_DEPRECATED;
int gg_write(struct gg_session *sess, const char *buf, int length) GG_DEPRECATED;
void *gg_recv_packet(struct gg_session *sess) GG_DEPRECATED;
int gg_send_packet(struct gg_session *sess, int type, ...) GG_DEPRECATED;
unsigned int gg_login_hash(const unsigned char *password, unsigned int seed) GG_DEPRECATED;
void gg_login_hash_sha1(const char *password, uint32_t seed, uint8_t *result) GG_DEPRECATED;
uint32_t gg_fix32(uint32_t x);
uint16_t gg_fix16(uint16_t x);
#define fix16 gg_fix16
#define fix32 gg_fix32
char *gg_proxy_auth(void) GG_DEPRECATED;
char *gg_base64_encode(const char *buf) GG_DEPRECATED;
char *gg_base64_decode(const char *buf) GG_DEPRECATED;
int gg_image_queue_remove(struct gg_session *s, struct gg_image_queue *q, int freeq) GG_DEPRECATED;

/**
 * Kolejka odbieranych obrazków.
 */
struct gg_image_queue {
	uin_t sender;			/**< Nadawca obrazka */
	uint32_t size;			/**< Rozmiar obrazka */
	uint32_t crc32;			/**< Suma kontrolna CRC32 */
	char *filename;			/**< Nazwa pliku */
	char *image;			/**< Bufor z odebranymi danymi */
	uint32_t done;			/**< Rozmiar odebranych danych */

	struct gg_image_queue *next;	/**< Kolejny element listy */
} GG_DEPRECATED;

int gg_dcc7_handle_id(struct gg_session *sess, struct gg_event *e, const void *payload, int len) GG_DEPRECATED;
int gg_dcc7_handle_new(struct gg_session *sess, struct gg_event *e, const void *payload, int len) GG_DEPRECATED;
int gg_dcc7_handle_info(struct gg_session *sess, struct gg_event *e, const void *payload, int len) GG_DEPRECATED;
int gg_dcc7_handle_accept(struct gg_session *sess, struct gg_event *e, const void *payload, int len) GG_DEPRECATED;
int gg_dcc7_handle_reject(struct gg_session *sess, struct gg_event *e, const void *payload, int len) GG_DEPRECATED;

#define GG_APPMSG_HOST "appmsg.gadu-gadu.pl"
#define GG_APPMSG_PORT 80
#define GG_PUBDIR_HOST "pubdir.gadu-gadu.pl"
#define GG_PUBDIR_PORT 80
#define GG_REGISTER_HOST "register.gadu-gadu.pl"
#define GG_REGISTER_PORT 80
#define GG_REMIND_HOST "retr.gadu-gadu.pl"
#define GG_REMIND_PORT 80
#define GG_RELAY_HOST "relay.gadu-gadu.pl"
#define GG_RELAY_PORT 80

#define GG_DEFAULT_PORT 8074
#define GG_HTTPS_PORT 443
#define GG_HTTP_USERAGENT "Mozilla/4.7 [en] (Win98; I)"

#define GG_DEFAULT_CLIENT_VERSION "10.1.0.11070"
#define GG_DEFAULT_PROTOCOL_VERSION 0x2e
#define GG_DEFAULT_TIMEOUT 30
#define GG_HAS_AUDIO_MASK 0x40000000
#define GG_HAS_AUDIO7_MASK 0x20000000
#define GG_ERA_OMNIX_MASK 0x04000000
#undef GG_LIBGADU_VERSION

#ifndef DOXYGEN

#define GG_FEATURE_MSG77		0x0001
#define GG_FEATURE_STATUS77		0x0002
#define GG_FEATURE_UNKNOWN_4		0x0004
#define GG_FEATURE_UNKNOWN_8		0x0008
#define GG_FEATURE_DND_FFC		0x0010
#define GG_FEATURE_IMAGE_DESCR		0x0020
#define GG_FEATURE_UNKNOWN_40		0x0040
#define GG_FEATURE_UNKNOWN_80		0x0080
#define GG_FEATURE_UNKNOWN_100		0x0100
#define GG_FEATURE_USER_DATA		0x0200
#define GG_FEATURE_MSG_ACK		0x0400
#define GG_FEATURE_UNKNOWN_800		0x0800
#define GG_FEATURE_UNKNOWN_1000		0x1000
#define GG_FEATURE_TYPING_NOTIFICATION	0x2000
#define GG_FEATURE_MULTILOGON		0x4000

/* Poniższe makra zostały zachowane dla zgodności API */
#define GG_FEATURE_MSG80		0
#define GG_FEATURE_STATUS80		0
#define GG_FEATURE_STATUS80BETA		0

#define GG_FEATURE_ALL			(GG_FEATURE_MSG80 | GG_FEATURE_STATUS80 | GG_FEATURE_DND_FFC | GG_FEATURE_IMAGE_DESCR | GG_FEATURE_UNKNOWN_100 | GG_FEATURE_USER_DATA | GG_FEATURE_MSG_ACK | GG_FEATURE_TYPING_NOTIFICATION)

#else

/** 
 * \ingroup login
 *
 * Flagi opcji protokołu.
 */
enum {
	GG_FEATURE_MSG77,	/**< Klient życzy sobie otrzymywać wiadomości zgodnie z protokołem 7.7 */
	GG_FEATURE_STATUS77,	/**< Klient życzy sobie otrzymywać zmiany stanu zgodnie z protokołem 7.7 */
	GG_FEATURE_DND_FFC,	/**< Klient obsługuje statusy "nie przeszkadzać" i "poGGadaj ze mną" */
	GG_FEATURE_IMAGE_DESCR,	/**< Klient obsługuje opisy graficzne oraz flagę \c GG_STATUS80_DESCR_MASK */
};


#endif

#define GG_DEFAULT_DCC_PORT 1550

struct gg_header {
	uint32_t type;			/* typ pakietu */
	uint32_t length;		/* długość reszty pakietu */
} GG_PACKED;

#define GG_WELCOME 0x0001
#define GG_NEED_EMAIL 0x0014

struct gg_welcome {
	uint32_t key;			/* klucz szyfrowania hasła */
} GG_PACKED;

#define GG_LOGIN 0x000c

struct gg_login {
	uint32_t uin;			/* mój numerek */
	uint32_t hash;			/* hash hasła */
	uint32_t status;		/* status na dzień dobry */
	uint32_t version;		/* moja wersja klienta */
	uint32_t local_ip;		/* mój adres ip */
	uint16_t local_port;		/* port, na którym słucham */
} GG_PACKED;

#define GG_LOGIN_EXT 0x0013

struct gg_login_ext {
	uint32_t uin;			/* mój numerek */
	uint32_t hash;			/* hash hasła */
	uint32_t status;		/* status na dzień dobry */
	uint32_t version;		/* moja wersja klienta */
	uint32_t local_ip;		/* mój adres ip */
	uint16_t local_port;		/* port, na którym słucham */
	uint32_t external_ip;		/* zewnętrzny adres ip */
	uint16_t external_port;		/* zewnętrzny port */
} GG_PACKED;

#define GG_LOGIN60 0x0015

struct gg_login60 {
	uint32_t uin;			/* mój numerek */
	uint32_t hash;			/* hash hasła */
	uint32_t status;		/* status na dzień dobry */
	uint32_t version;		/* moja wersja klienta */
	uint8_t dunno1;			/* 0x00 */
	uint32_t local_ip;		/* mój adres ip */
	uint16_t local_port;		/* port, na którym słucham */
	uint32_t external_ip;		/* zewnętrzny adres ip */
	uint16_t external_port;		/* zewnętrzny port */
	uint8_t image_size;		/* maksymalny rozmiar grafiki w KiB */
	uint8_t dunno2;			/* 0xbe */
} GG_PACKED;

#define GG_LOGIN70 0x0019

struct gg_login70 {
	uint32_t uin;			/* mój numerek */
	uint8_t hash_type;		/* rodzaj hashowania hasła */
	uint8_t hash[64];		/* hash hasła dopełniony zerami */
	uint32_t status;		/* status na dzień dobry */
	uint32_t version;		/* moja wersja klienta */
	uint8_t dunno1;			/* 0x00 */
	uint32_t local_ip;		/* mój adres ip */
	uint16_t local_port;		/* port, na którym słucham */
	uint32_t external_ip;		/* zewnętrzny adres ip (???) */
	uint16_t external_port;		/* zewnętrzny port (???) */
	uint8_t image_size;		/* maksymalny rozmiar grafiki w KiB */
	uint8_t dunno2;			/* 0xbe */
} GG_PACKED;

#define GG_LOGIN_OK 0x0003

#define GG_LOGIN_FAILED 0x0009

#define GG_PUBDIR50_REQUEST 0x0014

struct gg_pubdir50_request {
	uint8_t type;			/* GG_PUBDIR50_* */
	uint32_t seq;			/* czas wysłania zapytania */
} GG_PACKED;

#define GG_PUBDIR50_REPLY 0x000e

struct gg_pubdir50_reply {
	uint8_t type;			/* GG_PUBDIR50_* */
	uint32_t seq;			/* czas wysłania zapytania */
} GG_PACKED;

#define GG_NEW_STATUS 0x0002

#ifndef DOXYGEN

#define GG_STATUS_NOT_AVAIL 0x0001
#define GG_STATUS_NOT_AVAIL_DESCR 0x0015
#define GG_STATUS_FFC 0x0017
#define GG_STATUS_FFC_DESCR 0x0018
#define GG_STATUS_AVAIL 0x0002
#define GG_STATUS_AVAIL_DESCR 0x0004
#define GG_STATUS_BUSY 0x0003
#define GG_STATUS_BUSY_DESCR 0x0005
#define GG_STATUS_DND 0x0021
#define GG_STATUS_DND_DESCR 0x0022
#define GG_STATUS_INVISIBLE 0x0014
#define GG_STATUS_INVISIBLE_DESCR 0x0016
#define GG_STATUS_BLOCKED 0x0006

#define GG_STATUS_IMAGE_MASK 0x0100
#define GG_STATUS_DESCR_MASK 0x4000
#define GG_STATUS_FRIENDS_MASK 0x8000

#define GG_STATUS_FLAG_UNKNOWN 0x00000001
#define GG_STATUS_FLAG_VIDEO 0x00000002
#define GG_STATUS_FLAG_MOBILE 0x00100000
#define GG_STATUS_FLAG_SPAM 0x00800000

#else

/**
 * Rodzaje statusów użytkownika.
 *
 * \ingroup status
 */
enum {
	GG_STATUS_NOT_AVAIL,		/**< Niedostępny */
	GG_STATUS_NOT_AVAIL_DESCR,	/**< Niedostępny z opisem */
	GG_STATUS_FFC,			/**< PoGGadaj ze mną */
	GG_STATUS_FFC_DESCR,		/**< PoGGadaj ze mną z opisem */
	GG_STATUS_AVAIL,		/**< Dostępny */
	GG_STATUS_AVAIL_DESCR,		/**< Dostępny z opisem */
	GG_STATUS_BUSY,			/**< Zajęty */
	GG_STATUS_BUSY_DESCR,		/**< Zajęty z opisem */
	GG_STATUS_DND,			/**< Nie przeszkadzać */
	GG_STATUS_DND_DESCR,		/**< Nie przeszakdzać z opisem */
	GG_STATUS_INVISIBLE,		/**< Niewidoczny (tylko własny status) */
	GG_STATUS_INVISIBLE_DESCR,	/**< Niewidoczny z opisem (tylko własny status) */
	GG_STATUS_BLOCKED,		/**< Zablokowany (tylko status innych) */
	GG_STATUS_IMAGE_MASK,		/**< Flaga bitowa oznaczająca opis graficzny (tylko jeśli wybrano \c GG_FEATURE_IMAGE_DESCR) */
	GG_STATUS_DESCR_MASK,		/**< Flaga bitowa oznaczająca status z opisem (tylko jeśli wybrano \c GG_FEATURE_IMAGE_DESCR) */
	GG_STATUS_FRIENDS_MASK,		/**< Flaga bitowa dostępności tylko dla znajomych */
};

/**
 * Rodzaje statusów użytkownika. Mapa bitowa.
 *
 * \ingroup status
 */
enum {
	GG_STATUS_FLAG_UNKNOWN,		/**< Przeznaczenie nieznane, ale występuje zawsze */
	GG_STATUS_FLAG_VIDEO,		/**< Klient obsługuje wideorozmowy */
	GG_STATUS_FLAG_MOBILE,		/**< Klient mobilny (ikona telefonu komórkowego) */
	GG_STATUS_FLAG_SPAM,		/**< Klient chce otrzymywać linki od nieznajomych */
};

#endif	/* DOXYGEN */

/**
 * \ingroup status
 *
 * Flaga bitowa dostepnosci informujaca ze mozemy voipowac
 */

#define GG_STATUS_VOICE_MASK 0x20000	/**< czy ma wlaczone audio (7.7) */

/**
 * \ingroup status
 *
 * Maksymalna długośc opisu.
 */
#define GG_STATUS_DESCR_MAXSIZE 255
#define GG_STATUS_DESCR_MAXSIZE_PRE_8_0 70

#define GG_STATUS_MASK 0xff

/* GG_S_F() tryb tylko dla znajomych */
#define GG_S_F(x) (((x) & GG_STATUS_FRIENDS_MASK) != 0)

/* GG_S() stan bez uwzględnienia dodatkowych flag */
#define GG_S(x) ((x) & GG_STATUS_MASK)


/* GG_S_FF() chętny do rozmowy */
#define GG_S_FF(x) (GG_S(x) == GG_STATUS_FFC || GG_S(x) == GG_STATUS_FFC_DESCR)

/* GG_S_AV() dostępny */
#define GG_S_AV(x) (GG_S(x) == GG_STATUS_AVAIL || GG_S(x) == GG_STATUS_AVAIL_DESCR)

/* GG_S_AW() zaraz wracam */
#define GG_S_AW(x) (GG_S(x) == GG_STATUS_BUSY || GG_S(x) == GG_STATUS_BUSY_DESCR)

/* GG_S_DD() nie przeszkadzać */
#define GG_S_DD(x) (GG_S(x) == GG_STATUS_DND || GG_S(x) == GG_STATUS_DND_DESCR)

/* GG_S_NA() niedostępny */
#define GG_S_NA(x) (GG_S(x) == GG_STATUS_NOT_AVAIL || GG_S(x) == GG_STATUS_NOT_AVAIL_DESCR)

/* GG_S_I() niewidoczny */
#define GG_S_I(x) (GG_S(x) == GG_STATUS_INVISIBLE || GG_S(x) == GG_STATUS_INVISIBLE_DESCR)


/* GG_S_A() dostępny lub chętny do rozmowy */
#define GG_S_A(x) (GG_S_FF(x) || GG_S_AV(x))

/* GG_S_B() zajęty lub nie przeszkadzać */
#define GG_S_B(x) (GG_S_AW(x) || GG_S_DD(x))


/* GG_S_D() stan opisowy */
#define GG_S_D(x) (GG_S(x) == GG_STATUS_NOT_AVAIL_DESCR || \
		   GG_S(x) == GG_STATUS_FFC_DESCR || \
		   GG_S(x) == GG_STATUS_AVAIL_DESCR || \
		   GG_S(x) == GG_STATUS_BUSY_DESCR || \
		   GG_S(x) == GG_STATUS_DND_DESCR || \
		   GG_S(x) == GG_STATUS_INVISIBLE_DESCR)

/* GG_S_BL() blokowany lub blokujący */
#define GG_S_BL(x) (GG_S(x) == GG_STATUS_BLOCKED)

/**
 * Zmiana statusu (pakiet \c GG_NEW_STATUS i \c GG_NEW_STATUS80BETA)
 */
struct gg_new_status {
	uint32_t status;			/**< Nowy status */
} GG_PACKED;

#define GG_NOTIFY_FIRST 0x000f
#define GG_NOTIFY_LAST 0x0010

#define GG_NOTIFY 0x0010

struct gg_notify {
	uint32_t uin;				/* numerek danej osoby */
	uint8_t dunno1;				/* rodzaj wpisu w liście */
} GG_PACKED;

#ifndef DOXYGEN

#define GG_USER_OFFLINE 0x01
#define GG_USER_NORMAL 0x03
#define GG_USER_BLOCKED 0x04

#else

/**
 * \ingroup contacts
 *
 * Rodzaj kontaktu.
 */
enum {
	GG_USER_NORMAL,		/**< Zwykły kontakt */
	GG_USER_BLOCKED,	/**< Zablokowany */
	GG_USER_OFFLINE,	/**< Niewidoczny dla kontaktu */
};

#endif	/* DOXYGEN */

#define GG_LIST_EMPTY 0x0012

#define GG_NOTIFY_REPLY 0x000c	/* tak, to samo co GG_LOGIN */

struct gg_notify_reply {
	uint32_t uin;			/* numerek */
	uint32_t status;		/* status danej osoby */
	uint32_t remote_ip;		/* adres ip delikwenta */
	uint16_t remote_port;		/* port, na którym słucha klient */
	uint32_t version;		/* wersja klienta */
	uint16_t dunno2;		/* znowu port? */
} GG_PACKED;

#define GG_NOTIFY_REPLY60 0x0011

struct gg_notify_reply60 {
	uint32_t uin;			/* numerek plus flagi w MSB */
	uint8_t status;			/* status danej osoby */
	uint32_t remote_ip;		/* adres ip delikwenta */
	uint16_t remote_port;		/* port, na którym słucha klient */
	uint8_t version;		/* wersja klienta */
	uint8_t image_size;		/* maksymalny rozmiar grafiki w KiB */
	uint8_t dunno1;			/* 0x00 */
} GG_PACKED;

#define GG_STATUS60 0x000f

struct gg_status60 {
	uint32_t uin;			/* numerek plus flagi w MSB */
	uint8_t status;			/* status danej osoby */
	uint32_t remote_ip;		/* adres ip delikwenta */
	uint16_t remote_port;		/* port, na którym słucha klient */
	uint8_t version;		/* wersja klienta */
	uint8_t image_size;		/* maksymalny rozmiar grafiki w KiB */
	uint8_t dunno1;			/* 0x00 */
} GG_PACKED;

#define GG_NOTIFY_REPLY77 0x0018

struct gg_notify_reply77 {
	uint32_t uin;			/* numerek plus flagi w MSB */
	uint8_t status;			/* status danej osoby */
	uint32_t remote_ip;		/* adres ip delikwenta */
	uint16_t remote_port;		/* port, na którym słucha klient */
	uint8_t version;		/* wersja klienta */
	uint8_t image_size;		/* maksymalny rozmiar grafiki w KiB */
	uint8_t dunno1;			/* 0x00 */
	uint32_t dunno2;		/* ? */
} GG_PACKED;

#define GG_STATUS77 0x0017

struct gg_status77 {
	uint32_t uin;			/* numerek plus flagi w MSB */
	uint8_t status;			/* status danej osoby */
	uint32_t remote_ip;		/* adres ip delikwenta */
	uint16_t remote_port;		/* port, na którym słucha klient */
	uint8_t version;		/* wersja klienta */
	uint8_t image_size;		/* maksymalny rozmiar grafiki w KiB */
	uint8_t dunno1;			/* 0x00 */
	uint32_t dunno2;		/* ? */
} GG_PACKED;

#define GG_ADD_NOTIFY 0x000d
#define GG_REMOVE_NOTIFY 0x000e

struct gg_add_remove {
	uint32_t uin;			/* numerek */
	uint8_t dunno1;			/* bitmapa */
} GG_PACKED;

#define GG_STATUS 0x0002

struct gg_status {
	uint32_t uin;			/* numerek */
	uint32_t status;		/* nowy stan */
} GG_PACKED;

#define GG_SEND_MSG 0x000b

#ifndef DOXYGEN

#define GG_CLASS_QUEUED 0x0001
#define GG_CLASS_OFFLINE GG_CLASS_QUEUED
#define GG_CLASS_MSG 0x0004
#define GG_CLASS_CHAT 0x0008
#define GG_CLASS_CTCP 0x0010
#define GG_CLASS_ACK 0x0020
#define GG_CLASS_EXT GG_CLASS_ACK	/**< Dla kompatybilności wstecz */

#else

/**
 * Klasy wiadomości. Wartości są maskami bitowymi, które w większości
 * przypadków można łączyć (połączenie \c GG_CLASS_MSG i \c GG_CLASS_CHAT
 * nie ma sensu).
 *
 * \ingroup messages
 */
enum {
	GG_CLASS_MSG,		/**< Wiadomość ma pojawić się w osobnym oknie */
	GG_CLASS_CHAT,		/**< Wiadomość ma pojawić się w oknie rozmowy */
	GG_CLASS_CTCP,		/**< Wiadomość przeznaczona dla klienta Gadu-Gadu */
	GG_CLASS_ACK,		/**< Klient nie życzy sobie potwierdzenia */
	GG_CLASS_QUEUED,	/**< Wiadomość zakolejkowana na serwerze (tylko przy odbieraniu) */
};

#endif	/* DOXYGEN */

/**
 * Maksymalna długość wiadomości.
 *
 * \ingroup messages
 */
#define GG_MSG_MAXSIZE 1989

struct gg_send_msg {
	uint32_t recipient;
	uint32_t seq;
	uint32_t msgclass;
} GG_PACKED;

struct gg_msg_richtext {
	uint8_t flag;
	uint16_t length;
} GG_PACKED;

/**
 * Struktura opisująca formatowanie tekstu. W zależności od wartości pola
 * \c font, zaraz za tą strukturą może wystąpić \c gg_msg_richtext_color
 * lub \c gg_msg_richtext_image.
 *
 * \ingroup messages
 */
struct gg_msg_richtext_format {
	uint16_t position;	/**< Początkowy znak formatowania (liczony od 0) */
	uint8_t font;		/**< Atrybuty formatowania */
} GG_PACKED;

#ifndef DOXYGEN

#define GG_FONT_BOLD 0x01
#define GG_FONT_ITALIC 0x02
#define GG_FONT_UNDERLINE 0x04
#define GG_FONT_COLOR 0x08
#define GG_FONT_IMAGE 0x80

#else

/**
 * Atrybuty formatowania wiadomości.
 *
 * \ingroup messages
 */
enum {
	GG_FONT_BOLD,
	GG_FONT_ITALIC,
	GG_FONT_UNDERLINE,
	GG_FONT_COLOR,
	GG_FONT_IMAGE
};

#endif	/* DOXYGEN */

/**
 * Struktura opisującą kolor tekstu dla atrybutu \c GG_FONT_COLOR.
 *
 * \ingroup messages
 */
struct gg_msg_richtext_color {
	uint8_t red;		/**< Składowa czerwona koloru */
	uint8_t green;		/**< Składowa zielona koloru */
	uint8_t blue;		/**< Składowa niebieska koloru */
} GG_PACKED;

/**
 * Strukturya opisująca obrazek wstawiony do wiadomości dla atrubutu
 * \c GG_FONT_IMAGE.
 *
 * \ingroup messages
 */
struct gg_msg_richtext_image {
	uint16_t unknown1;	/**< Nieznane pole o wartości 0x0109 */
	uint32_t size;		/**< Rozmiar obrazka */
	uint32_t crc32;		/**< Suma kontrolna CRC32 obrazka */
} GG_PACKED;

struct gg_msg_recipients {
	uint8_t flag;
	uint32_t count;
} GG_PACKED;

struct gg_msg_image_request {
	uint8_t flag;
	uint32_t size;
	uint32_t crc32;
} GG_PACKED;

struct gg_msg_image_reply {
	uint8_t flag;
	uint32_t size;
	uint32_t crc32;
	/* char filename[]; */
	/* char image[]; */
} GG_PACKED;

#define GG_SEND_MSG_ACK 0x0005

#ifndef DOXYGEN

#define GG_ACK_BLOCKED 0x0001
#define GG_ACK_DELIVERED 0x0002
#define GG_ACK_QUEUED 0x0003
#define GG_ACK_MBOXFULL 0x0004
#define GG_ACK_NOT_DELIVERED 0x0006

#else

/**
 * Status doręczenia wiadomości.
 *
 * \ingroup messages
 */
enum
{
	GG_ACK_DELIVERED,	/**< Wiadomość dostarczono. */
	GG_ACK_QUEUED,		/**< Wiadomość zakolejkowano z powodu niedostępności odbiorcy. */
	GG_ACK_BLOCKED,		/**< Wiadomość zablokowana przez serwer (spam, świąteczne ograniczenia itd.) */
	GG_ACK_MBOXFULL,	/**< Wiadomości nie dostarczono z powodu zapełnionej kolejki wiadomości odbiorcy. */
	GG_ACK_NOT_DELIVERED	/**< Wiadomości nie dostarczono (tylko dla \c GG_CLASS_CTCP). */
};

#endif	/* DOXYGEN */

struct gg_send_msg_ack {
	uint32_t status;
	uint32_t recipient;
	uint32_t seq;
} GG_PACKED;

#define GG_RECV_MSG 0x000a

struct gg_recv_msg {
	uint32_t sender;
	uint32_t seq;
	uint32_t time;
	uint32_t msgclass;
} GG_PACKED;

#define GG_PING 0x0008

#define GG_PONG 0x0007

#define GG_DISCONNECTING 0x000b

#define GG_USERLIST_REQUEST 0x0016

#define GG_XML_EVENT 0x0027

#ifndef DOXYGEN

#define GG_USERLIST_PUT 0x00
#define GG_USERLIST_PUT_MORE 0x01
#define GG_USERLIST_GET 0x02

#else

/**
 * \ingroup importexport
 *
 * Rodzaj zapytania.
 */
enum {
	GG_USERLIST_PUT,	/**< Eksport listy kontaktów. */
	GG_USERLIST_GET,	/**< Import listy kontaktów. */
};

#endif	/* DOXYGEN */

struct gg_userlist_request {
	uint8_t type;
} GG_PACKED;

#define GG_USERLIST_REPLY 0x0010

#ifndef DOXYGEN

#define GG_USERLIST_PUT_REPLY 0x00
#define GG_USERLIST_PUT_MORE_REPLY 0x02
#define GG_USERLIST_GET_REPLY 0x06
#define GG_USERLIST_GET_MORE_REPLY 0x04

#else

/**
 * \ingroup importexport
 *
 * Rodzaj odpowiedzi.
 */
enum {
	GG_USERLIST_PUT_REPLY,	/**< Wyeksportowano listy kontaktów. */
	GG_USERLIST_GET_REPLY,	/**< Zaimportowano listę kontaktów. */
};

#endif	/* DOXYGEN */

struct gg_userlist_reply {
	uint8_t type;
} GG_PACKED;

#ifndef DOXYGEN

#define GG_USERLIST100_PUT 0x00
#define GG_USERLIST100_GET 0x02

#else

/**
 * \ingroup importexport
 *
 * Rodzaj zapytania (10.0).
 */
enum {
	GG_USERLIST100_PUT,	/**< Eksport listy kontaktów. */
	GG_USERLIST100_GET,	/**< Import listy kontaktów. */
};

#endif	/* DOXYGEN */

#ifndef DOXYGEN

#define GG_USERLIST100_FORMAT_TYPE_NONE 0x00
#define GG_USERLIST100_FORMAT_TYPE_GG70 0x01
#define GG_USERLIST100_FORMAT_TYPE_GG100 0x02

#else

/**
 * \ingroup importexport
 *
 * Typ formatu listy kontaktów (10.0).
 */
enum {
	GG_USERLIST100_FORMAT_TYPE_NONE,	/**< Brak treści listy kontaktów. */
	GG_USERLIST100_FORMAT_TYPE_GG70,	/**< Format listy kontaktów zgodny z Gadu-Gadu 7.0. */
	GG_USERLIST100_FORMAT_TYPE_GG100,	/**< Format listy kontaktów zgodny z Gadu-Gadu 10.0. */
};

#endif	/* DOXYGEN */

#ifndef DOXYGEN

#define GG_USERLIST100_REPLY_LIST 0x00
#define GG_USERLIST100_REPLY_ACK 0x10
#define GG_USERLIST100_REPLY_REJECT 0x12

#else

/**
 * \ingroup importexport
 *
 * Typ odpowiedzi listy kontaktów (10.0).
 */
enum {
	GG_USERLIST100_REPLY_LIST,	/**< W odpowiedzi znajduje się aktualna lista kontaktów na serwerze. */
	GG_USERLIST100_REPLY_ACK,	/**< Potwierdzenie odebrania nowej wersji listy kontaktów. W polu \c gg_userlist100_reply.version znajduje się numer nowej wersji listy kontaktów. */
	GG_USERLIST100_REPLY_REJECT,	/**< Odmowa przyjęcia nowej wersji listy kontaktów. W polu \c gg_userlist100_reply.version znajduje się numer wersji listy kontaktów aktualnie przechowywanej przez serwer. */
};

#endif /* DOXYGEN */

struct gg_dcc_tiny_packet {
	uint8_t type;		/* rodzaj pakietu */
} GG_PACKED;

struct gg_dcc_small_packet {
	uint32_t type;		/* rodzaj pakietu */
} GG_PACKED;

struct gg_dcc_big_packet {
	uint32_t type;		/* rodzaj pakietu */
	uint32_t dunno1;		/* niewiadoma */
	uint32_t dunno2;		/* niewiadoma */
} GG_PACKED;

/*
 * póki co, nie znamy dokładnie protokołu. nie wiemy, co czemu odpowiada.
 * nazwy są niepoważne i tymczasowe.
 */
#define GG_DCC_WANT_FILE 0x0003		/* peer chce plik */
#define GG_DCC_HAVE_FILE 0x0001		/* więc mu damy */
#define GG_DCC_HAVE_FILEINFO 0x0003	/* niech ma informacje o pliku */
#define GG_DCC_GIMME_FILE 0x0006	/* peer jest pewny */
#define GG_DCC_CATCH_FILE 0x0002	/* wysyłamy plik */

#define GG_DCC_FILEATTR_READONLY 0x0020

#define GG_DCC_TIMEOUT_SEND 1800	/* 30 minut */
#define GG_DCC_TIMEOUT_GET 1800		/* 30 minut */
#define GG_DCC_TIMEOUT_FILE_ACK 300	/* 5 minut */
#define GG_DCC_TIMEOUT_VOICE_ACK 300	/* 5 minut */

#define GG_DCC7_INFO 0x1f

struct gg_dcc7_info {
	uint32_t uin;			/* numer nadawcy */
	uint32_t type;			/* sposób połączenia */
	gg_dcc7_id_t id;		/* identyfikator połączenia */
	char info[GG_DCC7_INFO_LEN];	/* informacje o połączeniu "ip port" */
	char hash[GG_DCC7_INFO_HASH_LEN];/* skrót "ip" */
} GG_PACKED;

#define GG_DCC7_NEW 0x20

struct gg_dcc7_new {
	gg_dcc7_id_t id;		/* identyfikator połączenia */
	uint32_t uin_from;		/* numer nadawcy */
	uint32_t uin_to;		/* numer odbiorcy */
	uint32_t type;			/* rodzaj transmisji */
	unsigned char filename[GG_DCC7_FILENAME_LEN];	/* nazwa pliku */
	uint32_t size;			/* rozmiar pliku */
	uint32_t size_hi;		/* rozmiar pliku (starsze bajty) */
	unsigned char hash[GG_DCC7_HASH_LEN];	/* hash SHA1 */
} GG_PACKED;

#define GG_DCC7_ACCEPT 0x21

struct gg_dcc7_accept {
	uint32_t uin;			/* numer przyjmującego połączenie */
	gg_dcc7_id_t id;		/* identyfikator połączenia */
	uint32_t offset;		/* offset przy wznawianiu transmisji */
	uint32_t dunno1;		/* 0x00000000 */
} GG_PACKED;

// XXX API
#define GG_DCC7_TYPE_P2P 0x00000001	/**< Połączenie bezpośrednie */
#define GG_DCC7_TYPE_SERVER 0x00000002	/**< Połączenie przez serwer */

#define GG_DCC7_REJECT 0x22

struct gg_dcc7_reject {
	uint32_t uin;			/**< Numer odrzucającego połączenie */
	gg_dcc7_id_t id;		/**< Identyfikator połączenia */
	uint32_t reason;		/**< Powód rozłączenia */
} GG_PACKED;

// XXX API
#define GG_DCC7_REJECT_BUSY 0x00000001	/**< Połączenie bezpośrednie już trwa, nie umiem obsłużyć więcej */
#define GG_DCC7_REJECT_USER 0x00000002	/**< Użytkownik odrzucił połączenie */
#define GG_DCC7_REJECT_VERSION 0x00000006	/**< Druga strona ma wersję klienta nieobsługującą połączeń bezpośrednich tego typu */

#define GG_DCC7_ID_REQUEST 0x23

struct gg_dcc7_id_request {
	uint32_t type;			/**< Rodzaj tranmisji */
} GG_PACKED;

// XXX API
#define GG_DCC7_TYPE_VOICE 0x00000001	/**< Transmisja głosu */
#define GG_DCC7_TYPE_FILE 0x00000004	/**< transmisja pliku */

#define GG_DCC7_ID_REPLY 0x23

struct gg_dcc7_id_reply {
	uint32_t type;			/** Rodzaj transmisji */
	gg_dcc7_id_t id;		/** Przyznany identyfikator */
} GG_PACKED;

#define GG_DCC7_DUNNO1 0x24

struct gg_dcc7_dunno1 {
	// XXX
} GG_PACKED;

#define GG_DCC7_TIMEOUT_CONNECT 10	/* 10 sekund */
#define GG_DCC7_TIMEOUT_SEND 1800	/* 30 minut */
#define GG_DCC7_TIMEOUT_GET 1800	/* 30 minut */
#define GG_DCC7_TIMEOUT_FILE_ACK 300	/* 5 minut */
#define GG_DCC7_TIMEOUT_VOICE_ACK 300	/* 5 minut */

#ifdef _WIN32
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __GG_LIBGADU_H */

/*
 * Local variables:
 * c-indentation-style: k&r
 * c-basic-offset: 8
 * indent-tabs-mode: notnil
 * End:
 *
 * vim: shiftwidth=8:
 */
