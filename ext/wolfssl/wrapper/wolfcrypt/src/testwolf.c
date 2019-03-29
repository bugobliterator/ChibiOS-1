/* test.c
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if HAL_USE_CRY == TRUE
#include "chibioshal.h"

#ifdef HAL_CRYPTO_WOLF_SHA_OPT
#define TEST_SHABUFF_LEN		2*128
#define TEST_SHABUFF_WORD_LEN		(TEST_SHABUFF_LEN / 4)
ALIGNED_VAR(4) uint32_t shabuffer[TEST_SHABUFF_WORD_LEN];
#endif

#endif

#ifdef XMALLOC_USER
    #include <stdlib.h>  /* we're using malloc / free direct here */
#endif



#include <wolfssl/wolfcrypt/memory.h>
#ifdef WOLFSSL_STATIC_MEMORY
    static WOLFSSL_HEAP_HINT* HEAP_HINT;
#else
    #define HEAP_HINT NULL
#endif /* WOLFSSL_STATIC_MEMORY */

#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_TEST_CERT
    #include <wolfssl/wolfcrypt/asn.h>
#else
    #include <wolfssl/wolfcrypt/asn_public.h>
#endif
#include <wolfssl/wolfcrypt/md2.h>
#include <wolfssl/wolfcrypt/md5.h>
#include <wolfssl/wolfcrypt/md4.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/arc4.h>

#if defined(WC_NO_RNG) && defined(USE_FAST_MATH)
    #include <wolfssl/wolfcrypt/tfm.h>
#else
    #include <wolfssl/wolfcrypt/random.h>
#endif

#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/signature.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/des3.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/wc_encrypt.h>
#include <wolfssl/wolfcrypt/cmac.h>
#include <wolfssl/wolfcrypt/poly1305.h>
#include <wolfssl/wolfcrypt/camellia.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/dsa.h>
#include <wolfssl/wolfcrypt/srp.h>
#include <wolfssl/wolfcrypt/idea.h>
#include <wolfssl/wolfcrypt/hc128.h>
#include <wolfssl/wolfcrypt/rabbit.h>
#include <wolfssl/wolfcrypt/chacha.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#include <wolfssl/wolfcrypt/pwdbased.h>
#include <wolfssl/wolfcrypt/ripemd.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>
#endif
#ifdef HAVE_CURVE25519
    #include <wolfssl/wolfcrypt/curve25519.h>
#endif
#ifdef HAVE_ED25519
    #include <wolfssl/wolfcrypt/ed25519.h>
#endif
#ifdef HAVE_BLAKE2
    #include <wolfssl/wolfcrypt/blake2.h>
#endif
#ifdef WOLFSSL_SHA3
    #include <wolfssl/wolfcrypt/sha3.h>
#endif
#ifdef HAVE_LIBZ
    #include <wolfssl/wolfcrypt/compress.h>
#endif
#ifdef HAVE_PKCS7
    #include <wolfssl/wolfcrypt/pkcs7.h>
#endif
#ifdef HAVE_FIPS
    #include <wolfssl/wolfcrypt/fips_test.h>
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif
#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
    #include <wolfssl/wolfcrypt/logging.h>
#endif

/* only for stack size check */
#ifdef HAVE_STACK_SIZE
    #include <wolfssl/ssl.h>
    #define err_sys err_sys_remap /* remap err_sys */
    #include <wolfssl/test.h>
    #undef err_sys
#endif

#ifdef _MSC_VER
    /* 4996 warning to use MS extensions e.g., strcpy_s instead of strncpy */
    #pragma warning(disable: 4996)
#endif

#ifdef OPENSSL_EXTRA
  #ifndef WOLFCRYPT_ONLY
    #include <wolfssl/openssl/evp.h>
  #endif
    #include <wolfssl/openssl/rand.h>
    #include <wolfssl/openssl/hmac.h>
    #include <wolfssl/openssl/aes.h>
    #include <wolfssl/openssl/des.h>
#endif


#if defined(NO_FILESYSTEM)
    #if !defined(USE_CERT_BUFFERS_1024) && !defined(USE_CERT_BUFFERS_2048) && \
        !defined(USE_CERT_BUFFERS_4096)
        #define USE_CERT_BUFFERS_2048
    #endif
    #if !defined(USE_CERT_BUFFERS_256)
        #define USE_CERT_BUFFERS_256
    #endif
#endif

#include <wolfssl/certs_test.h>

#if defined(WOLFSSL_MDK_ARM)
        #include <stdio.h>
        #include <stdlib.h>
    extern FILE * wolfSSL_fopen(const char *fname, const char *mode) ;
    #define fopen wolfSSL_fopen
#endif

#ifdef HAVE_NTRU
    #include "libntruencrypt/ntru_crypto.h"
#endif

#if defined(WOLFSSL_CHIBIOS)
	#undef  printf
#include "chtypes.h"
#include "hal_objects.h"
#include "hal_streams.h"
#include "chprintf.h"
#include "memstreams.h"
extern BaseSequentialStream *serialp;
    int printf(const char *fmt, ...) {
      va_list ap;
      int formatted_bytes;

      va_start(ap, fmt);
      formatted_bytes = chvprintf(serialp, fmt, ap);
      va_end(ap);

      return formatted_bytes;
    }
#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    #include <mqx.h>
    #include <stdlib.h>
    #if MQX_USE_IO_OLD
        #include <fio.h>
    #else
        #include <nio.h>
    #endif
#elif defined(FREESCALE_KSDK_BM)
    #include "fsl_debug_console.h"
    #undef printf
    #define printf PRINTF
#else
    #include <stdio.h>
#endif

#if defined(WOLFSSL_CERT_GEN) && (defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES))
    #define ENABLE_ECC384_CERT_GEN_TEST
#endif


#ifdef THREADX
    /* since just testing, use THREADX log printf instead */
    int dc_log_printf(char*, ...);
        #undef printf
        #define printf dc_log_printf
#endif


#ifdef MICRIUM
    #include <bsp_ser.h>
    void BSP_Ser_Printf (CPU_CHAR* format, ...);
    #undef printf
    #define printf BSP_Ser_Printf
#endif

#include "testwolf.h"


/* for async devices */
static int devId = INVALID_DEVID;

#ifdef HAVE_WNR
    const char* wnrConfigFile = "wnr-example.conf";
#endif

typedef struct testVector {
    const char*  input;
    const char*  output;
    size_t inLen;
    size_t outLen;
} testVector;

int  error_test(void);

int  sha_test(void);
int  sha224_test(void);
int  sha256_test(void);
int  sha512_test(void);
int  sha384_test(void);
int  sha3_test(void);

int  des_test(void);
int  des3_test(void);
int  aes_test(void);
int  aes192_test(void);
int  aes256_test(void);


#ifndef WC_NO_RNG
int  random_test(void);
#endif /* WC_NO_RNG */




#if defined(DEBUG_WOLFSSL) && !defined(HAVE_VALGRIND) && \
        !defined(OPENSSL_EXTRA) && !defined(HAVE_STACK_SIZE)
#ifdef __cplusplus
    extern "C" {
#endif
    WOLFSSL_API int wolfSSL_Debugging_ON(void);
#ifdef __cplusplus
    }  /* extern "C" */
#endif
#endif

/* General big buffer size for many tests. */
#define FOURK_BUF 4096


#define ERROR_OUT(err, eLabel) { ret = (err); goto eLabel; }

#ifdef HAVE_STACK_SIZE
static THREAD_RETURN err_sys(const char* msg, int es)
#else
static int err_sys(const char* msg, int es)
#endif
{
    printf("%s error = %d\n", msg, es);

    EXIT_TEST(-1);
}

#ifndef HAVE_STACK_SIZE
/* func_args from test.h, so don't have to pull in other stuff */
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;
#endif /* !HAVE_STACK_SIZE */

#ifdef HAVE_FIPS

static void myFipsCb(int ok, int err, const char* hash)
{
    printf("in my Fips callback, ok = %d, err = %d\n", ok, err);
    printf("message = %s\n", wc_GetErrorString(err));
    printf("hash = %s\n", hash);

    if (err == IN_CORE_FIPS_E) {
        printf("In core integrity hash check failure, copy above hash\n");
        printf("into verifyCore[] in fips_test.c and rebuild\n");
    }
}

#endif /* HAVE_FIPS */

#ifdef WOLFSSL_STATIC_MEMORY
    #ifdef BENCH_EMBEDDED
        static byte gTestMemory[10000];
    #elif defined(USE_FAST_MATH) && !defined(ALT_ECC_SIZE)
        static byte gTestMemory[130000];
    #else
        static byte gTestMemory[80000];
    #endif
#endif

#ifdef HAVE_STACK_SIZE
THREAD_RETURN WOLFSSL_THREAD wolfcrypt_test(void* args)
#else
//int wolfcrypt_test(void* args)
int wolfcrypt_test(void)
#endif
{
    int ret;

    printf( "starting Chibios Wolfcrypt!\n");

//	--------------------	SHA 	---------------------

    if ( (ret = sha_test()) != 0)
        return err_sys("SHA      test failed!\n", ret);
    else
    	printf( "SHA  test passed!\n");

    if ( (ret = sha256_test()) != 0)
        return err_sys("SHA-256  test failed!\n", ret);
    else
        printf( "SHA-256  test passed!\n");

    if ( (ret = sha512_test()) != 0)
        return err_sys("SHA-512  test failed!\n", ret);
    else
        printf( "SHA-512  test passed!\n");


//	--------------------	DES 	---------------------

    if ( (ret = des_test()) != 0)
        return err_sys("DES      test failed!\n", ret);
    else
        printf( "DES      test passed!\n");



    if ( (ret = des3_test()) != 0)
        return err_sys("DES3     test failed!\n", ret);
    else
        printf( "DES3     test passed!\n");

//	--------------------	AES 	---------------------


    if ( (ret = aes_test()) != 0)
        return err_sys("AES      test failed!\n", ret);
    else
        printf( "AES      test passed!\n");

    if ( (ret = aes192_test()) != 0)
        return err_sys("AES192   test failed!\n", ret);
    else
        printf( "AES192   test passed!\n");

    if ( (ret = aes256_test()) != 0)
        return err_sys("AES256   test failed!\n", ret);
    else
        printf( "AES256   test passed!\n");

//	--------------------	RANDOM 	---------------------
    if ( (ret = random_test()) != 0)
        return err_sys("RANDOM   test failed!\n", ret);
    else
        printf( "RANDOM   test passed!\n");



   return ret;
}


#ifndef NO_SHA
#define TODO_FIX
int sha_test(void)
{
    int ret = 0;
    int i;
    wc_Sha sha;
    byte hash[WC_SHA_DIGEST_SIZE];
    byte hashcopy[WC_SHA_DIGEST_SIZE];
    testVector a, b, c, d, e;
    testVector test_sha[5];

#ifndef TODO_FIX
    int times = sizeof(test_sha) / sizeof(struct testVector), i;
#else
    int times = 3;
#endif
    a.input  = "";
    a.output = "\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18"
               "\x90\xaf\xd8\x07\x09";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = WC_SHA_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\xA9\x99\x3E\x36\x47\x06\x81\x6A\xBA\x3E\x25\x71\x78\x50\xC2"
               "\x6C\x9C\xD0\xD8\x9D";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = WC_SHA_DIGEST_SIZE;

    c.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    c.output = "\x84\x98\x3E\x44\x1C\x3B\xD2\x6E\xBA\xAE\x4A\xA1\xF9\x51\x29"
               "\xE5\xE5\x46\x70\xF1";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = WC_SHA_DIGEST_SIZE;

    d.input  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa";
    d.output = "\x00\x98\xBA\x82\x4B\x5C\x16\x42\x7B\xD7\xA1\x12\x2A\x5A\x44"
               "\x2A\x25\xEC\x64\x4D";
    d.inLen  = XSTRLEN(d.input);
    d.outLen = WC_SHA_DIGEST_SIZE;

    e.input  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaa";
    e.output = "\xAD\x5B\x3F\xDB\xCB\x52\x67\x78\xC2\x83\x9D\x2F\x15\x1E\xA7"
               "\x53\x99\x5E\x26\xA0";
    e.inLen  = XSTRLEN(e.input);
    e.outLen = WC_SHA_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;
    test_sha[3] = d;
    test_sha[4] = e;

#ifdef HAL_CRYPTO_WOLF_SHA_OPT
#endif

    ret = wc_InitSha_ex(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -1700;

    for (i = 0; i < times; ++i) {
    	memset(sha.buffer,0,sha.sha1ctxp.sha.sha_buffer_size);
        ret = wc_ShaUpdate(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-1710 - i, exit);
        ret = wc_ShaGetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-1720 - i, exit);
        ret = wc_ShaFinal(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-1730 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA_DIGEST_SIZE) != 0)
            ERROR_OUT(-1740 - i, exit);
        if (XMEMCMP(hash, hashcopy, WC_SHA_DIGEST_SIZE) != 0)
            ERROR_OUT(-1750 - i, exit);
    }
