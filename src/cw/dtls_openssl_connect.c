#include <openssl/err.h>


#include "dtls_openssl.h"
#include "log.h"
#include "dbg.h"
#include "cw_util.h"
#include "timer.h"

/*
static BIO_METHOD bio_methods = {
	BIO_TYPE_DGRAM,
	"cw packet",
	dtls_openssl_bio_write,
	dtls_openssl_bio_read,
	dtls_openssl_bio_puts,
	NULL, 			// dgram_gets
	dtls_openssl_bio_ctrl,
	dtls_openssl_bio_new,
	dtls_openssl_bio_free,
	NULL,
};
*/

 unsigned int psk_client_cb(SSL * ssl,
				  const char *hint,
				  char *identity,
				  unsigned int max_identity_len,
				  unsigned char *psk, unsigned int max_psk_len)
{
	int l;
	BIO *b = SSL_get_rbio(ssl);
	/*struct conn *conn = b->ptr;*/
	struct conn * conn = BIO_get_data(b); /*b->ptr;*/

	snprintf(identity, max_identity_len, "CLient_identity");

	l = bstr16_len(conn->dtls_psk) < max_psk_len ? bstr16_len(conn->dtls_psk) : max_psk_len;
	memcpy(psk, bstr16_data(conn->dtls_psk), l);
	return l;

}


int 
dtls_openssl_connect(struct conn *conn)
{
	struct dtls_openssl_data *d;
	int rc;
	time_t timer;
	
	if (!conn->dtls_data)
		conn->dtls_data =
		    dtls_openssl_data_create(conn, DTLSv1_client_method(),
					     dtls_openssl_bio_method());

	d = (struct dtls_openssl_data *) conn->dtls_data;
	if (!d)
		return 0;

/*
	if (conn->dtls_psk)
		SSL_set_psk_client_callback(d->ssl, psk_client_cb);
*/


	errno =0;
	timer = cw_timer_start(10);
	do {
		rc = SSL_connect(d->ssl);
	}while(rc!=1 && errno==EAGAIN && !cw_timer_timeout(timer));


	if (rc == 1) {
		cw_dbg(DBG_DTLS,"SSL connect successfull!");
		conn->read = dtls_openssl_read;
		conn->write = dtls_openssl_write;
		return 1;
	}

	rc = dtls_openssl_log_error(d->ssl, rc, "DTLS connect");
	return 0;
}

