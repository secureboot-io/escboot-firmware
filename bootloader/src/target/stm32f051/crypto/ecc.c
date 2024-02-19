#include "crypto/ecc.h"
#include <stdio.h>
#include <string.h>
#include "cmox_crypto.h"
#include "crypto/entropy.h"
#include "utils/hexdump.h"
#include "ecc/cmox_ecdsa.h"

cmox_ecc_handle_t Ecc_Ctx;
uint8_t Working_Buffer[2000];

bool crypto_ecc_generate(uint8_t *privateKey, size_t privateKeyLength, uint8_t *publicKey, size_t publicKeyLength)
{
    printf("Generating ECC keypair\n");
    uint8_t rand[32];
    entropyGather(256, rand);
    cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));
    cmox_ecc_retval_t retval = cmox_ecdsa_keyGen(&Ecc_Ctx, CMOX_ECC_SECP256K1_HIGHMEM, rand, 32, privateKey, &privateKeyLength, publicKey, &publicKeyLength);

    if(retval != CMOX_ECC_SUCCESS)
    {
        printf("cmox_ecdsa_keyGen failed, ret=%d\n", (int)retval);
        return false;
    }

    cmox_ecc_cleanup(&Ecc_Ctx);
    return true;
}

bool crypto_ecc_startup()
{
    if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    {
        return false;
    }
    return true;
}

bool crypto_ecc_verify(uint8_t *data, size_t dataLength, uint8_t *signature, size_t signatureLength, uint8_t *publicKey, size_t publicKeyLength)
{
    // printf("Verifying ECC signature\n");
    uint8_t hash[CMOX_SHA256_SIZE];
    uint32_t faultCheck;
    size_t computed_size;

    cmox_hash_retval_t hretval = cmox_hash_compute(CMOX_SHA256_ALGO, data, dataLength, hash, CMOX_SHA256_SIZE, &computed_size);
    if(hretval != CMOX_HASH_SUCCESS)
    {
        printf("cmox_hash_compute failed, ret=%d\n", (int)hretval);
        return false;
    }

    cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));

    cmox_ecc_retval_t retval = cmox_ecdsa_verify(&Ecc_Ctx, CMOX_ECC_CURVE_SECP256K1, publicKey, publicKeyLength, hash, CMOX_SHA256_SIZE, signature, signatureLength, &faultCheck);

    cmox_ecc_cleanup(&Ecc_Ctx);
    if(retval != CMOX_ECC_AUTH_SUCCESS)
    {
        printf("cmox_ecdsa_verify failed, ret=%d\n", (int)retval);
        return false;
    }

    if(faultCheck != CMOX_ECC_AUTH_SUCCESS)
    {
        printf("Fault check failed\n");
        return false;
    }
    
    return true;
}

void crypto_ecc_cleanup()
{
    cmox_ecc_cleanup(&Ecc_Ctx);
}