#ifndef TODO_FIX
    /* BEGIN LARGE HASH TEST */ {
    byte large_input[1024];
    const char* large_digest =
        "\x8b\x77\x02\x48\x39\xe8\xdb\xd3\x9a\xf4\x05\x24\x66\x12\x2d\x9e"
        "\xc5\xd9\x0a\xac";

    for (i = 0; i < (int)sizeof(large_input); i++) {
        large_input[i] = (byte)(i & 0xFF);
    }
    times = 100;
#ifdef WOLFSSL_PIC32MZ_HASH
    wc_ShaSizeSet(&sha, times * sizeof(large_input));
#endif
    for (i = 0; i < times; ++i) {
        ret = wc_ShaUpdate(&sha, (byte*)large_input,
            (word32)sizeof(large_input));
        if (ret != 0)
            ERROR_OUT(-1760, exit);
    }
    ret = wc_ShaFinal(&sha, hash);
    if (ret != 0)
        ERROR_OUT(-1761, exit);
    if (XMEMCMP(hash, large_digest, WC_SHA_DIGEST_SIZE) != 0)
        ERROR_OUT(-1762, exit);
    } /* END LARGE HASH TEST */
#endif

exit:

    wc_ShaFree(&sha);

    return ret;
}

#endif /* NO_SHA */


#ifdef WOLFSSL_SHA224
int sha224_test(void)
{
    wc_Sha224 sha;
    byte      hash[WC_SHA224_DIGEST_SIZE];
    byte      hashcopy[WC_SHA224_DIGEST_SIZE];
    int       ret = 0;

    testVector a, b, c;
    testVector test_sha[3];
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "";
    a.output = "\xd1\x4a\x02\x8c\x2a\x3a\x2b\xc9\x47\x61\x02\xbb\x28\x82\x34"
               "\xc4\x15\xa2\xb0\x1f\x82\x8e\xa6\x2a\xc5\xb3\xe4\x2f";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = WC_SHA224_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\x23\x09\x7d\x22\x34\x05\xd8\x22\x86\x42\xa4\x77\xbd\xa2\x55"
               "\xb3\x2a\xad\xbc\xe4\xbd\xa0\xb3\xf7\xe3\x6c\x9d\xa7";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = WC_SHA224_DIGEST_SIZE;

    c.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    c.output = "\x75\x38\x8b\x16\x51\x27\x76\xcc\x5d\xba\x5d\xa1\xfd\x89\x01"
               "\x50\xb0\xc6\x45\x5c\xb4\xf5\x8b\x19\x52\x52\x25\x25";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = WC_SHA224_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;

    ret = wc_InitSha224_ex(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -2000;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha224Update(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-2010 - i, exit);
        ret = wc_Sha224GetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-2020 - i, exit);
        ret = wc_Sha224Final(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-2030 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA224_DIGEST_SIZE) != 0)
            ERROR_OUT(-2040 - i, exit);
        if (XMEMCMP(hash, hashcopy, WC_SHA224_DIGEST_SIZE) != 0)
            ERROR_OUT(-2050 - i, exit);
    }

exit:
    wc_Sha224Free(&sha);

    return ret;
}
#endif


#ifndef NO_SHA256
int sha256_test(void)
{
    wc_Sha256 sha;
    byte      hash[WC_SHA256_DIGEST_SIZE];
    byte      hashcopy[WC_SHA256_DIGEST_SIZE];
    int       ret = 0;

    testVector a, b, c;
    testVector test_sha[3];
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "";
    a.output = "\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9"
               "\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52"
               "\xb8\x55";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = WC_SHA256_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\xBA\x78\x16\xBF\x8F\x01\xCF\xEA\x41\x41\x40\xDE\x5D\xAE\x22"
               "\x23\xB0\x03\x61\xA3\x96\x17\x7A\x9C\xB4\x10\xFF\x61\xF2\x00"
               "\x15\xAD";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = WC_SHA256_DIGEST_SIZE;

    c.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    c.output = "\x24\x8D\x6A\x61\xD2\x06\x38\xB8\xE5\xC0\x26\x93\x0C\x3E\x60"
               "\x39\xA3\x3C\xE4\x59\x64\xFF\x21\x67\xF6\xEC\xED\xD4\x19\xDB"
               "\x06\xC1";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = WC_SHA256_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;

    ret = wc_InitSha256_ex(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -2100;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha256Update(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-2110 - i, exit);
        ret = wc_Sha256GetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-2120 - i, exit);
        ret = wc_Sha256Final(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-2130 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA256_DIGEST_SIZE) != 0)
            ERROR_OUT(-2140 - i, exit);
        if (XMEMCMP(hash, hashcopy, WC_SHA256_DIGEST_SIZE) != 0)
            ERROR_OUT(-2150 - i, exit);
    }

    /* BEGIN LARGE HASH TEST */ {
    byte large_input[1024];
    const char* large_digest =
        "\x27\x78\x3e\x87\x96\x3a\x4e\xfb\x68\x29\xb5\x31\xc9\xba\x57\xb4"
        "\x4f\x45\x79\x7f\x67\x70\xbd\x63\x7f\xbf\x0d\x80\x7c\xbd\xba\xe0";

    for (i = 0; i < (int)sizeof(large_input); i++) {
        large_input[i] = (byte)(i & 0xFF);
    }
    times = 100;
#ifdef WOLFSSL_PIC32MZ_HASH
    wc_Sha256SizeSet(&sha, times * sizeof(large_input));
#endif
    for (i = 0; i < times; ++i) {
        ret = wc_Sha256Update(&sha, (byte*)large_input,
            (word32)sizeof(large_input));
        if (ret != 0)
            ERROR_OUT(-2160, exit);
    }
    ret = wc_Sha256Final(&sha, hash);
    if (ret != 0)
        ERROR_OUT(-2161, exit);
    if (XMEMCMP(hash, large_digest, WC_SHA256_DIGEST_SIZE) != 0)
        ERROR_OUT(-2162, exit);
    } /* END LARGE HASH TEST */

exit:

    wc_Sha256Free(&sha);

    return ret;
}
#endif


#ifdef WOLFSSL_SHA512
int sha512_test(void)
{
    wc_Sha512 sha;
    byte      hash[WC_SHA512_DIGEST_SIZE];
    byte      hashcopy[WC_SHA512_DIGEST_SIZE];
    int       ret = 0;

    testVector a, b, c;
    testVector test_sha[3];
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "";
    a.output = "\xcf\x83\xe1\x35\x7e\xef\xb8\xbd\xf1\x54\x28\x50\xd6\x6d\x80"
               "\x07\xd6\x20\xe4\x05\x0b\x57\x15\xdc\x83\xf4\xa9\x21\xd3\x6c"
               "\xe9\xce\x47\xd0\xd1\x3c\x5d\x85\xf2\xb0\xff\x83\x18\xd2\x87"
               "\x7e\xec\x2f\x63\xb9\x31\xbd\x47\x41\x7a\x81\xa5\x38\x32\x7a"
               "\xf9\x27\xda\x3e";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = WC_SHA512_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\xdd\xaf\x35\xa1\x93\x61\x7a\xba\xcc\x41\x73\x49\xae\x20\x41"
               "\x31\x12\xe6\xfa\x4e\x89\xa9\x7e\xa2\x0a\x9e\xee\xe6\x4b\x55"
               "\xd3\x9a\x21\x92\x99\x2a\x27\x4f\xc1\xa8\x36\xba\x3c\x23\xa3"
               "\xfe\xeb\xbd\x45\x4d\x44\x23\x64\x3c\xe8\x0e\x2a\x9a\xc9\x4f"
               "\xa5\x4c\xa4\x9f";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = WC_SHA512_DIGEST_SIZE;

    c.input  = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhi"
               "jklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    c.output = "\x8e\x95\x9b\x75\xda\xe3\x13\xda\x8c\xf4\xf7\x28\x14\xfc\x14"
               "\x3f\x8f\x77\x79\xc6\xeb\x9f\x7f\xa1\x72\x99\xae\xad\xb6\x88"
               "\x90\x18\x50\x1d\x28\x9e\x49\x00\xf7\xe4\x33\x1b\x99\xde\xc4"
               "\xb5\x43\x3a\xc7\xd3\x29\xee\xb6\xdd\x26\x54\x5e\x96\xe5\x5b"
               "\x87\x4b\xe9\x09";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = WC_SHA512_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;

    ret = wc_InitSha512_ex(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -2200;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha512Update(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-2210 - i, exit);
        ret = wc_Sha512GetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-2220 - i, exit);
        ret = wc_Sha512Final(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-2230 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA512_DIGEST_SIZE) != 0)
            ERROR_OUT(-2240 - i, exit);
        if (XMEMCMP(hash, hashcopy, WC_SHA512_DIGEST_SIZE) != 0)
            ERROR_OUT(-2250 - i, exit);
    }

    /* BEGIN LARGE HASH TEST */ {
    byte large_input[1024];
    const char* large_digest =
        "\x5a\x1f\x73\x90\xbd\x8c\xe4\x63\x54\xce\xa0\x9b\xef\x32\x78\x2d"
        "\x2e\xe7\x0d\x5e\x2f\x9d\x15\x1b\xdd\x2d\xde\x65\x0c\x7b\xfa\x83"
        "\x5e\x80\x02\x13\x84\xb8\x3f\xff\x71\x62\xb5\x09\x89\x63\xe1\xdc"
        "\xa5\xdc\xfc\xfa\x9d\x1a\x4d\xc0\xfa\x3a\x14\xf6\x01\x51\x90\xa4";

    for (i = 0; i < (int)sizeof(large_input); i++) {
        large_input[i] = (byte)(i & 0xFF);
    }
    times = 100;
    for (i = 0; i < times; ++i) {
        ret = wc_Sha512Update(&sha, (byte*)large_input,
            (word32)sizeof(large_input));
        if (ret != 0)
            ERROR_OUT(-2260, exit);
    }
    ret = wc_Sha512Final(&sha, hash);
    if (ret != 0)
        ERROR_OUT(-2261, exit);
    if (XMEMCMP(hash, large_digest, WC_SHA512_DIGEST_SIZE) != 0)
        ERROR_OUT(-2262, exit);
    } /* END LARGE HASH TEST */

exit:
    wc_Sha512Free(&sha);

    return ret;
}
#endif


#ifdef WOLFSSL_SHA384
int sha384_test(void)
{
    wc_Sha384 sha;
    byte      hash[WC_SHA384_DIGEST_SIZE];
    byte      hashcopy[WC_SHA384_DIGEST_SIZE];
    int       ret = 0;

    testVector a, b, c;
    testVector test_sha[3];
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "";

    a.output = "\x38\xb0\x60\xa7\x51\xac\x96\x38\x4c\xd9\x32\x7e\xb1\xb1\xe3"
               "\x6a\x21\xfd\xb7\x11\x14\xbe\x07\x43\x4c\x0c\xc7\xbf\x63\xf6"
               "\xe1\xda\x27\x4e\xde\xbf\xe7\x6f\x65\xfb\xd5\x1a\xd2\xf1\x48"
               "\x98\xb9\x5b";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = WC_SHA384_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\xcb\x00\x75\x3f\x45\xa3\x5e\x8b\xb5\xa0\x3d\x69\x9a\xc6\x50"
               "\x07\x27\x2c\x32\xab\x0e\xde\xd1\x63\x1a\x8b\x60\x5a\x43\xff"
               "\x5b\xed\x80\x86\x07\x2b\xa1\xe7\xcc\x23\x58\xba\xec\xa1\x34"
               "\xc8\x25\xa7";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = WC_SHA384_DIGEST_SIZE;

    c.input  = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhi"
               "jklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    c.output = "\x09\x33\x0c\x33\xf7\x11\x47\xe8\x3d\x19\x2f\xc7\x82\xcd\x1b"
               "\x47\x53\x11\x1b\x17\x3b\x3b\x05\xd2\x2f\xa0\x80\x86\xe3\xb0"
               "\xf7\x12\xfc\xc7\xc7\x1a\x55\x7e\x2d\xb9\x66\xc3\xe9\xfa\x91"
               "\x74\x60\x39";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = WC_SHA384_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;

    ret = wc_InitSha384_ex(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -2300;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha384Update(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-2310 - i, exit);
        ret = wc_Sha384GetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-2320 - i, exit);
        ret = wc_Sha384Final(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-2330 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA384_DIGEST_SIZE) != 0)
            ERROR_OUT(-2340 - i, exit);
        if (XMEMCMP(hash, hashcopy, WC_SHA384_DIGEST_SIZE) != 0)
            ERROR_OUT(-2350 - i, exit);
    }

    /* BEGIN LARGE HASH TEST */ {
    byte large_input[1024];
    const char* large_digest =
        "\x37\x01\xdb\xff\x1e\x40\x4f\xe1\xe2\xea\x0b\x40\xbb\x3b\x39\x9a"
        "\xcc\xe8\x44\x8e\x7e\xe5\x64\xb5\x6b\x7f\x56\x64\xa7\x2b\x84\xe3"
        "\xc5\xd7\x79\x03\x25\x90\xf7\xa4\x58\xcb\x97\xa8\x8b\xb1\xa4\x81";

    for (i = 0; i < (int)sizeof(large_input); i++) {
        large_input[i] = (byte)(i & 0xFF);
    }
    times = 100;
    for (i = 0; i < times; ++i) {
        ret = wc_Sha384Update(&sha, (byte*)large_input,
            (word32)sizeof(large_input));
        if (ret != 0)
            ERROR_OUT(-2360, exit);
    }
    ret = wc_Sha384Final(&sha, hash);
    if (ret != 0)
        ERROR_OUT(-2361, exit);
    if (XMEMCMP(hash, large_digest, WC_SHA384_DIGEST_SIZE) != 0)
        ERROR_OUT(-2362, exit);
    } /* END LARGE HASH TEST */

