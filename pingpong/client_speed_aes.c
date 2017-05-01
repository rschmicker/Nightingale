//SSL-Client.c
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "mysecond.h"
#include <stdlib.h>
#include <sys/time.h>

#define FAIL    -1


double mysecond()
{
    struct timeval tp;
    gettimeofday(&tp, (void *)NULL);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

    //Added the LoadCertificates how in the server-side makes.    
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
 /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

int OpenConnection(const char *hostname, int port)
{   int sd;
    struct hostent *host;
    struct sockaddr_in addr;

    if ( (host = gethostbyname(hostname)) == NULL )
    {
        perror(hostname);
        abort();
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}

SSL_CTX* InitCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = SSLv23_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */

    /* Set the cipher to use */
    if (SSL_CTX_set_cipher_list(ctx, "ECDHE-RSA-AES256-SHA384") <= 0) {
        printf("Error setting the cipher list.\n");
        exit(0);
    }

    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("No certificates.\n");
}

int main(int argc, char **argv)
{   SSL_CTX *ctx;
    int server;
    SSL *ssl;
    size_t length = 1024 * 1024 * 1024;
    unsigned char *buf = calloc(sizeof(unsigned char), length);
    int bytes;
    char hostname[]="127.0.0.1";
    char* portnum=argv[1];
    char CertFile[] = "cert.pem";
    char KeyFile[] = "key.pem";

    SSL_library_init();

    ctx = InitCTX();
    LoadCertificates(ctx, CertFile, KeyFile);
    server = OpenConnection(hostname, atoi(portnum));
    ssl = SSL_new(ctx);      /* create new SSL connection state */
    SSL_set_fd(ssl, server);    /* attach the socket descriptor */
    if ( SSL_connect(ssl) == FAIL )   /* perform the connection */
        ERR_print_errors_fp(stderr);
    else
    {   char *msg = "Hello???";

        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl);        /* get any certs */
        SSL_write(ssl, msg, strlen(msg));   /* encrypt & send message */
        printf("Receiving...\n");
	double t1;
	
	int current_length = 0;
	int counter = 0;
	int chunk_size = 16 * 1024;

	t1 = mysecond();
	while(current_length < length){
	    bytes = SSL_read(ssl, (buf + (counter * chunk_size)), chunk_size); /* get reply & decrypt */
	    int check = bytes;
	    if (check <= 0) {
		ERR_print_errors_fp(stderr);
		exit(0);
	    }
	    else {
		current_length += check;
		counter++;
	    }
	    
	}        
        t1 = mysecond() - t1;
	double rate = (((double)length)/1000000000.)/t1;
	buf[bytes] = 0;
        printf("Received \"%d\" bytes!\n", current_length);
	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    	printf("Transfer Time:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        SSL_free(ssl);        /* release connection state */
    }
    close(server);         /* close socket */
    SSL_CTX_free(ctx);        /* release context */
    return 0;
}
