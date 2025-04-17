#pragma once

#ifdef USE_OPENSSL
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace smpl
{
    class SSL_Singleton
    {
    public:
        ~SSL_Singleton()
        {

        }

        SSL_CTX* getCTX()
        {
            return ssl_ctx;
        }

        static SSL_Singleton& getSingleton()
        {
            return singleton;
        }

    private:
        SSL_CTX* ssl_ctx = nullptr;
        SSL_Singleton()
        {
            SSL_load_error_strings();
            ERR_load_crypto_strings();
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            ssl_ctx = SSL_CTX_new(TLS_method());
            SSL_CTX_set_mode(ssl_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
            // SSL_CTX_set_options(ssl_ctx, SSL_OP_ENABLE_KTLS);
            
        }

        static SSL_Singleton singleton;
    };

    inline SSL_Singleton SSL_Singleton::singleton = SSL_Singleton();
}

#endif