exit:

    wc_Sha384Free(&sha);

    return ret;
}
#endif /* WOLFSSL_SHA384 */

#ifdef WOLFSSL_SHA3
#ifndef WOLFSSL_NOSHA3_224
static int sha3_224_test(void)
{
    Sha3  sha;
    byte  hash[SHA3_224_DIGEST_SIZE];
    byte  hashcopy[SHA3_224_DIGEST_SIZE];

    testVector a, b, c;
    testVector test_sha[3];
    int ret = 0;
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "";
    a.output = "\x6b\x4e\x03\x42\x36\x67\xdb\xb7\x3b\x6e\x15\x45\x4f\x0e\xb1"
               "\xab\xd4\x59\x7f\x9a\x1b\x07\x8e\x3f\x5b\x5a\x6b\xc7";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = SHA3_224_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\xe6\x42\x82\x4c\x3f\x8c\xf2\x4a\xd0\x92\x34\xee\x7d\x3c\x76"
               "\x6f\xc9\xa3\xa5\x16\x8d\x0c\x94\xad\x73\xb4\x6f\xdf";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = SHA3_224_DIGEST_SIZE;

    c.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    c.output = "\x8a\x24\x10\x8b\x15\x4a\xda\x21\xc9\xfd\x55\x74\x49\x44\x79"
               "\xba\x5c\x7e\x7a\xb7\x6e\xf2\x64\xea\xd0\xfc\xce\x33";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = SHA3_224_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;

    ret = wc_InitSha3_224(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -2000;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha3_224_Update(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-2010 - i, exit);
        ret = wc_Sha3_224_GetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-2020 - i, exit);
        ret = wc_Sha3_224_Final(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-2030 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, SHA3_224_DIGEST_SIZE) != 0)
            ERROR_OUT(-2040 - i, exit);
        if (XMEMCMP(hash, hashcopy, SHA3_224_DIGEST_SIZE) != 0)
            ERROR_OUT(-2050 - i, exit);
    }

    /* BEGIN LARGE HASH TEST */ {
    byte large_input[1024];
    const char* large_digest =
        "\x13\xe5\xd3\x98\x7b\x94\xda\x41\x12\xc7\x1e\x92\x3a\x19"
        "\x21\x20\x86\x6f\x24\xbf\x0a\x31\xbc\xfd\xd6\x70\x36\xf3";

    for (i = 0; i < (int)sizeof(large_input); i++) {
        large_input[i] = (byte)(i & 0xFF);
    }
    times = 100;
    for (i = 0; i < times; ++i) {
        ret = wc_Sha3_224_Update(&sha, (byte*)large_input,
            (word32)sizeof(large_input));
        if (ret != 0)
            ERROR_OUT(-2060, exit);
    }
    ret = wc_Sha3_224_Final(&sha, hash);
    if (ret != 0)
        ERROR_OUT(-2061, exit);
    if (XMEMCMP(hash, large_digest, SHA3_224_DIGEST_SIZE) != 0)
        ERROR_OUT(-2062, exit);
    } /* END LARGE HASH TEST */

exit:
    wc_Sha3_224_Free(&sha);

    return ret;
}
#endif /* WOLFSSL_NOSHA3_224 */

#ifndef WOLFSSL_NOSHA3_256
static int sha3_256_test(void)
{
    Sha3  sha;
    byte  hash[SHA3_256_DIGEST_SIZE];
    byte  hashcopy[SHA3_256_DIGEST_SIZE];

    testVector a, b, c;
    testVector test_sha[3];
    int ret = 0;
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "";
    a.output = "\xa7\xff\xc6\xf8\xbf\x1e\xd7\x66\x51\xc1\x47\x56\xa0\x61\xd6"
               "\x62\xf5\x80\xff\x4d\xe4\x3b\x49\xfa\x82\xd8\x0a\x4b\x80\xf8"
               "\x43\x4a";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = SHA3_256_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\x3a\x98\x5d\xa7\x4f\xe2\x25\xb2\x04\x5c\x17\x2d\x6b\xd3\x90"
               "\xbd\x85\x5f\x08\x6e\x3e\x9d\x52\x5b\x46\xbf\xe2\x45\x11\x43"
               "\x15\x32";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = SHA3_256_DIGEST_SIZE;

    c.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    c.output = "\x41\xc0\xdb\xa2\xa9\xd6\x24\x08\x49\x10\x03\x76\xa8\x23\x5e"
               "\x2c\x82\xe1\xb9\x99\x8a\x99\x9e\x21\xdb\x32\xdd\x97\x49\x6d"
               "\x33\x76";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = SHA3_256_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;

    ret = wc_InitSha3_256(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -2100;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha3_256_Update(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-2110 - i, exit);
        ret = wc_Sha3_256_GetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-2120 - i, exit);
        ret = wc_Sha3_256_Final(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-2130 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, SHA3_256_DIGEST_SIZE) != 0)
            ERROR_OUT(-2140 - i, exit);
        if (XMEMCMP(hash, hashcopy, SHA3_256_DIGEST_SIZE) != 0)
            ERROR_OUT(-2150 - i, exit);
    }

    /* BEGIN LARGE HASH TEST */ {
    byte large_input[1024];
    const char* large_digest =
        "\xdc\x90\xc0\xb1\x25\xdb\x2c\x34\x81\xa3\xff\xbc\x1e\x2e\x87\xeb"
        "\x6d\x70\x85\x61\xe0\xe9\x63\x61\xff\xe5\x84\x4b\x1f\x68\x05\x15";

    for (i = 0; i < (int)sizeof(large_input); i++) {
        large_input[i] = (byte)(i & 0xFF);
    }
    times = 100;
    for (i = 0; i < times; ++i) {
        ret = wc_Sha3_256_Update(&sha, (byte*)large_input,
            (word32)sizeof(large_input));
        if (ret != 0)
            ERROR_OUT(-2160, exit);
    }
    ret = wc_Sha3_256_Final(&sha, hash);
    if (ret != 0)
        ERROR_OUT(-2161, exit);
    if (XMEMCMP(hash, large_digest, SHA3_256_DIGEST_SIZE) != 0)
        ERROR_OUT(-2162, exit);
    } /* END LARGE HASH TEST */

exit:
    wc_Sha3_256_Free(&sha);

    return ret;
}
#endif /* WOLFSSL_NOSHA3_256 */

#ifndef WOLFSSL_NOSHA3_384
static int sha3_384_test(void)
{
    Sha3  sha;
    byte  hash[SHA3_384_DIGEST_SIZE];
    byte  hashcopy[SHA3_384_DIGEST_SIZE];

    testVector a, b, c;
    testVector test_sha[3];
    int ret;
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "";
    a.output = "\x0c\x63\xa7\x5b\x84\x5e\x4f\x7d\x01\x10\x7d\x85\x2e\x4c\x24"
               "\x85\xc5\x1a\x50\xaa\xaa\x94\xfc\x61\x99\x5e\x71\xbb\xee\x98"
               "\x3a\x2a\xc3\x71\x38\x31\x26\x4a\xdb\x47\xfb\x6b\xd1\xe0\x58"
               "\xd5\xf0\x04";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = SHA3_384_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\xec\x01\x49\x82\x88\x51\x6f\xc9\x26\x45\x9f\x58\xe2\xc6\xad"
               "\x8d\xf9\xb4\x73\xcb\x0f\xc0\x8c\x25\x96\xda\x7c\xf0\xe4\x9b"
               "\xe4\xb2\x98\xd8\x8c\xea\x92\x7a\xc7\xf5\x39\xf1\xed\xf2\x28"
               "\x37\x6d\x25";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = SHA3_384_DIGEST_SIZE;

    c.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    c.output = "\x99\x1c\x66\x57\x55\xeb\x3a\x4b\x6b\xbd\xfb\x75\xc7\x8a\x49"
               "\x2e\x8c\x56\xa2\x2c\x5c\x4d\x7e\x42\x9b\xfd\xbc\x32\xb9\xd4"
               "\xad\x5a\xa0\x4a\x1f\x07\x6e\x62\xfe\xa1\x9e\xef\x51\xac\xd0"
               "\x65\x7c\x22";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = SHA3_384_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;

    ret = wc_InitSha3_384(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -2200;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha3_384_Update(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-2210 - i, exit);
        ret = wc_Sha3_384_GetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-2220 - i, exit);
        ret = wc_Sha3_384_Final(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-2230 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, SHA3_384_DIGEST_SIZE) != 0)
            ERROR_OUT(-2240 - i, exit);
        if (XMEMCMP(hash, hashcopy, SHA3_384_DIGEST_SIZE) != 0)
            ERROR_OUT(-2250 - i, exit);
    }

    /* BEGIN LARGE HASH TEST */ {
    byte large_input[1024];
    const char* large_digest =
        "\x30\x44\xec\x17\xef\x47\x9f\x55\x36\x11\xd6\x3f\x8a\x31\x5a\x71"
        "\x8a\x71\xa7\x1d\x8e\x84\xe8\x6c\x24\x02\x2f\x7a\x08\x4e\xea\xd7"
        "\x42\x36\x5d\xa8\xc2\xb7\x42\xad\xec\x19\xfb\xca\xc6\x64\xb3\xa4";

    for (i = 0; i < (int)sizeof(large_input); i++) {
        large_input[i] = (byte)(i & 0xFF);
    }
    times = 100;
    for (i = 0; i < times; ++i) {
        ret = wc_Sha3_384_Update(&sha, (byte*)large_input,
            (word32)sizeof(large_input));
        if (ret != 0)
            ERROR_OUT(-2260, exit);
    }
    ret = wc_Sha3_384_Final(&sha, hash);
    if (ret != 0)
        ERROR_OUT(-2261, exit);
    if (XMEMCMP(hash, large_digest, SHA3_384_DIGEST_SIZE) != 0)
        ERROR_OUT(-2262, exit);
    } /* END LARGE HASH TEST */

exit:
    wc_Sha3_384_Free(&sha);

    return ret;
}
#endif /* WOLFSSL_NOSHA3_384 */

