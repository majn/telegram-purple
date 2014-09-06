/**
 * @file certificate.h Public-Key Certificate API
 * @ingroup core
 * @see @ref certificate-signals
 * @since 2.2.0
 */

/*
 *
 * purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#ifndef _PURPLE_CERTIFICATE_H
#define _PURPLE_CERTIFICATE_H

#include <time.h>

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum
{
	PURPLE_CERTIFICATE_INVALID = 0,
	PURPLE_CERTIFICATE_VALID = 1
} PurpleCertificateVerificationStatus;

typedef struct _PurpleCertificate PurpleCertificate;
typedef struct _PurpleCertificatePool PurpleCertificatePool;
typedef struct _PurpleCertificateScheme PurpleCertificateScheme;
typedef struct _PurpleCertificateVerifier PurpleCertificateVerifier;
typedef struct _PurpleCertificateVerificationRequest PurpleCertificateVerificationRequest;

/**
 * Callback function for the results of a verification check
 * @param st       Status code
 * @param userdata User-defined data
 */
typedef void (*PurpleCertificateVerifiedCallback)
		(PurpleCertificateVerificationStatus st,
		 gpointer userdata);

/** A certificate instance
 *
 *  An opaque data structure representing a single certificate under some
 *  CertificateScheme
 */
struct _PurpleCertificate
{
	/** Scheme this certificate is under */
	PurpleCertificateScheme * scheme;
	/** Opaque pointer to internal data */
	gpointer data;
};

/**
 * Database for retrieval or storage of Certificates
 *
 * More or less a hash table; all lookups and writes are controlled by a string
 * key.
 */
struct _PurpleCertificatePool
{
	/** Scheme this Pool operates for */
	gchar *scheme_name;
	/** Internal name to refer to the pool by */
	gchar *name;

	/** User-friendly name for this type
	 *  ex: N_("SSL Servers")
	 *  When this is displayed anywhere, it should be i18ned
	 *  ex: _(pool->fullname)
	 */
	gchar *fullname;

	/** Internal pool data */
	gpointer data;

	/**
	 * Set up the Pool's internal state
	 *
	 * Upon calling purple_certificate_register_pool() , this function will
	 * be called. May be NULL.
	 * @return TRUE if the initialization succeeded, otherwise FALSE
	 */
	gboolean (* init)(void);

	/**
	 * Uninit the Pool's internal state
	 *
	 * Will be called by purple_certificate_unregister_pool() . May be NULL
	 */
	void (* uninit)(void);

	/** Check for presence of a certificate in the pool using unique ID */
	gboolean (* cert_in_pool)(const gchar *id);
	/** Retrieve a PurpleCertificate from the pool */
	PurpleCertificate * (* get_cert)(const gchar *id);
	/** Add a certificate to the pool. Must overwrite any other
	 *  certificates sharing the same ID in the pool.
	 *  @return TRUE if the operation succeeded, otherwise FALSE
	 */
	gboolean (* put_cert)(const gchar *id, PurpleCertificate *crt);
	/** Delete a certificate from the pool */
	gboolean (* delete_cert)(const gchar *id);

	/** Returns a list of IDs stored in the pool */
	GList * (* get_idlist)(void);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

/** A certificate type
 *
 *  A CertificateScheme must implement all of the fields in the structure,
 *  and register it using purple_certificate_register_scheme()
 *
 *  There may be only ONE CertificateScheme provided for each certificate
 *  type, as specified by the "name" field.
 */
struct _PurpleCertificateScheme
{
	/** Name of the certificate type
	 *  ex: "x509", "pgp", etc.
	 *  This must be globally unique - you may not register more than one
	 *  CertificateScheme of the same name at a time.
	 */
	gchar * name;

	/** User-friendly name for this type
	 *  ex: N_("X.509 Certificates")
	 *  When this is displayed anywhere, it should be i18ned
	 *  ex: _(scheme->fullname)
	 */
	gchar * fullname;

	/** Imports a certificate from a file
	 *
	 *  @param filename   File to import the certificate from
	 *  @return           Pointer to the newly allocated Certificate struct
	 *                    or NULL on failure.
	 */
	PurpleCertificate * (* import_certificate)(const gchar * filename);

