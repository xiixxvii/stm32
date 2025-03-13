/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "mbedtls/sha1.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include "mbedtls/platform.h"


#include "string.h"
#include <stdint.h>
#include <stdlib.h>

/* USER CODE END Includes */

#include "framework.h"



// aes����
// aes_key -- �48�ֽ�
// iv -- �16�ֽ�
// plaintext -- �������ı�
// ciphertext -- ���ܵõ����ı�
// len -- len should be 16*N bytes
// return �C 0 for ok, else for error
//len should be 16*N bytes
int aes_cbc_encryp(uint8_t *aes_key, uint8_t *iv, uint8_t *plaintext, uint8_t *ciphertext, uint32_t len) 
{
    int i;
    int blk = (len + 15) >> 4;
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    //setkey_dec
    mbedtls_aes_setkey_enc(&aes_ctx, aes_key, 128);
    for (i = 0; i < blk; ++i) {
        mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, 16, iv, plaintext + (i * 16), ciphertext + (i * 16));
    }
    mbedtls_aes_free(&aes_ctx);
    return 0; //OK
}

// aes����
// aes_key -- �48�ֽ�
// iv -- �16�ֽ�
// ciphertext -- �����ܵ��ı�
// plaintext -- ���ܺõ��ı�
// len -- len should be 16*N bytes
// return �C 0 for ok, else for error
//len should be 16*N bytes
int aes_cbc_decryp(uint8_t *aes_key, uint8_t *iv, uint8_t *ciphertext, uint8_t *plaintext, uint32_t len) 
{
    int i;
    int blk = (len + 15) >> 4;
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    //setkey_dec
    mbedtls_aes_setkey_dec(&aes_ctx, aes_key, 128);
    for (i = 0; i < blk; ++i) {
        mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, 16, iv, ciphertext + (i * 16), plaintext + (i * 16));
    }
    mbedtls_aes_free(&aes_ctx);
    return 0; //OK
}