#ifndef WOLFSSL_NOSHA3_512
static int sha3_512_test(void)
{
    Sha3  sha;
    byte  hash[SHA3_512_DIGEST_SIZE];
    byte  hashcopy[SHA3_512_DIGEST_SIZE];

    testVector a, b, c;
    testVector test_sha[3];
    int ret;
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "";
    a.output = "\xa6\x9f\x73\xcc\xa2\x3a\x9a\xc5\xc8\xb5\x67\xdc\x18\x5a\x75"
               "\x6e\x97\xc9\x82\x16\x4f\xe2\x58\x59\xe0\xd1\xdc\xc1\x47\x5c"
               "\x80\xa6\x15\xb2\x12\x3a\xf1\xf5\xf9\x4c\x11\xe3\xe9\x40\x2c"
               "\x3a\xc5\x58\xf5\x00\x19\x9d\x95\xb6\xd3\xe3\x01\x75\x85\x86"
               "\x28\x1d\xcd\x26";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = SHA3_512_DIGEST_SIZE;

    b.input  = "abc";
    b.output = "\xb7\x51\x85\x0b\x1a\x57\x16\x8a\x56\x93\xcd\x92\x4b\x6b\x09"
               "\x6e\x08\xf6\x21\x82\x74\x44\xf7\x0d\x88\x4f\x5d\x02\x40\xd2"
               "\x71\x2e\x10\xe1\x16\xe9\x19\x2a\xf3\xc9\x1a\x7e\xc5\x76\x47"
               "\xe3\x93\x40\x57\x34\x0b\x4c\xf4\x08\xd5\xa5\x65\x92\xf8\x27"
               "\x4e\xec\x53\xf0";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = SHA3_512_DIGEST_SIZE;

    c.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    c.output = "\x04\xa3\x71\xe8\x4e\xcf\xb5\xb8\xb7\x7c\xb4\x86\x10\xfc\xa8"
               "\x18\x2d\xd4\x57\xce\x6f\x32\x6a\x0f\xd3\xd7\xec\x2f\x1e\x91"
               "\x63\x6d\xee\x69\x1f\xbe\x0c\x98\x53\x02\xba\x1b\x0d\x8d\xc7"
               "\x8c\x08\x63\x46\xb5\x33\xb4\x9c\x03\x0d\x99\xa2\x7d\xaf\x11"
               "\x39\xd6\xe7\x5e";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = SHA3_512_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;

    ret = wc_InitSha3_512(&sha, HEAP_HINT, devId);
    if (ret != 0)
        return -2300;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha3_512_Update(&sha, (byte*)test_sha[i].input,
            (word32)test_sha[i].inLen);
        if (ret != 0)
            ERROR_OUT(-2310 - i, exit);
        ret = wc_Sha3_512_GetHash(&sha, hashcopy);
        if (ret != 0)
            ERROR_OUT(-2320 - i, exit);
        ret = wc_Sha3_512_Final(&sha, hash);
        if (ret != 0)
            ERROR_OUT(-2330 - i, exit);

        if (XMEMCMP(hash, test_sha[i].output, SHA3_512_DIGEST_SIZE) != 0)
            ERROR_OUT(-2340 - i, exit);
        if (XMEMCMP(hash, hashcopy, SHA3_512_DIGEST_SIZE) != 0)
            ERROR_OUT(-2350 - i, exit);
    }

    /* BEGIN LARGE HASH TEST */ {
    byte large_input[1024];
    const char* large_digest =
        "\x9c\x13\x26\xb6\x26\xb2\x94\x31\xbc\xf4\x34\xe9\x6f\xf2\xd6\x29"
        "\x9a\xd0\x9b\x32\x63\x2f\x18\xa7\x5f\x23\xc9\x60\xc2\x32\x0c\xbc"
        "\x57\x77\x33\xf1\x83\x81\x8a\xd3\x15\x7c\x93\xdc\x80\x9f\xed\x61"
        "\x41\xa7\x5b\xfd\x32\x0e\x38\x15\xb0\x46\x3b\x7a\x4f\xfd\x44\x88";

    for (i = 0; i < (int)sizeof(large_input); i++) {
        large_input[i] = (byte)(i & 0xFF);
    }
    times = 100;
    for (i = 0; i < times; ++i) {
        ret = wc_Sha3_512_Update(&sha, (byte*)large_input,
            (word32)sizeof(large_input));
        if (ret != 0)
            ERROR_OUT(-2360, exit);
    }
    ret = wc_Sha3_512_Final(&sha, hash);
    if (ret != 0)
        ERROR_OUT(-2361, exit);
    if (XMEMCMP(hash, large_digest, SHA3_512_DIGEST_SIZE) != 0)
        ERROR_OUT(-2362, exit);
    } /* END LARGE HASH TEST */

exit:
    wc_Sha3_512_Free(&sha);

    return ret;
}
#endif /* WOLFSSL_NOSHA3_512 */

int sha3_test(void)
{
    int ret;

#ifndef WOLFSSL_NOSHA3_224
    if ((ret = sha3_224_test()) != 0)
        return ret;
#endif
#ifndef WOLFSSL_NOSHA3_256
    if ((ret = sha3_256_test()) != 0)
        return ret;
#endif
#ifndef WOLFSSL_NOSHA3_384
    if ((ret = sha3_384_test()) != 0)
        return ret;
#endif
#ifndef WOLFSSL_NOSHA3_512
    if ((ret = sha3_512_test()) != 0)
        return ret;
#endif

    return 0;
}
#endif



#ifndef NO_DES3
int des_test(void)
{
    const byte vector[] = { /* "now is the time for all " w/o trailing 0 */
        0x6e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };

    byte plain[24];
    byte cipher[24];

    Des enc;
    Des dec;

    const byte key[] =
    {
        0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef
    };

    const byte iv[] =
    {
        0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef
    };

    const byte verify[] =
    {
        0x8b,0x7c,0x52,0xb0,0x01,0x2b,0x6c,0xb8,
        0x4f,0x0f,0xeb,0xf3,0xfb,0x5f,0x86,0x73,
        0x15,0x85,0xb3,0x22,0x4b,0x86,0x2b,0x4b
    };

    int ret;

    ret = wc_Des_SetKey(&enc, key, iv, DES_ENCRYPTION);
    if (ret != 0)
        return -3800;

    ret = wc_Des_CbcEncrypt(&enc, cipher, vector, sizeof(vector));

    if (ret != 0)
        return -3801;

    ret = wc_Des_SetKey(&dec, key, iv, DES_DECRYPTION);
    if (ret != 0)
        return -3802;

    ret = wc_Des_CbcDecrypt(&dec, plain, cipher, sizeof(cipher));
    if (ret != 0)
        return -3803;

    if (XMEMCMP(plain, vector, sizeof(plain)))
        return -3804;

    if (XMEMCMP(cipher, verify, sizeof(cipher)))
        return -3805;

    return 0;
}
#endif /* NO_DES3 */


#ifndef NO_DES3
int des3_test(void)
{
    const byte vector[] = { /* "Now is the time for all " w/o trailing 0 */
        0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };

    byte plain[24];
    byte cipher[24];

    Des3 enc;
    Des3 dec;

    const byte key3[] =
    {
        0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
        0xfe,0xde,0xba,0x98,0x76,0x54,0x32,0x10,
        0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67
    };
    const byte iv3[] =
    {
        0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x11,0x21,0x31,0x41,0x51,0x61,0x71,0x81

    };

    const byte verify3[] =
    {
        0x43,0xa0,0x29,0x7e,0xd1,0x84,0xf8,0x0e,
        0x89,0x64,0x84,0x32,0x12,0xd5,0x08,0x98,
        0x18,0x94,0x15,0x74,0x87,0x12,0x7d,0xb0
    };

    int ret;


    if (wc_Des3Init(&enc, HEAP_HINT, devId) != 0)
        return -3900;
    if (wc_Des3Init(&dec, HEAP_HINT, devId) != 0)
        return -3901;

    ret = wc_Des3_SetKey(&enc, key3, iv3, DES_ENCRYPTION);
    if (ret != 0)
        return -3902;
    ret = wc_Des3_SetKey(&dec, key3, iv3, DES_DECRYPTION);
    if (ret != 0)
        return -3903;
    ret = wc_Des3_CbcEncrypt(&enc, cipher, vector, sizeof(vector));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &enc.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -3904;
    ret = wc_Des3_CbcDecrypt(&dec, plain, cipher, sizeof(cipher));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &dec.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -3905;

    if (XMEMCMP(plain, vector, sizeof(plain)))
        return -3906;

    if (XMEMCMP(cipher, verify3, sizeof(cipher)))
        return -3907;

    wc_Des3Free(&enc);
    wc_Des3Free(&dec);

    return 0;
}
#endif /* NO_DES */


#ifndef NO_AES
static int aes_key_size_test(void)
{
    int    ret;
    Aes    aes;
    byte   key16[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                       0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66 };
    byte   key24[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                       0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
                       0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 };
    byte   key32[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                       0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
                       0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                       0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66 };
    byte   iv[]    = "1234567890abcdef";
#ifndef HAVE_FIPS
    word32 keySize;
#endif

#ifdef WC_INITAES_H
    ret = wc_InitAes_h(NULL, NULL);
    if (ret != BAD_FUNC_ARG)
        return -4000;
    ret = wc_InitAes_h(&aes, NULL);
    if (ret != 0)
        return -4001;
#endif

#ifndef HAVE_FIPS
    /* Parameter Validation testing. */
    ret = wc_AesGetKeySize(NULL, NULL);
    if (ret != BAD_FUNC_ARG)
        return -4002;
    ret = wc_AesGetKeySize(&aes, NULL);
    if (ret != BAD_FUNC_ARG)
        return -4003;
    ret = wc_AesGetKeySize(NULL, &keySize);
    if (ret != BAD_FUNC_ARG)
        return -4004;
    /* Crashes in FIPS */
    ret = wc_AesSetKey(NULL, key16, sizeof(key16), iv, AES_ENCRYPTION);
    if (ret != BAD_FUNC_ARG)
        return -4005;
#endif
    /* NULL IV indicates to use all zeros IV. */
    ret = wc_AesSetKey(&aes, key16, sizeof(key16), NULL, AES_ENCRYPTION);
    if (ret != 0)
        return -4006;
    ret = wc_AesSetKey(&aes, key32, sizeof(key32) - 1, iv, AES_ENCRYPTION);
    if (ret != BAD_FUNC_ARG)
        return -4007;
#ifndef HAVE_FIPS
    /* Force invalid rounds */
    aes.rounds = 16;
    ret = wc_AesGetKeySize(&aes, &keySize);
    if (ret != BAD_FUNC_ARG)
        return -4008;
#endif

    ret = wc_AesSetKey(&aes, key16, sizeof(key16), iv, AES_ENCRYPTION);
    if (ret != 0)
        return -4009;
#ifndef HAVE_FIPS
    ret = wc_AesGetKeySize(&aes, &keySize);
    if (ret != 0 || keySize != sizeof(key16))
        return -4010;
#endif

    ret = wc_AesSetKey(&aes, key24, sizeof(key24), iv, AES_ENCRYPTION);
    if (ret != 0)
        return -4011;
#ifndef HAVE_FIPS
    ret = wc_AesGetKeySize(&aes, &keySize);
    if (ret != 0 || keySize != sizeof(key24))
        return -4012;
#endif

    ret = wc_AesSetKey(&aes, key32, sizeof(key32), iv, AES_ENCRYPTION);
    if (ret != 0)
        return -4013;
#ifndef HAVE_FIPS
    ret = wc_AesGetKeySize(&aes, &keySize);
    if (ret != 0 || keySize != sizeof(key32))
        return -4014;
#endif

    return 0;
}
#endif