	/**
	 * Exports a certificate to a file
	 *
	 * @param filename    File to export the certificate to
	 * @param crt         Certificate to export
	 * @return TRUE if the export succeeded, otherwise FALSE
	 * @see purple_certificate_export()
	 */
	gboolean (* export_certificate)(const gchar *filename, PurpleCertificate *crt);

	/**
	 * Duplicates a certificate
	 *
	 * Certificates are generally assumed to be read-only, so feel free to
	 * do any sort of reference-counting magic you want here. If this ever
	 * changes, please remember to change the magic accordingly.
	 * @return Reference to the new copy
	 */
	PurpleCertificate * (* copy_certificate)(PurpleCertificate *crt);

	/** Destroys and frees a Certificate structure
	 *
	 *  Destroys a Certificate's internal data structures and calls
	 *  free(crt)
	 *
	 *  @param crt  Certificate instance to be destroyed. It WILL NOT be
	 *              destroyed if it is not of the correct
	 *              CertificateScheme. Can be NULL
	 */
	void (* destroy_certificate)(PurpleCertificate * crt);

	/** Find whether "crt" has a valid signature from issuer "issuer"
	 *  @see purple_certificate_signed_by() */
	gboolean (*signed_by)(PurpleCertificate *crt, PurpleCertificate *issuer);
	/**
	 * Retrieves the certificate public key fingerprint using SHA1
	 *
	 * @param crt   Certificate instance
	 * @return Binary representation of SHA1 hash - must be freed using
	 *         g_byte_array_free()
	 */
	GByteArray * (* get_fingerprint_sha1)(PurpleCertificate *crt);

	/**
	 * Retrieves a unique certificate identifier
	 *
	 * @param crt   Certificate instance
	 * @return Newly allocated string that can be used to uniquely
	 *         identify the certificate.
	 */
	gchar * (* get_unique_id)(PurpleCertificate *crt);

	/**
	 * Retrieves a unique identifier for the certificate's issuer
	 *
	 * @param crt   Certificate instance
	 * @return Newly allocated string that can be used to uniquely
	 *         identify the issuer's certificate.
	 */
	gchar * (* get_issuer_unique_id)(PurpleCertificate *crt);

	/**
	 * Gets the certificate subject's name
	 *
	 * For X.509, this is the "Common Name" field, as we're only using it
	 * for hostname verification at the moment
	 *
	 * @see purple_certificate_get_subject_name()
	 *
	 * @param crt   Certificate instance
	 * @return Newly allocated string with the certificate subject.
	 */
	gchar * (* get_subject_name)(PurpleCertificate *crt);

	/**
	 * Check the subject name against that on the certificate
	 * @see purple_certificate_check_subject_name()
	 * @return TRUE if it is a match, else FALSE
	 */
	gboolean (* check_subject_name)(PurpleCertificate *crt, const gchar *name);

	/** Retrieve the certificate activation/expiration times */
	gboolean (* get_times)(PurpleCertificate *crt, time_t *activation, time_t *expiration);

	/** Imports certificates from a file
	 *
	 *  @param filename   File to import the certificates from
	 *  @return           GSList of pointers to the newly allocated Certificate structs
	 *                    or NULL on failure.
	 */
	GSList * (* import_certificates)(const gchar * filename);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
};

/** A set of operations used to provide logic for verifying a Certificate's
 *  authenticity.
 *
 * A Verifier provider must fill out these fields, then register it using
 * purple_certificate_register_verifier()
 *
 * The (scheme_name, name) value must be unique for each Verifier - you may not
 * register more than one Verifier of the same name for each Scheme
 */
struct _PurpleCertificateVerifier
{
	/** Name of the scheme this Verifier operates on
	 *
	 * The scheme will be looked up by name when a Request is generated
	 * using this Verifier
	 */
	gchar *scheme_name;

	/** Name of the Verifier - case insensitive */
	gchar *name;

