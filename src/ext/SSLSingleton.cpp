#include "ext/SSLSingleton.h"

#ifdef USE_OPENSSL

namespace smpl
{
    SSL_Singleton::SSL_Singleton()
    {
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        ERR_load_crypto_strings();
        ssl_ctx = SSL_CTX_new(TLS_method());
        SSL_CTX_set_mode(ssl_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
        SSL_CTX_set_options(ssl_ctx, SSL_OP_ENABLE_KTLS);
    }
    SSL_Singleton::~SSL_Singleton()
    {

    }

    SSL_CTX* SSL_Singleton::getCTX()
    {
        return ssl_ctx;
    }

    SSL_Singleton& SSL_Singleton::getSingleton()
    {
        return singleton;
    }
}

#endif