#if defined(WOLFSSL_AES_XTS)
/* test vectors from http://csrc.nist.gov/groups/STM/cavp/block-cipher-modes.html */
static int aes_xts_128_test(void)
{
    XtsAes aes;
    int ret = 0;
    unsigned char buf[AES_BLOCK_SIZE * 2];
    unsigned char cipher[AES_BLOCK_SIZE * 2];

    /* 128 key tests */
    static unsigned char k1[] = {
        0xa1, 0xb9, 0x0c, 0xba, 0x3f, 0x06, 0xac, 0x35,
        0x3b, 0x2c, 0x34, 0x38, 0x76, 0x08, 0x17, 0x62,
        0x09, 0x09, 0x23, 0x02, 0x6e, 0x91, 0x77, 0x18,
        0x15, 0xf2, 0x9d, 0xab, 0x01, 0x93, 0x2f, 0x2f
    };

    static unsigned char i1[] = {
        0x4f, 0xae, 0xf7, 0x11, 0x7c, 0xda, 0x59, 0xc6,
        0x6e, 0x4b, 0x92, 0x01, 0x3e, 0x76, 0x8a, 0xd5
    };

    static unsigned char p1[] = {
        0xeb, 0xab, 0xce, 0x95, 0xb1, 0x4d, 0x3c, 0x8d,
        0x6f, 0xb3, 0x50, 0x39, 0x07, 0x90, 0x31, 0x1c
    };

    /* plain text test of partial block is not from NIST test vector list */
    static unsigned char pp[] = {
        0xeb, 0xab, 0xce, 0x95, 0xb1, 0x4d, 0x3c, 0x8d,
        0x6f, 0xb3, 0x50, 0x39, 0x07, 0x90, 0x31, 0x1c,
        0x6e, 0x4b, 0x92, 0x01, 0x3e, 0x76, 0x8a, 0xd5
    };

    static unsigned char c1[] = {
        0x77, 0x8a, 0xe8, 0xb4, 0x3c, 0xb9, 0x8d, 0x5a,
        0x82, 0x50, 0x81, 0xd5, 0xbe, 0x47, 0x1c, 0x63
    };

    static unsigned char k2[] = {
        0x39, 0x25, 0x79, 0x05, 0xdf, 0xcc, 0x77, 0x76,
        0x6c, 0x87, 0x0a, 0x80, 0x6a, 0x60, 0xe3, 0xc0,
        0x93, 0xd1, 0x2a, 0xcf, 0xcb, 0x51, 0x42, 0xfa,
        0x09, 0x69, 0x89, 0x62, 0x5b, 0x60, 0xdb, 0x16
    };

    static unsigned char i2[] = {
        0x5c, 0xf7, 0x9d, 0xb6, 0xc5, 0xcd, 0x99, 0x1a,
        0x1c, 0x78, 0x81, 0x42, 0x24, 0x95, 0x1e, 0x84
    };

    static unsigned char p2[] = {
        0xbd, 0xc5, 0x46, 0x8f, 0xbc, 0x8d, 0x50, 0xa1,
        0x0d, 0x1c, 0x85, 0x7f, 0x79, 0x1c, 0x5c, 0xba,
        0xb3, 0x81, 0x0d, 0x0d, 0x73, 0xcf, 0x8f, 0x20,
        0x46, 0xb1, 0xd1, 0x9e, 0x7d, 0x5d, 0x8a, 0x56
    };

    static unsigned char c2[] = {
        0xd6, 0xbe, 0x04, 0x6d, 0x41, 0xf2, 0x3b, 0x5e,
        0xd7, 0x0b, 0x6b, 0x3d, 0x5c, 0x8e, 0x66, 0x23,
        0x2b, 0xe6, 0xb8, 0x07, 0xd4, 0xdc, 0xc6, 0x0e,
        0xff, 0x8d, 0xbc, 0x1d, 0x9f, 0x7f, 0xc8, 0x22
    };

    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k2, sizeof(k2), AES_ENCRYPTION,
            HEAP_HINT, devId) != 0)
        return -4000;
    ret = wc_AesXtsEncrypt(&aes, buf, p2, sizeof(p2), i2, sizeof(i2));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4001;
    if (XMEMCMP(c2, buf, sizeof(c2)))
        return -4002;

    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k1, sizeof(k1), AES_ENCRYPTION,
            HEAP_HINT, devId) != 0)
        return -4003;
    ret = wc_AesXtsEncrypt(&aes, buf, p1, sizeof(p1), i1, sizeof(i1));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4004;
    if (XMEMCMP(c1, buf, AES_BLOCK_SIZE))
        return -4005;

    /* partial block encryption test */
    XMEMSET(cipher, 0, sizeof(cipher));
    ret = wc_AesXtsEncrypt(&aes, cipher, pp, sizeof(pp), i1, sizeof(i1));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4006;
    wc_AesXtsFree(&aes);

    /* partial block decrypt test */
    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k1, sizeof(k1), AES_DECRYPTION,
            HEAP_HINT, devId) != 0)
        return -4007;
    ret = wc_AesXtsDecrypt(&aes, buf, cipher, sizeof(pp), i1, sizeof(i1));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4008;
    if (XMEMCMP(pp, buf, sizeof(pp)))
        return -4009;

    /* NIST decrypt test vector */
    XMEMSET(buf, 0, sizeof(buf));
    ret = wc_AesXtsDecrypt(&aes, buf, c1, sizeof(c1), i1, sizeof(i1));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4010;
    if (XMEMCMP(p1, buf, AES_BLOCK_SIZE))
        return -4011;

    /* fail case with decrypting using wrong key */
    XMEMSET(buf, 0, sizeof(buf));
    ret = wc_AesXtsDecrypt(&aes, buf, c2, sizeof(c2), i2, sizeof(i2));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4012;
    if (XMEMCMP(p2, buf, sizeof(p2)) == 0) /* fail case with wrong key */
        return -4013;

    /* set correct key and retest */
    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k2, sizeof(k2), AES_DECRYPTION,
            HEAP_HINT, devId) != 0)
        return -4014;
    ret = wc_AesXtsDecrypt(&aes, buf, c2, sizeof(c2), i2, sizeof(i2));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4015;
    if (XMEMCMP(p2, buf, sizeof(p2)))
        return -4016;
    wc_AesXtsFree(&aes);

    return ret;
}


static int aes_xts_256_test(void)
{
    XtsAes aes;
    int ret = 0;
    unsigned char buf[AES_BLOCK_SIZE * 3];
    unsigned char cipher[AES_BLOCK_SIZE * 3];

    /* 256 key tests */
    static unsigned char k1[] = {
        0x1e, 0xa6, 0x61, 0xc5, 0x8d, 0x94, 0x3a, 0x0e,
        0x48, 0x01, 0xe4, 0x2f, 0x4b, 0x09, 0x47, 0x14,
        0x9e, 0x7f, 0x9f, 0x8e, 0x3e, 0x68, 0xd0, 0xc7,
        0x50, 0x52, 0x10, 0xbd, 0x31, 0x1a, 0x0e, 0x7c,
        0xd6, 0xe1, 0x3f, 0xfd, 0xf2, 0x41, 0x8d, 0x8d,
        0x19, 0x11, 0xc0, 0x04, 0xcd, 0xa5, 0x8d, 0xa3,
        0xd6, 0x19, 0xb7, 0xe2, 0xb9, 0x14, 0x1e, 0x58,
        0x31, 0x8e, 0xea, 0x39, 0x2c, 0xf4, 0x1b, 0x08
    };

    static unsigned char i1[] = {
        0xad, 0xf8, 0xd9, 0x26, 0x27, 0x46, 0x4a, 0xd2,
        0xf0, 0x42, 0x8e, 0x84, 0xa9, 0xf8, 0x75, 0x64
    };

    static unsigned char p1[] = {
        0x2e, 0xed, 0xea, 0x52, 0xcd, 0x82, 0x15, 0xe1,
        0xac, 0xc6, 0x47, 0xe8, 0x10, 0xbb, 0xc3, 0x64,
        0x2e, 0x87, 0x28, 0x7f, 0x8d, 0x2e, 0x57, 0xe3,
        0x6c, 0x0a, 0x24, 0xfb, 0xc1, 0x2a, 0x20, 0x2e
    };

    /* plain text test of partial block is not from NIST test vector list */
    static unsigned char pp[] = {
        0xeb, 0xab, 0xce, 0x95, 0xb1, 0x4d, 0x3c, 0x8d,
        0x6f, 0xb3, 0x50, 0x39, 0x07, 0x90, 0x31, 0x1c,
        0x6e, 0x4b, 0x92, 0x01, 0x3e, 0x76, 0x8a, 0xd5
    };

    static unsigned char c1[] = {
        0xcb, 0xaa, 0xd0, 0xe2, 0xf6, 0xce, 0xa3, 0xf5,
        0x0b, 0x37, 0xf9, 0x34, 0xd4, 0x6a, 0x9b, 0x13,
        0x0b, 0x9d, 0x54, 0xf0, 0x7e, 0x34, 0xf3, 0x6a,
        0xf7, 0x93, 0xe8, 0x6f, 0x73, 0xc6, 0xd7, 0xdb
    };

    static unsigned char k2[] = {
        0xad, 0x50, 0x4b, 0x85, 0xd7, 0x51, 0xbf, 0xba,
        0x69, 0x13, 0xb4, 0xcc, 0x79, 0xb6, 0x5a, 0x62,
        0xf7, 0xf3, 0x9d, 0x36, 0x0f, 0x35, 0xb5, 0xec,
        0x4a, 0x7e, 0x95, 0xbd, 0x9b, 0xa5, 0xf2, 0xec,
        0xc1, 0xd7, 0x7e, 0xa3, 0xc3, 0x74, 0xbd, 0x4b,
        0x13, 0x1b, 0x07, 0x83, 0x87, 0xdd, 0x55, 0x5a,
        0xb5, 0xb0, 0xc7, 0xe5, 0x2d, 0xb5, 0x06, 0x12,
        0xd2, 0xb5, 0x3a, 0xcb, 0x47, 0x8a, 0x53, 0xb4
    };

    static unsigned char i2[] = {
        0xe6, 0x42, 0x19, 0xed, 0xe0, 0xe1, 0xc2, 0xa0,
        0x0e, 0xf5, 0x58, 0x6a, 0xc4, 0x9b, 0xeb, 0x6f
    };

    static unsigned char p2[] = {
        0x24, 0xcb, 0x76, 0x22, 0x55, 0xb5, 0xa8, 0x00,
        0xf4, 0x6e, 0x80, 0x60, 0x56, 0x9e, 0x05, 0x53,
        0xbc, 0xfe, 0x86, 0x55, 0x3b, 0xca, 0xd5, 0x89,
        0xc7, 0x54, 0x1a, 0x73, 0xac, 0xc3, 0x9a, 0xbd,
        0x53, 0xc4, 0x07, 0x76, 0xd8, 0xe8, 0x22, 0x61,
        0x9e, 0xa9, 0xad, 0x77, 0xa0, 0x13, 0x4c, 0xfc
    };

    static unsigned char c2[] = {
        0xa3, 0xc6, 0xf3, 0xf3, 0x82, 0x79, 0x5b, 0x10,
        0x87, 0xd7, 0x02, 0x50, 0xdb, 0x2c, 0xd3, 0xb1,
        0xa1, 0x62, 0xa8, 0xb6, 0xdc, 0x12, 0x60, 0x61,
        0xc1, 0x0a, 0x84, 0xa5, 0x85, 0x3f, 0x3a, 0x89,
        0xe6, 0x6c, 0xdb, 0xb7, 0x9a, 0xb4, 0x28, 0x9b,
        0xc3, 0xea, 0xd8, 0x10, 0xe9, 0xc0, 0xaf, 0x92
    };

    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k2, sizeof(k2), AES_ENCRYPTION,
            HEAP_HINT, devId) != 0)
        return -4017;
    ret = wc_AesXtsEncrypt(&aes, buf, p2, sizeof(p2), i2, sizeof(i2));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4018;
    if (XMEMCMP(c2, buf, sizeof(c2)))
        return -4019;

    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k1, sizeof(k1), AES_ENCRYPTION,
            HEAP_HINT, devId) != 0)
        return -4020;
    ret = wc_AesXtsEncrypt(&aes, buf, p1, sizeof(p1), i1, sizeof(i1));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4021;
    if (XMEMCMP(c1, buf, AES_BLOCK_SIZE))
        return -4022;

    /* partial block encryption test */
    XMEMSET(cipher, 0, sizeof(cipher));
    ret = wc_AesXtsEncrypt(&aes, cipher, pp, sizeof(pp), i1, sizeof(i1));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4023;
    wc_AesXtsFree(&aes);

    /* partial block decrypt test */
    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k1, sizeof(k1), AES_DECRYPTION,
            HEAP_HINT, devId) != 0)
        return -4024;
    ret = wc_AesXtsDecrypt(&aes, buf, cipher, sizeof(pp), i1, sizeof(i1));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4025;
    if (XMEMCMP(pp, buf, sizeof(pp)))
        return -4026;

    /* NIST decrypt test vector */
    XMEMSET(buf, 0, sizeof(buf));
    ret = wc_AesXtsDecrypt(&aes, buf, c1, sizeof(c1), i1, sizeof(i1));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4027;
    if (XMEMCMP(p1, buf, AES_BLOCK_SIZE))
        return -4028;

    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k2, sizeof(k2), AES_DECRYPTION,
            HEAP_HINT, devId) != 0)
        return -4029;
    ret = wc_AesXtsDecrypt(&aes, buf, c2, sizeof(c2), i2, sizeof(i2));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4030;
    if (XMEMCMP(p2, buf, sizeof(p2)))
        return -4031;
    wc_AesXtsFree(&aes);

    return ret;
}