	/**
	 * Start the verification process
	 *
	 * To be called from purple_certificate_verify once it has
	 * constructed the request. This will use the information in the
	 * given VerificationRequest to check the certificate and callback
	 * the requester with the verification results.
	 *
	 * @param vrq      Request to process
	 */
	void (* start_verification)(PurpleCertificateVerificationRequest *vrq);

	/**
	 * Destroy a completed Request under this Verifier
	 * The function pointed to here is only responsible for cleaning up
	 * whatever PurpleCertificateVerificationRequest::data points to.
	 * It should not call free(vrq)
	 *
	 * @param vrq       Request to destroy
	 */
	void (* destroy_request)(PurpleCertificateVerificationRequest *vrq);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

/** Structure for a single certificate request
 *
 *  Useful for keeping track of the state of a verification that involves
 *  several steps
 */
struct _PurpleCertificateVerificationRequest
{
	/** Reference to the verification logic used */
	PurpleCertificateVerifier *verifier;
	/** Reference to the scheme used.
	 *
	 * This is looked up from the Verifier when the Request is generated
	 */
	PurpleCertificateScheme *scheme;

	/**
	 * Name to check that the certificate is issued to
	 *
	 * For X.509 certificates, this is the Common Name
	 */
	gchar *subject_name;

	/** List of certificates in the chain to be verified (such as that returned by purple_ssl_get_peer_certificates )
	 *
	 * This is most relevant for X.509 certificates used in SSL sessions.
	 * The list order should be: certificate, issuer, issuer's issuer, etc.
	 */
	GList *cert_chain;

	/** Internal data used by the Verifier code */
	gpointer data;

