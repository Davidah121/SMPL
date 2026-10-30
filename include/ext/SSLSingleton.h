#pragma once

#ifdef USE_OPENSSL
#include "StandardTypes.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace smpl
{
    class SSL_Singleton
    {
    public:
		SSL_Singleton(const SSL_Singleton& other) = default;
		SSL_Singleton& operator=(const SSL_Singleton& other) = default;

		SSL_Singleton(SSL_Singleton&& other) = delete;
		SSL_Singleton& operator=(SSL_Singleton&& other) = delete;
        ~SSL_Singleton();
        SSL_CTX* getCTX();
        static SSL_Singleton& getSingleton();

    private:
        SSL_CTX* ssl_ctx = nullptr;
        SSL_Singleton();

        // static SSL_Singleton singleton;
    };

    // inline SSL_Singleton SSL_Singleton::singleton;
}

#endif