/* both 128 and 256 bit key test */
static int aes_xts_sector_test(void)
{
    XtsAes aes;
    int ret = 0;
    unsigned char buf[AES_BLOCK_SIZE * 2];

    /* 128 key tests */
    static unsigned char k1[] = {
        0xa3, 0xe4, 0x0d, 0x5b, 0xd4, 0xb6, 0xbb, 0xed,
        0xb2, 0xd1, 0x8c, 0x70, 0x0a, 0xd2, 0xdb, 0x22,
        0x10, 0xc8, 0x11, 0x90, 0x64, 0x6d, 0x67, 0x3c,
        0xbc, 0xa5, 0x3f, 0x13, 0x3e, 0xab, 0x37, 0x3c
    };

    static unsigned char p1[] = {
        0x20, 0xe0, 0x71, 0x94, 0x05, 0x99, 0x3f, 0x09,
        0xa6, 0x6a, 0xe5, 0xbb, 0x50, 0x0e, 0x56, 0x2c
    };

    static unsigned char c1[] = {
        0x74, 0x62, 0x35, 0x51, 0x21, 0x02, 0x16, 0xac,
        0x92, 0x6b, 0x96, 0x50, 0xb6, 0xd3, 0xfa, 0x52
    };
    word64 s1 = 141;

    /* 256 key tests */
    static unsigned char k2[] = {
        0xef, 0x01, 0x0c, 0xa1, 0xa3, 0x66, 0x3e, 0x32,
        0x53, 0x43, 0x49, 0xbc, 0x0b, 0xae, 0x62, 0x23,
        0x2a, 0x15, 0x73, 0x34, 0x85, 0x68, 0xfb, 0x9e,
        0xf4, 0x17, 0x68, 0xa7, 0x67, 0x4f, 0x50, 0x7a,
        0x72, 0x7f, 0x98, 0x75, 0x53, 0x97, 0xd0, 0xe0,
        0xaa, 0x32, 0xf8, 0x30, 0x33, 0x8c, 0xc7, 0xa9,
        0x26, 0xc7, 0x73, 0xf0, 0x9e, 0x57, 0xb3, 0x57,
        0xcd, 0x15, 0x6a, 0xfb, 0xca, 0x46, 0xe1, 0xa0
    };

    static unsigned char p2[] = {
        0xed, 0x98, 0xe0, 0x17, 0x70, 0xa8, 0x53, 0xb4,
        0x9d, 0xb9, 0xe6, 0xaa, 0xf8, 0x8f, 0x0a, 0x41,
        0xb9, 0xb5, 0x6e, 0x91, 0xa5, 0xa2, 0xb1, 0x1d,
        0x40, 0x52, 0x92, 0x54, 0xf5, 0x52, 0x3e, 0x75
    };

    static unsigned char c2[] = {
        0xca, 0x20, 0xc5, 0x5e, 0x8d, 0xc1, 0x49, 0x68,
        0x7d, 0x25, 0x41, 0xde, 0x39, 0xc3, 0xdf, 0x63,
        0x00, 0xbb, 0x5a, 0x16, 0x3c, 0x10, 0xce, 0xd3,
        0x66, 0x6b, 0x13, 0x57, 0xdb, 0x8b, 0xd3, 0x9d
    };
    word64 s2 = 187;

    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k1, sizeof(k1), AES_ENCRYPTION,
            HEAP_HINT, devId) != 0)
        return -4032;
    ret = wc_AesXtsEncryptSector(&aes, buf, p1, sizeof(p1), s1);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4033;
    if (XMEMCMP(c1, buf, AES_BLOCK_SIZE))
        return -4034;

    /* decrypt test */
    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k1, sizeof(k1), AES_DECRYPTION,
            HEAP_HINT, devId) != 0)
        return -4035;
    ret = wc_AesXtsDecryptSector(&aes, buf, c1, sizeof(c1), s1);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4036;
    if (XMEMCMP(p1, buf, AES_BLOCK_SIZE))
        return -4037;
    wc_AesXtsFree(&aes);

    /* 256 bit key tests */
    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k2, sizeof(k2), AES_ENCRYPTION,
            HEAP_HINT, devId) != 0)
        return -4038;
    ret = wc_AesXtsEncryptSector(&aes, buf, p2, sizeof(p2), s2);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4039;
    if (XMEMCMP(c2, buf, sizeof(c2)))
        return -4040;

    /* decrypt test */
    XMEMSET(buf, 0, sizeof(buf));
    if (wc_AesXtsSetKey(&aes, k2, sizeof(k2), AES_DECRYPTION,
            HEAP_HINT, devId) != 0)
        return -4041;
    ret = wc_AesXtsDecryptSector(&aes, buf, c2, sizeof(c2), s2);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4042;
    if (XMEMCMP(p2, buf, sizeof(p2)))
        return -4043;
    wc_AesXtsFree(&aes);

    return ret;
}


/* testing of bad arguments */
static int aes_xts_args_test(void)
{
    XtsAes aes;
    int ret = 0;
    unsigned char buf[AES_BLOCK_SIZE * 2];

    /* 128 key tests */
    static unsigned char k1[] = {
        0xa3, 0xe4, 0x0d, 0x5b, 0xd4, 0xb6, 0xbb, 0xed,
        0xb2, 0xd1, 0x8c, 0x70, 0x0a, 0xd2, 0xdb, 0x22,
        0x10, 0xc8, 0x11, 0x90, 0x64, 0x6d, 0x67, 0x3c,
        0xbc, 0xa5, 0x3f, 0x13, 0x3e, 0xab, 0x37, 0x3c
    };

    static unsigned char p1[] = {
        0x20, 0xe0, 0x71, 0x94, 0x05, 0x99, 0x3f, 0x09,
        0xa6, 0x6a, 0xe5, 0xbb, 0x50, 0x0e, 0x56, 0x2c
    };

    static unsigned char c1[] = {
        0x74, 0x62, 0x35, 0x51, 0x21, 0x02, 0x16, 0xac,
        0x92, 0x6b, 0x96, 0x50, 0xb6, 0xd3, 0xfa, 0x52
    };
    word64 s1 = 141;

    if (wc_AesXtsSetKey(NULL, k1, sizeof(k1), AES_ENCRYPTION,
            HEAP_HINT, devId) == 0)
        return -4044;
    if (wc_AesXtsSetKey(&aes, NULL, sizeof(k1), AES_ENCRYPTION,
            HEAP_HINT, devId) == 0)
        return -4045;

    /* encryption operations */
    if (wc_AesXtsSetKey(&aes, k1, sizeof(k1), AES_ENCRYPTION,
            HEAP_HINT, devId) != 0)
        return -4046;
    ret = wc_AesXtsEncryptSector(NULL, buf, p1, sizeof(p1), s1);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret == 0)
        return -4047;

    ret = wc_AesXtsEncryptSector(&aes, NULL, p1, sizeof(p1), s1);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret == 0)
        return -4048;
    wc_AesXtsFree(&aes);

    /* decryption operations */
    if (wc_AesXtsSetKey(&aes, k1, sizeof(k1), AES_DECRYPTION,
            HEAP_HINT, devId) != 0)
        return -4046;
    ret = wc_AesXtsDecryptSector(NULL, buf, c1, sizeof(c1), s1);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret == 0)
        return -4049;

    ret = wc_AesXtsDecryptSector(&aes, NULL, c1, sizeof(c1), s1);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &aes.aes.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret == 0)
        return -4050;
    wc_AesXtsFree(&aes);

    return 0;
}
#endif /* WOLFSSL_AES_XTS */

#if defined(HAVE_AES_CBC)
static int aes_cbc_test(void)
{
    byte cipher[AES_BLOCK_SIZE];
    byte plain[AES_BLOCK_SIZE];
    int  ret;
    const byte msg[] = { /* "Now is the time for all " w/o trailing 0 */
        0x6e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };
    byte key[] = "0123456789abcdef   ";  /* align */
    byte iv[]  = "1234567890abcdef   ";  /* align */

    /* Parameter Validation testing. */
    ret = wc_AesCbcEncryptWithKey(cipher, msg, AES_BLOCK_SIZE, key, 17, NULL);
    if (ret != BAD_FUNC_ARG)
        return -4100;
    ret = wc_AesCbcDecryptWithKey(plain, cipher, AES_BLOCK_SIZE, key, 17, NULL);
    if (ret != BAD_FUNC_ARG)
        return -4101;

    ret = wc_AesCbcEncryptWithKey(cipher, msg, AES_BLOCK_SIZE, key,
                                  AES_BLOCK_SIZE, iv);
    if (ret != 0)
        return -4102;
    ret = wc_AesCbcDecryptWithKey(plain, cipher, AES_BLOCK_SIZE, key,
                                  AES_BLOCK_SIZE, iv);
    if (ret != 0)
        return -4103;

    if (XMEMCMP(plain, msg, AES_BLOCK_SIZE) != 0)
        return -4104;

    return 0;
}
#endif