	/** Function to call with the verification result */
	PurpleCertificateVerifiedCallback cb;
	/** Data to pass to the post-verification callback */
	gpointer cb_data;
};

/*****************************************************************************/
/** @name Certificate Verification Functions                                 */
/*****************************************************************************/
/*@{*/

/**
 * Constructs a verification request and passed control to the specified Verifier
 *
 * It is possible that the callback will be called immediately upon calling
 * this function. Plan accordingly.
 *
 * @param verifier      Verification logic to use.
 *                      @see purple_certificate_find_verifier()
 *
 * @param subject_name  Name that should match the first certificate in the
 *                      chain for the certificate to be valid. Will be strdup'd
 *                      into the Request struct
 *
 * @param cert_chain    Certificate chain to check. If there is more than one
 *                      certificate in the chain (X.509), the peer's
 *                      certificate comes first, then the issuer/signer's
 *                      certificate, etc. The whole list is duplicated into the
 *                      Request struct.
 *
 * @param cb            Callback function to be called with whether the
 *                      certificate was approved or not.
 * @param cb_data       User-defined data for the above.
 */
void
purple_certificate_verify (PurpleCertificateVerifier *verifier,
			   const gchar *subject_name, GList *cert_chain,
			   PurpleCertificateVerifiedCallback cb,
			   gpointer cb_data);

/**
 * Completes and destroys a VerificationRequest
 *
 * @param vrq           Request to conclude
 * @param st            Success/failure code to pass to the request's
 *                      completion callback.
 */
void
purple_certificate_verify_complete(PurpleCertificateVerificationRequest *vrq,
				   PurpleCertificateVerificationStatus st);

/*@}*/

/*****************************************************************************/
/** @name Certificate Functions                                              */
/*****************************************************************************/
/*@{*/

/**
 * Makes a duplicate of a certificate
 *
 * @param crt        Instance to duplicate
 * @return Pointer to new instance
 */
PurpleCertificate *
purple_certificate_copy(PurpleCertificate *crt);

/**
 * Duplicates an entire list of certificates
 *
 * @param crt_list   List to duplicate
 * @return New list copy
 */
GList *
purple_certificate_copy_list(GList *crt_list);

/**
 * Destroys and free()'s a Certificate
 *
 * @param crt        Instance to destroy. May be NULL.
 */
void
purple_certificate_destroy (PurpleCertificate *crt);

/**
 * Destroy an entire list of Certificate instances and the containing list
 *
 * @param crt_list   List of certificates to destroy. May be NULL.
 */
void
purple_certificate_destroy_list (GList * crt_list);

/**
 * Check whether 'crt' has a valid signature made by 'issuer'
 *
 * @param crt        Certificate instance to check signature of
 * @param issuer     Certificate thought to have signed 'crt'
 *
 * @return TRUE if 'crt' has a valid signature made by 'issuer',
 *         otherwise FALSE
 * @todo Find a way to give the reason (bad signature, not the issuer, etc.)
 */
gboolean
purple_certificate_signed_by(PurpleCertificate *crt, PurpleCertificate *issuer);

/**
 * Check that a certificate chain is valid and, if not, the failing certificate.
 *
 * Uses purple_certificate_signed_by() to verify that each PurpleCertificate
 * in the chain carries a valid signature from the next. A single-certificate
 * chain is considered to be valid.
 *
 * @param chain      List of PurpleCertificate instances comprising the chain,
 *                   in the order certificate, issuer, issuer's issuer, etc.
 * @param failing    A pointer to a PurpleCertificate*. If not NULL, if the
 *                   chain fails to validate, this will be set to the
 *                   certificate whose signature could not be validated.
 * @return TRUE if the chain is valid. See description.
 *
 * @since 2.6.0
 * @deprecated  This function will become
 *              purple_certificate_check_signature_chain in 3.0.0
 */
gboolean
purple_certificate_check_signature_chain_with_failing(GList *chain,
		PurpleCertificate **failing);

/**
 * Check that a certificate chain is valid
 *
 * Uses purple_certificate_signed_by() to verify that each PurpleCertificate
 * in the chain carries a valid signature from the next. A single-certificate
 * chain is considered to be valid.
 *
 * @param chain      List of PurpleCertificate instances comprising the chain,
 *                   in the order certificate, issuer, issuer's issuer, etc.
 * @return TRUE if the chain is valid. See description.
 * @todo Specify which certificate in the chain caused a failure
 * @deprecated  This function will be removed in 3.0.0 and replaced with
 *              purple_certificate_check_signature_chain_with_failing
 */
gboolean
purple_certificate_check_signature_chain(GList *chain);

/**
 * Imports a PurpleCertificate from a file
 *
 * @param scheme      Scheme to import under
 * @param filename    File path to import from
 * @return Pointer to a new PurpleCertificate, or NULL on failure
 */
PurpleCertificate *
purple_certificate_import(PurpleCertificateScheme *scheme, const gchar *filename);

/**
 * Imports a list of PurpleCertificates from a file
 *
 * @param scheme      Scheme to import under
 * @param filename    File path to import from
 * @return Pointer to a GSList of new PurpleCertificates, or NULL on failure
 */
GSList *
purple_certificates_import(PurpleCertificateScheme *scheme, const gchar *filename);

/**
 * Exports a PurpleCertificate to a file
 *
 * @param filename    File to export the certificate to
 * @param crt         Certificate to export
 * @return TRUE if the export succeeded, otherwise FALSE
 */
gboolean
purple_certificate_export(const gchar *filename, PurpleCertificate *crt);


/**
 * Retrieves the certificate public key fingerprint using SHA1.
 *
 * @param crt        Certificate instance
 * @return Binary representation of the hash. You are responsible for free()ing
 *         this.
 * @see purple_base16_encode_chunked()
 */
GByteArray *
purple_certificate_get_fingerprint_sha1(PurpleCertificate *crt);

/**
 * Get a unique identifier for the certificate
 *
 * @param crt        Certificate instance
 * @return String representing the certificate uniquely. Must be g_free()'ed
 */
gchar *
purple_certificate_get_unique_id(PurpleCertificate *crt);

/**
 * Get a unique identifier for the certificate's issuer
 *
 * @param crt        Certificate instance
 * @return String representing the certificate's issuer uniquely. Must be
 *         g_free()'ed
 */
gchar *
purple_certificate_get_issuer_unique_id(PurpleCertificate *crt);

/**
 * Gets the certificate subject's name
 *
 * For X.509, this is the "Common Name" field, as we're only using it
 * for hostname verification at the moment
 *
 * @param crt   Certificate instance
 * @return Newly allocated string with the certificate subject.
 */
gchar *
purple_certificate_get_subject_name(PurpleCertificate *crt);

/**
 * Check the subject name against that on the certificate
 * @param crt   Certificate instance
 * @param name  Name to check.
 * @return TRUE if it is a match, else FALSE
 */
gboolean
purple_certificate_check_subject_name(PurpleCertificate *crt, const gchar *name);

/**
 * Get the expiration/activation times.
 *
 * @param crt          Certificate instance
 * @param activation   Reference to store the activation time at. May be NULL
 *                     if you don't actually want it.
 * @param expiration   Reference to store the expiration time at. May be NULL
 *                     if you don't actually want it.
 * @return TRUE if the requested values were obtained, otherwise FALSE.
 */
gboolean
purple_certificate_get_times(PurpleCertificate *crt, time_t *activation, time_t *expiration);

/*@}*/

/*****************************************************************************/
/** @name Certificate Pool Functions                                         */
/*****************************************************************************/
/*@{*/
/**
 * Helper function for generating file paths in ~/.purple/certificates for
 * CertificatePools that use them.
 *
 * All components will be escaped for filesystem friendliness.
 *
 * @param pool   CertificatePool to build a path for
 * @param id     Key to look up a Certificate by. May be NULL.
 * @return A newly allocated path of the form
 *         ~/.purple/certificates/scheme_name/pool_name/unique_id
 */
gchar *
purple_certificate_pool_mkpath(PurpleCertificatePool *pool, const gchar *id);

/**
 * Determines whether a pool can be used.
 *
 * Checks whether the associated CertificateScheme is loaded.
 *
 * @param pool   Pool to check
 *
 * @return TRUE if the pool can be used, otherwise FALSE
 */
gboolean
purple_certificate_pool_usable(PurpleCertificatePool *pool);

/**
 * Looks up the scheme the pool operates under
 *
 * @param pool   Pool to get the scheme of
 *
 * @return Pointer to the pool's scheme, or NULL if it isn't loaded.
 * @see purple_certificate_pool_usable()
 */
PurpleCertificateScheme *
purple_certificate_pool_get_scheme(PurpleCertificatePool *pool);

/**
 * Check for presence of an ID in a pool.
 * @param pool   Pool to look in
 * @param id     ID to look for
 * @return TRUE if the ID is in the pool, else FALSE
 */
gboolean
purple_certificate_pool_contains(PurpleCertificatePool *pool, const gchar *id);

/**
 * Retrieve a certificate from a pool.
 * @param pool   Pool to fish in
 * @param id     ID to look up
 * @return Retrieved certificate, or NULL if it wasn't there
 */
PurpleCertificate *
purple_certificate_pool_retrieve(PurpleCertificatePool *pool, const gchar *id);

/**
 * Add a certificate to a pool
 *
 * Any pre-existing certificate of the same ID will be overwritten.
 *
 * @param pool   Pool to add to
 * @param id     ID to store the certificate with
 * @param crt    Certificate to store
 * @return TRUE if the operation succeeded, otherwise FALSE
 */
gboolean
purple_certificate_pool_store(PurpleCertificatePool *pool, const gchar *id, PurpleCertificate *crt);

/**
 * Remove a certificate from a pool
 *
 * @param pool   Pool to remove from
 * @param id     ID to remove
 * @return TRUE if the operation succeeded, otherwise FALSE
 */
gboolean
purple_certificate_pool_delete(PurpleCertificatePool *pool, const gchar *id);

/**
 * Get the list of IDs currently in the pool.
 *
 * @param pool   Pool to enumerate
 * @return GList pointing to newly-allocated id strings. Free using
 *         purple_certificate_pool_destroy_idlist()
 */
GList *
purple_certificate_pool_get_idlist(PurpleCertificatePool *pool);

/**
 * Destroys the result given by purple_certificate_pool_get_idlist()
 *
 * @param idlist ID List to destroy
 */
void
purple_certificate_pool_destroy_idlist(GList *idlist);

/*@}*/

/*****************************************************************************/
/** @name Certificate Subsystem API                                          */
/*****************************************************************************/
/*@{*/

/**
 * Initialize the certificate system
 */
void
purple_certificate_init(void);

/**
 * Un-initialize the certificate system
 */
void
purple_certificate_uninit(void);

/**
 * Get the Certificate subsystem handle for signalling purposes
 */
gpointer
purple_certificate_get_handle(void);

/** Look up a registered CertificateScheme by name
 * @param name   The scheme name. Case insensitive.
 * @return Pointer to the located Scheme, or NULL if it isn't found.
 */
PurpleCertificateScheme *
purple_certificate_find_scheme(const gchar *name);

/**
 * Get all registered CertificateSchemes
 *
 * @return GList pointing to all registered CertificateSchemes . This value
 *         is owned by libpurple
 */
GList *
purple_certificate_get_schemes(void);

/** Register a CertificateScheme with libpurple
 *
 * No two schemes can be registered with the same name; this function enforces
 * that.
 *
 * @param scheme  Pointer to the scheme to register.
 * @return TRUE if the scheme was successfully added, otherwise FALSE
 */
gboolean
purple_certificate_register_scheme(PurpleCertificateScheme *scheme);

/** Unregister a CertificateScheme from libpurple
 *
 * @param scheme    Scheme to unregister.
 *                  If the scheme is not registered, this is a no-op.
 *
 * @return TRUE if the unregister completed successfully
 */
gboolean
purple_certificate_unregister_scheme(PurpleCertificateScheme *scheme);

/** Look up a registered PurpleCertificateVerifier by scheme and name
 * @param scheme_name  Scheme name. Case insensitive.
 * @param ver_name     The verifier name. Case insensitive.
 * @return Pointer to the located Verifier, or NULL if it isn't found.
 */
PurpleCertificateVerifier *
purple_certificate_find_verifier(const gchar *scheme_name, const gchar *ver_name);

/**
 * Get the list of registered CertificateVerifiers
 *
 * @return GList of all registered PurpleCertificateVerifier. This value
 *         is owned by libpurple
 */
GList *
purple_certificate_get_verifiers(void);

/**
 * Register a CertificateVerifier with libpurple
 *
 * @param vr     Verifier to register.
 * @return TRUE if register succeeded, otherwise FALSE
 */
gboolean
purple_certificate_register_verifier(PurpleCertificateVerifier *vr);

/**
 * Unregister a CertificateVerifier with libpurple
 *
 * @param vr     Verifier to unregister.
 * @return TRUE if unregister succeeded, otherwise FALSE
 */
gboolean
purple_certificate_unregister_verifier(PurpleCertificateVerifier *vr);

/** Look up a registered PurpleCertificatePool by scheme and name
 * @param scheme_name  Scheme name. Case insensitive.
 * @param pool_name    Pool name. Case insensitive.
 * @return Pointer to the located Pool, or NULL if it isn't found.
 */
PurpleCertificatePool *
purple_certificate_find_pool(const gchar *scheme_name, const gchar *pool_name);

/**
 * Get the list of registered Pools
 *
 * @return GList of all registered PurpleCertificatePool s. This value
 *         is owned by libpurple
 */
GList *
purple_certificate_get_pools(void);

/**
 * Register a CertificatePool with libpurple and call its init function
 *
 * @param pool   Pool to register.
 * @return TRUE if the register succeeded, otherwise FALSE
 */
gboolean
purple_certificate_register_pool(PurpleCertificatePool *pool);

/**
 * Unregister a CertificatePool with libpurple and call its uninit function
 *
 * @param pool   Pool to unregister.
 * @return TRUE if the unregister succeeded, otherwise FALSE
 */
gboolean
purple_certificate_unregister_pool(PurpleCertificatePool *pool);

/*@}*/


/**
 * Displays a window showing X.509 certificate information
 *
 * @param crt    Certificate under an "x509" Scheme
 * @todo Will break on CA certs, as they have no Common Name
 */
void
purple_certificate_display_x509(PurpleCertificate *crt);

/**
 * Add a search path for certificates.
 *
 * @param path   Path to search for certificates.
 */
void purple_certificate_add_ca_search_path(const char *path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PURPLE_CERTIFICATE_H */