int aes_test(void)
{
#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_COUNTER)
    Aes enc;
    byte cipher[AES_BLOCK_SIZE * 4];
#if defined(HAVE_AES_DECRYPT) || defined(WOLFSSL_AES_COUNTER)
    Aes dec;
    byte plain [AES_BLOCK_SIZE * 4];
#endif
#endif /* HAVE_AES_CBC || WOLFSSL_AES_COUNTER */
    int  ret = 0;

#ifdef HAVE_AES_CBC
    const byte msg[] = { /* "Now is the time for all " w/o trailing 0 */
        0x6e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };

    const byte verify[] =
    {
        0x95,0x94,0x92,0x57,0x5f,0x42,0x81,0x53,
        0x2c,0xcc,0x9d,0x46,0x77,0xa2,0x33,0xcb
    };

    byte key[] = "0123456789abcdef   ";  /* align */
    byte iv[]  = "1234567890abcdef   ";  /* align */

#ifdef WOLFSSL_ASYNC_CRYPT
    if (wc_AesInit(&enc, HEAP_HINT, devId) != 0)
        return -4200;
    if (wc_AesInit(&dec, HEAP_HINT, devId) != 0)
        return -4201;
#endif

    ret = wc_AesSetKey(&enc, key, AES_BLOCK_SIZE, iv, AES_ENCRYPTION);
    if (ret != 0)
        return -4202;
#ifdef HAVE_AES_DECRYPT
    ret = wc_AesSetKey(&dec, key, AES_BLOCK_SIZE, iv, AES_DECRYPTION);
    if (ret != 0)
        return -4203;
#endif

    ret = wc_AesCbcEncrypt(&enc, cipher, msg, AES_BLOCK_SIZE);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &enc.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4204;
#ifdef HAVE_AES_DECRYPT
    ret = wc_AesCbcDecrypt(&dec, plain, cipher, AES_BLOCK_SIZE);
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &dec.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4205;
if (XMEMCMP(plain, msg, AES_BLOCK_SIZE))
        return -4206;
#endif /* HAVE_AES_DECRYPT */
    if (XMEMCMP(cipher, verify, AES_BLOCK_SIZE))
        return -4207;

#if defined(WOLFSSL_AESNI) && defined(HAVE_AES_DECRYPT)
    {
        const byte bigMsg[] = {
            /* "All work and no play makes Jack a dull boy. " */
            0x41,0x6c,0x6c,0x20,0x77,0x6f,0x72,0x6b,
            0x20,0x61,0x6e,0x64,0x20,0x6e,0x6f,0x20,
            0x70,0x6c,0x61,0x79,0x20,0x6d,0x61,0x6b,
            0x65,0x73,0x20,0x4a,0x61,0x63,0x6b,0x20,
            0x61,0x20,0x64,0x75,0x6c,0x6c,0x20,0x62,
            0x6f,0x79,0x2e,0x20,0x41,0x6c,0x6c,0x20,
            0x77,0x6f,0x72,0x6b,0x20,0x61,0x6e,0x64,
            0x20,0x6e,0x6f,0x20,0x70,0x6c,0x61,0x79,
            0x20,0x6d,0x61,0x6b,0x65,0x73,0x20,0x4a,
            0x61,0x63,0x6b,0x20,0x61,0x20,0x64,0x75,
            0x6c,0x6c,0x20,0x62,0x6f,0x79,0x2e,0x20,
            0x41,0x6c,0x6c,0x20,0x77,0x6f,0x72,0x6b,
            0x20,0x61,0x6e,0x64,0x20,0x6e,0x6f,0x20,
            0x70,0x6c,0x61,0x79,0x20,0x6d,0x61,0x6b,
            0x65,0x73,0x20,0x4a,0x61,0x63,0x6b,0x20,
            0x61,0x20,0x64,0x75,0x6c,0x6c,0x20,0x62,
            0x6f,0x79,0x2e,0x20,0x41,0x6c,0x6c,0x20,
            0x77,0x6f,0x72,0x6b,0x20,0x61,0x6e,0x64,
            0x20,0x6e,0x6f,0x20,0x70,0x6c,0x61,0x79,
            0x20,0x6d,0x61,0x6b,0x65,0x73,0x20,0x4a,
            0x61,0x63,0x6b,0x20,0x61,0x20,0x64,0x75,
            0x6c,0x6c,0x20,0x62,0x6f,0x79,0x2e,0x20,
            0x41,0x6c,0x6c,0x20,0x77,0x6f,0x72,0x6b,
            0x20,0x61,0x6e,0x64,0x20,0x6e,0x6f,0x20,
            0x70,0x6c,0x61,0x79,0x20,0x6d,0x61,0x6b,
            0x65,0x73,0x20,0x4a,0x61,0x63,0x6b,0x20,
            0x61,0x20,0x64,0x75,0x6c,0x6c,0x20,0x62,
            0x6f,0x79,0x2e,0x20,0x41,0x6c,0x6c,0x20,
            0x77,0x6f,0x72,0x6b,0x20,0x61,0x6e,0x64,
            0x20,0x6e,0x6f,0x20,0x70,0x6c,0x61,0x79,
            0x20,0x6d,0x61,0x6b,0x65,0x73,0x20,0x4a,
            0x61,0x63,0x6b,0x20,0x61,0x20,0x64,0x75,
            0x6c,0x6c,0x20,0x62,0x6f,0x79,0x2e,0x20,
            0x41,0x6c,0x6c,0x20,0x77,0x6f,0x72,0x6b,
            0x20,0x61,0x6e,0x64,0x20,0x6e,0x6f,0x20,
            0x70,0x6c,0x61,0x79,0x20,0x6d,0x61,0x6b,
            0x65,0x73,0x20,0x4a,0x61,0x63,0x6b,0x20,
            0x61,0x20,0x64,0x75,0x6c,0x6c,0x20,0x62,
            0x6f,0x79,0x2e,0x20,0x41,0x6c,0x6c,0x20,
            0x77,0x6f,0x72,0x6b,0x20,0x61,0x6e,0x64,
            0x20,0x6e,0x6f,0x20,0x70,0x6c,0x61,0x79,
            0x20,0x6d,0x61,0x6b,0x65,0x73,0x20,0x4a,
            0x61,0x63,0x6b,0x20,0x61,0x20,0x64,0x75,
            0x6c,0x6c,0x20,0x62,0x6f,0x79,0x2e,0x20,
            0x41,0x6c,0x6c,0x20,0x77,0x6f,0x72,0x6b,
            0x20,0x61,0x6e,0x64,0x20,0x6e,0x6f,0x20,
            0x70,0x6c,0x61,0x79,0x20,0x6d,0x61,0x6b,
            0x65,0x73,0x20,0x4a,0x61,0x63,0x6b,0x20
        };
        const byte bigKey[] = "0123456789abcdeffedcba9876543210";
        byte bigCipher[sizeof(bigMsg)];
        byte bigPlain[sizeof(bigMsg)];
        word32 keySz, msgSz;

        /* Iterate from one AES_BLOCK_SIZE of bigMsg through the whole
         * message by AES_BLOCK_SIZE for each size of AES key. */
        for (keySz = 16; keySz <= 32; keySz += 8) {
            for (msgSz = AES_BLOCK_SIZE;
                 msgSz <= sizeof(bigMsg);
                 msgSz += AES_BLOCK_SIZE) {

                XMEMSET(bigCipher, 0, sizeof(bigCipher));
                XMEMSET(bigPlain, 0, sizeof(bigPlain));
                ret = wc_AesSetKey(&enc, bigKey, keySz, iv, AES_ENCRYPTION);
                if (ret != 0)
                    return -4208;
                ret = wc_AesSetKey(&dec, bigKey, keySz, iv, AES_DECRYPTION);
                if (ret != 0)
                    return -4209;

                ret = wc_AesCbcEncrypt(&enc, bigCipher, bigMsg, msgSz);
            #if defined(WOLFSSL_ASYNC_CRYPT)
                ret = wc_AsyncWait(ret, &enc.asyncDev, WC_ASYNC_FLAG_NONE);
            #endif
                if (ret != 0)
                    return -4210;

                ret = wc_AesCbcDecrypt(&dec, bigPlain, bigCipher, msgSz);
            #if defined(WOLFSSL_ASYNC_CRYPT)
                ret = wc_AsyncWait(ret, &dec.asyncDev, WC_ASYNC_FLAG_NONE);
            #endif
                if (ret != 0)
                    return -4211;

                if (XMEMCMP(bigPlain, bigMsg, msgSz))
                    return -4212;
            }
        }
    }
#endif /* WOLFSSL_AESNI HAVE_AES_DECRYPT */

#endif /* HAVE_AES_CBC */

#ifdef WOLFSSL_AES_COUNTER
    {
        /* test vectors from "Recommendation for Block Cipher Modes of
         * Operation" NIST Special Publication 800-38A */

        const byte ctrIv[] =
        {
            0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,
            0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
        };

        const byte ctrPlain[] =
        {
            0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
            0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,
            0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,
            0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51,
            0x30,0xc8,0x1c,0x46,0xa3,0x5c,0xe4,0x11,
            0xe5,0xfb,0xc1,0x19,0x1a,0x0a,0x52,0xef,
            0xf6,0x9f,0x24,0x45,0xdf,0x4f,0x9b,0x17,
            0xad,0x2b,0x41,0x7b,0xe6,0x6c,0x37,0x10
        };

        const byte oddCipher[] =
        {
            0xb9,0xd7,0xcb,0x08,0xb0,0xe1,0x7b,0xa0,
            0xc2
        };

        const byte ctr128Key[] =
        {
            0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,
            0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c
        };

        const byte ctr128Cipher[] =
        {
            0x87,0x4d,0x61,0x91,0xb6,0x20,0xe3,0x26,
            0x1b,0xef,0x68,0x64,0x99,0x0d,0xb6,0xce,
            0x98,0x06,0xf6,0x6b,0x79,0x70,0xfd,0xff,
            0x86,0x17,0x18,0x7b,0xb9,0xff,0xfd,0xff,
            0x5a,0xe4,0xdf,0x3e,0xdb,0xd5,0xd3,0x5e,
            0x5b,0x4f,0x09,0x02,0x0d,0xb0,0x3e,0xab,
            0x1e,0x03,0x1d,0xda,0x2f,0xbe,0x03,0xd1,
            0x79,0x21,0x70,0xa0,0xf3,0x00,0x9c,0xee
        };

        const byte ctr192Key[] =
        {
            0x8e,0x73,0xb0,0xf7,0xda,0x0e,0x64,0x52,
            0xc8,0x10,0xf3,0x2b,0x80,0x90,0x79,0xe5,
            0x62,0xf8,0xea,0xd2,0x52,0x2c,0x6b,0x7b
        };

        const byte ctr192Cipher[] =
        {
            0x1a,0xbc,0x93,0x24,0x17,0x52,0x1c,0xa2,
            0x4f,0x2b,0x04,0x59,0xfe,0x7e,0x6e,0x0b,
            0x09,0x03,0x39,0xec,0x0a,0xa6,0xfa,0xef,
            0xd5,0xcc,0xc2,0xc6,0xf4,0xce,0x8e,0x94,
            0x1e,0x36,0xb2,0x6b,0xd1,0xeb,0xc6,0x70,
            0xd1,0xbd,0x1d,0x66,0x56,0x20,0xab,0xf7,
            0x4f,0x78,0xa7,0xf6,0xd2,0x98,0x09,0x58,
            0x5a,0x97,0xda,0xec,0x58,0xc6,0xb0,0x50
        };

        const byte ctr256Key[] =
        {
            0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
            0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
            0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
            0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4
        };

        const byte ctr256Cipher[] =
        {
            0x60,0x1e,0xc3,0x13,0x77,0x57,0x89,0xa5,
            0xb7,0xa7,0xf5,0x04,0xbb,0xf3,0xd2,0x28,
            0xf4,0x43,0xe3,0xca,0x4d,0x62,0xb5,0x9a,
            0xca,0x84,0xe9,0x90,0xca,0xca,0xf5,0xc5,
            0x2b,0x09,0x30,0xda,0xa2,0x3d,0xe9,0x4c,
            0xe8,0x70,0x17,0xba,0x2d,0x84,0x98,0x8d,
            0xdf,0xc9,0xc5,0x8d,0xb6,0x7a,0xad,0xa6,
            0x13,0xc2,0xdd,0x08,0x45,0x79,0x41,0xa6
        };

        wc_AesSetKeyDirect(&enc, ctr128Key, sizeof(ctr128Key),
                           ctrIv, AES_ENCRYPTION);
        /* Ctr only uses encrypt, even on key setup */
        wc_AesSetKeyDirect(&dec, ctr128Key, sizeof(ctr128Key),
                           ctrIv, AES_ENCRYPTION);

        ret = wc_AesCtrEncrypt(&enc, cipher, ctrPlain, sizeof(ctrPlain));
        if (ret != 0) {
            return -4227;
        }
        ret = wc_AesCtrEncrypt(&dec, plain, cipher, sizeof(ctrPlain));
        if (ret != 0) {
            return -4228;
        }
        if (XMEMCMP(plain, ctrPlain, sizeof(ctrPlain)))
            return -4213;

        if (XMEMCMP(cipher, ctr128Cipher, sizeof(ctr128Cipher)))
            return -4214;

        /* let's try with just 9 bytes, non block size test */
        wc_AesSetKeyDirect(&enc, ctr128Key, AES_BLOCK_SIZE,
                           ctrIv, AES_ENCRYPTION);
        /* Ctr only uses encrypt, even on key setup */
        wc_AesSetKeyDirect(&dec, ctr128Key, AES_BLOCK_SIZE,
                           ctrIv, AES_ENCRYPTION);

        ret = wc_AesCtrEncrypt(&enc, cipher, ctrPlain, sizeof(oddCipher));
        if (ret != 0) {
            return -4229;
        }
        ret = wc_AesCtrEncrypt(&dec, plain, cipher, sizeof(oddCipher));
        if (ret != 0) {
            return -4230;
        }

        if (XMEMCMP(plain, ctrPlain, sizeof(oddCipher)))
            return -4215;

        if (XMEMCMP(cipher, ctr128Cipher, sizeof(oddCipher)))
            return -4216;

        /* and an additional 9 bytes to reuse tmp left buffer */
        ret = wc_AesCtrEncrypt(&enc, cipher, ctrPlain, sizeof(oddCipher));
        if (ret != 0) {
            return -4231;
        }
        ret = wc_AesCtrEncrypt(&dec, plain, cipher, sizeof(oddCipher));
        if (ret != 0) {
            return -4232;
        }

        if (XMEMCMP(plain, ctrPlain, sizeof(oddCipher)))
            return -4217;

        if (XMEMCMP(cipher, oddCipher, sizeof(oddCipher)))
            return -4218;

        /* 192 bit key */
        wc_AesSetKeyDirect(&enc, ctr192Key, sizeof(ctr192Key),
                           ctrIv, AES_ENCRYPTION);
        /* Ctr only uses encrypt, even on key setup */
        wc_AesSetKeyDirect(&dec, ctr192Key, sizeof(ctr192Key),
                           ctrIv, AES_ENCRYPTION);

        XMEMSET(plain, 0, sizeof(plain));
        ret = wc_AesCtrEncrypt(&enc, plain, ctr192Cipher, sizeof(ctr192Cipher));
        if (ret != 0) {
            return -4233;
        }

        if (XMEMCMP(plain, ctrPlain, sizeof(ctr192Cipher)))
            return -4219;

        ret = wc_AesCtrEncrypt(&dec, cipher, ctrPlain, sizeof(ctrPlain));
        if (ret != 0) {
            return -4234;
        }
        if (XMEMCMP(ctr192Cipher, cipher, sizeof(ctr192Cipher)))
            return -4220;

        /* 256 bit key */
        wc_AesSetKeyDirect(&enc, ctr256Key, sizeof(ctr256Key),
                           ctrIv, AES_ENCRYPTION);
        /* Ctr only uses encrypt, even on key setup */
        wc_AesSetKeyDirect(&dec, ctr256Key, sizeof(ctr256Key),
                           ctrIv, AES_ENCRYPTION);

        XMEMSET(plain, 0, sizeof(plain));
        ret = wc_AesCtrEncrypt(&enc, plain, ctr256Cipher, sizeof(ctr256Cipher));
        if (ret != 0) {
            return -4235;
        }

        if (XMEMCMP(plain, ctrPlain, sizeof(ctrPlain)))
            return -4221;

        ret = wc_AesCtrEncrypt(&dec, cipher, ctrPlain, sizeof(ctrPlain));
        if (ret != 0) {
            return -4236;
        }
        if (XMEMCMP(ctr256Cipher, cipher, sizeof(ctr256Cipher)))
            return -4222;
    }
#endif /* WOLFSSL_AES_COUNTER */

#ifdef WOLFSSL_AES_DIRECT
    {
        const byte niPlain[] =
        {
            0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
            0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a
        };

        const byte niCipher[] =
        {
            0xf3,0xee,0xd1,0xbd,0xb5,0xd2,0xa0,0x3c,
            0x06,0x4b,0x5a,0x7e,0x3d,0xb1,0x81,0xf8
        };

        const byte niKey[] =
        {
            0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
            0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
            0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
            0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4
        };

        XMEMSET(cipher, 0, AES_BLOCK_SIZE);
        ret = wc_AesSetKey(&enc, niKey, sizeof(niKey), cipher, AES_ENCRYPTION);
        if (ret != 0)
            return -4223;
        wc_AesEncryptDirect(&enc, cipher, niPlain);
        if (XMEMCMP(cipher, niCipher, AES_BLOCK_SIZE) != 0)
            return -4224;

        XMEMSET(plain, 0, AES_BLOCK_SIZE);
        ret = wc_AesSetKey(&dec, niKey, sizeof(niKey), plain, AES_DECRYPTION);
        if (ret != 0)
            return -4225;
        wc_AesDecryptDirect(&dec, plain, niCipher);
        if (XMEMCMP(plain, niPlain, AES_BLOCK_SIZE) != 0)
            return -4226;
    }
#endif /* WOLFSSL_AES_DIRECT */

    ret = aes_key_size_test();
    if (ret != 0)
        return ret;

#if defined(HAVE_AES_CBC)
    ret = aes_cbc_test();
    if (ret != 0)
        return ret;
#endif

#if defined(WOLFSSL_AES_XTS)
    ret = aes_xts_128_test();
    if (ret != 0)
        return ret;
    ret = aes_xts_256_test();
    if (ret != 0)
        return ret;
    ret = aes_xts_sector_test();
    if (ret != 0)
        return ret;
    ret = aes_xts_args_test();
    if (ret != 0)
        return ret;
#endif

    wc_AesFree(&enc);
#ifdef HAVE_AES_DECRYPT
    wc_AesFree(&dec);
#endif

    return ret;
}

int aes192_test(void)
{
#ifdef HAVE_AES_CBC
    Aes enc;
    byte cipher[AES_BLOCK_SIZE];
#ifdef HAVE_AES_DECRYPT
    Aes dec;
    byte plain[AES_BLOCK_SIZE];
#endif
#endif /* HAVE_AES_CBC */
    int  ret = 0;

#ifdef HAVE_AES_CBC
    /* Test vectors from NIST Special Publication 800-38A, 2001 Edition
     * Appendix F.2.3  */

    const byte msg[] = {
        0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
        0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a
    };

    const byte verify[] =
    {
        0x4f,0x02,0x1d,0xb2,0x43,0xbc,0x63,0x3d,
        0x71,0x78,0x18,0x3a,0x9f,0xa0,0x71,0xe8
    };

    byte key[] = {
        0x8e,0x73,0xb0,0xf7,0xda,0x0e,0x64,0x52,
        0xc8,0x10,0xf3,0x2b,0x80,0x90,0x79,0xe5,
        0x62,0xf8,0xea,0xd2,0x52,0x2c,0x6b,0x7b
    };
    byte iv[]  = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
    };


    if (wc_AesInit(&enc, HEAP_HINT, devId) != 0)
        return -4230;
#ifdef HAVE_AES_DECRYPT
    if (wc_AesInit(&dec, HEAP_HINT, devId) != 0)
        return -4231;
#endif

    ret = wc_AesSetKey(&enc, key, (int) sizeof(key), iv, AES_ENCRYPTION);
    if (ret != 0)
        return -4232;
#ifdef HAVE_AES_DECRYPT
    ret = wc_AesSetKey(&dec, key, (int) sizeof(key), iv, AES_DECRYPTION);
    if (ret != 0)
        return -4233;
#endif

    ret = wc_AesCbcEncrypt(&enc, cipher, msg, (int) sizeof(msg));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &enc.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4234;
#ifdef HAVE_AES_DECRYPT
    ret = wc_AesCbcDecrypt(&dec, plain, cipher, (int) sizeof(cipher));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &dec.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4235;
    if (XMEMCMP(plain, msg, (int) sizeof(plain))) {
        return -4236;
    }
#endif

    if (XMEMCMP(cipher, verify, (int) sizeof(cipher)))
        return -4237;

    wc_AesFree(&enc);
#ifdef HAVE_AES_DECRYPT
    wc_AesFree(&dec);
#endif

#endif /* HAVE_AES_CBC */

    return ret;
}

int aes256_test(void)
{
#ifdef HAVE_AES_CBC
    Aes enc;
    byte cipher[AES_BLOCK_SIZE];
#ifdef HAVE_AES_DECRYPT
    Aes dec;
    byte plain[AES_BLOCK_SIZE];
#endif
#endif /* HAVE_AES_CBC */
    int  ret = 0;

#ifdef HAVE_AES_CBC
    /* Test vectors from NIST Special Publication 800-38A, 2001 Edition,
     * Appendix F.2.5  */
    const byte msg[] = {
        0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
        0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a
    };

    const byte verify[] =
    {
        0xf5,0x8c,0x4c,0x04,0xd6,0xe5,0xf1,0xba,
        0x77,0x9e,0xab,0xfb,0x5f,0x7b,0xfb,0xd6
    };

    byte key[] = {
        0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
        0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
        0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
        0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4
    };
    byte iv[]  = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
    };


    if (wc_AesInit(&enc, HEAP_HINT, devId) != 0)
        return -4240;
#ifdef HAVE_AES_DECRYPT
    if (wc_AesInit(&dec, HEAP_HINT, devId) != 0)
        return -4241;
#endif

    ret = wc_AesSetKey(&enc, key, (int) sizeof(key), iv, AES_ENCRYPTION);
    if (ret != 0)
        return -4242;
#ifdef HAVE_AES_DECRYPT
    ret = wc_AesSetKey(&dec, key, (int) sizeof(key), iv, AES_DECRYPTION);
    if (ret != 0)
        return -4243;
#endif

    ret = wc_AesCbcEncrypt(&enc, cipher, msg, (int) sizeof(msg));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &enc.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4244;
#ifdef HAVE_AES_DECRYPT
    ret = wc_AesCbcDecrypt(&dec, plain, cipher, (int) sizeof(cipher));
#if defined(WOLFSSL_ASYNC_CRYPT)
    ret = wc_AsyncWait(ret, &dec.asyncDev, WC_ASYNC_FLAG_NONE);
#endif
    if (ret != 0)
        return -4245;
    if (XMEMCMP(plain, msg, (int) sizeof(plain))) {
        return -4246;
    }
#endif

    if (XMEMCMP(cipher, verify, (int) sizeof(cipher)))
        return -4247;

    wc_AesFree(&enc);
#ifdef HAVE_AES_DECRYPT
    wc_AesFree(&dec);
#endif

#endif /* HAVE_AES_CBC */

    return 0;
}


#ifndef WC_NO_RNG
static int random_rng_test(void)
{
    WC_RNG rng;
    byte block[32];
    int ret, i;

#ifndef HAVE_FIPS
    ret = wc_InitRng_ex(&rng, HEAP_HINT, devId);
#else
    ret = wc_InitRng(&rng);
#endif
    if (ret != 0) return -4900;

    XMEMSET(block, 0, sizeof(block));

    ret = wc_RNG_GenerateBlock(&rng, block, sizeof(block));
    if (ret != 0) {
        ret = -4901;
        goto exit;
    }

    /* Check for 0's */
    for (i=0; i<(int)sizeof(block); i++) {
        if (block[i] == 0) {
            ret++;
        }
    }
    /* All zeros count check */
    if (ret >= (int)sizeof(block)) {
        ret = -4902;
        goto exit;
    }

    ret = wc_RNG_GenerateByte(&rng, block);
    if (ret != 0) {
        ret = -4903;
        goto exit;
    }

exit:
    /* Make sure and free RNG */
    wc_FreeRng(&rng);

    return ret;
}

#if defined(HAVE_HASHDRBG) && !defined(CUSTOM_RAND_GENERATE_BLOCK)

int random_test(void)
{
    const byte test1Entropy[] =
    {
        0xa6, 0x5a, 0xd0, 0xf3, 0x45, 0xdb, 0x4e, 0x0e, 0xff, 0xe8, 0x75, 0xc3,
        0xa2, 0xe7, 0x1f, 0x42, 0xc7, 0x12, 0x9d, 0x62, 0x0f, 0xf5, 0xc1, 0x19,
        0xa9, 0xef, 0x55, 0xf0, 0x51, 0x85, 0xe0, 0xfb, 0x85, 0x81, 0xf9, 0x31,
        0x75, 0x17, 0x27, 0x6e, 0x06, 0xe9, 0x60, 0x7d, 0xdb, 0xcb, 0xcc, 0x2e
    };
    const byte test1Output[] =
    {
        0xd3, 0xe1, 0x60, 0xc3, 0x5b, 0x99, 0xf3, 0x40, 0xb2, 0x62, 0x82, 0x64,
        0xd1, 0x75, 0x10, 0x60, 0xe0, 0x04, 0x5d, 0xa3, 0x83, 0xff, 0x57, 0xa5,
        0x7d, 0x73, 0xa6, 0x73, 0xd2, 0xb8, 0xd8, 0x0d, 0xaa, 0xf6, 0xa6, 0xc3,
        0x5a, 0x91, 0xbb, 0x45, 0x79, 0xd7, 0x3f, 0xd0, 0xc8, 0xfe, 0xd1, 0x11,
        0xb0, 0x39, 0x13, 0x06, 0x82, 0x8a, 0xdf, 0xed, 0x52, 0x8f, 0x01, 0x81,
        0x21, 0xb3, 0xfe, 0xbd, 0xc3, 0x43, 0xe7, 0x97, 0xb8, 0x7d, 0xbb, 0x63,
        0xdb, 0x13, 0x33, 0xde, 0xd9, 0xd1, 0xec, 0xe1, 0x77, 0xcf, 0xa6, 0xb7,
        0x1f, 0xe8, 0xab, 0x1d, 0xa4, 0x66, 0x24, 0xed, 0x64, 0x15, 0xe5, 0x1c,
        0xcd, 0xe2, 0xc7, 0xca, 0x86, 0xe2, 0x83, 0x99, 0x0e, 0xea, 0xeb, 0x91,
        0x12, 0x04, 0x15, 0x52, 0x8b, 0x22, 0x95, 0x91, 0x02, 0x81, 0xb0, 0x2d,
        0xd4, 0x31, 0xf4, 0xc9, 0xf7, 0x04, 0x27, 0xdf
    };
    const byte test2EntropyA[] =
    {
        0x63, 0x36, 0x33, 0x77, 0xe4, 0x1e, 0x86, 0x46, 0x8d, 0xeb, 0x0a, 0xb4,
        0xa8, 0xed, 0x68, 0x3f, 0x6a, 0x13, 0x4e, 0x47, 0xe0, 0x14, 0xc7, 0x00,
        0x45, 0x4e, 0x81, 0xe9, 0x53, 0x58, 0xa5, 0x69, 0x80, 0x8a, 0xa3, 0x8f,
        0x2a, 0x72, 0xa6, 0x23, 0x59, 0x91, 0x5a, 0x9f, 0x8a, 0x04, 0xca, 0x68
    };
    const byte test2EntropyB[] =
    {
        0xe6, 0x2b, 0x8a, 0x8e, 0xe8, 0xf1, 0x41, 0xb6, 0x98, 0x05, 0x66, 0xe3,
        0xbf, 0xe3, 0xc0, 0x49, 0x03, 0xda, 0xd4, 0xac, 0x2c, 0xdf, 0x9f, 0x22,
        0x80, 0x01, 0x0a, 0x67, 0x39, 0xbc, 0x83, 0xd3
    };
    const byte test2Output[] =
    {
        0x04, 0xee, 0xc6, 0x3b, 0xb2, 0x31, 0xdf, 0x2c, 0x63, 0x0a, 0x1a, 0xfb,
        0xe7, 0x24, 0x94, 0x9d, 0x00, 0x5a, 0x58, 0x78, 0x51, 0xe1, 0xaa, 0x79,
        0x5e, 0x47, 0x73, 0x47, 0xc8, 0xb0, 0x56, 0x62, 0x1c, 0x18, 0xbd, 0xdc,
        0xdd, 0x8d, 0x99, 0xfc, 0x5f, 0xc2, 0xb9, 0x20, 0x53, 0xd8, 0xcf, 0xac,
        0xfb, 0x0b, 0xb8, 0x83, 0x12, 0x05, 0xfa, 0xd1, 0xdd, 0xd6, 0xc0, 0x71,
        0x31, 0x8a, 0x60, 0x18, 0xf0, 0x3b, 0x73, 0xf5, 0xed, 0xe4, 0xd4, 0xd0,
        0x71, 0xf9, 0xde, 0x03, 0xfd, 0x7a, 0xea, 0x10, 0x5d, 0x92, 0x99, 0xb8,
        0xaf, 0x99, 0xaa, 0x07, 0x5b, 0xdb, 0x4d, 0xb9, 0xaa, 0x28, 0xc1, 0x8d,
        0x17, 0x4b, 0x56, 0xee, 0x2a, 0x01, 0x4d, 0x09, 0x88, 0x96, 0xff, 0x22,
        0x82, 0xc9, 0x55, 0xa8, 0x19, 0x69, 0xe0, 0x69, 0xfa, 0x8c, 0xe0, 0x07,
        0xa1, 0x80, 0x18, 0x3a, 0x07, 0xdf, 0xae, 0x17
    };

    byte output[WC_SHA256_DIGEST_SIZE * 4];
    int ret;

    ret = wc_RNG_HealthTest(0, test1Entropy, sizeof(test1Entropy), NULL, 0,
                            output, sizeof(output));
    if (ret != 0)
        return -5000;

    if (XMEMCMP(test1Output, output, sizeof(output)) != 0)
        return -5001;

    ret = wc_RNG_HealthTest(1, test2EntropyA, sizeof(test2EntropyA),
                            test2EntropyB, sizeof(test2EntropyB),
                            output, sizeof(output));
    if (ret != 0)
        return -5002;

    if (XMEMCMP(test2Output, output, sizeof(output)) != 0)
        return -5003;

    /* Basic RNG generate block test */
    if (random_rng_test() != 0)
        return -5004;

    return 0;
}

#else

int random_test(void)
{
    /* Basic RNG generate block test */
    return random_rng_test();
}

#endif /* HAVE_HASHDRBG && !CUSTOM_RAND_GENERATE_BLOCK */
#endif /* WC_NO_RNG */


