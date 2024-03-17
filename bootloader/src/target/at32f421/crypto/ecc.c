// #include "crypto/ecc.h"
// #include <stdio.h>
// #include <string.h>
// // #include "cmox_crypto.h"
// #include "crypto/entropy.h"
// // #include "utils/hexdump.h"
// // #include "ecc/cmox_ecdsa.h"
// #include "debug/logging.h"
// #include "uECC.h"

// // cmox_ecc_handle_t Ecc_Ctx;
// // uint8_t Working_Buffer[2000];

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "debug/logging.h"
#include "sha256.h"
#include "crypto/entropy.h"
#include "uECC.h"

static int RNG(uint8_t *dest, unsigned size) {
    LOG_DEBUG("RNG size=%d", size);
    entropyGather(size * 8, dest);
    return 1;
}

bool cryptoGenerateECCKeyPair(uint8_t *privateKey, size_t privateKeyLength, uint8_t *publicKey, size_t publicKeyLength)
{
    // printf("Generating ECC keypair\n");
    // uint8_t rand[32];
    
    // if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    // {
    //     LOG_DEBUG("cmox_initialize failed");
    //     return false;
    // }

    // entropyGather(256, rand);

    const struct uECC_Curve_t * curve = uECC_secp256k1();
    uECC_set_rng(&RNG);
    uECC_make_key(publicKey, privateKey, curve);


    // cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));
    // cmox_ecc_retval_t retval = cmox_ecdsa_keyGen(&Ecc_Ctx, CMOX_ECC_SECP256K1_HIGHMEM, rand, 32, privateKey, &privateKeyLength, publicKey, &publicKeyLength);

    // if(retval != CMOX_ECC_SUCCESS)
    // {
    //     // printf("cmox_ecdsa_keyGen failed, ret=%d\n", (int)retval);
    //     return false;
    // }

    // cmox_ecc_cleanup(&Ecc_Ctx);
    return true;
}

bool cryptoInitECC()
{
    // if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    // {
    //     return false;
    // }
    // return true;
    return true;
}

bool cryptoVerifyECC(uint8_t *data, size_t dataLength, uint8_t *signature, size_t signatureLength, uint8_t *publicKey, size_t publicKeyLength)
{
    // printf("Verifying ECC signature\n");
    uint8_t hash[32];
    // uint32_t faultCheck;
    // size_t computed_size;

    // if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    // {
    //     LOG_DEBUG("cmox_initialize failed");
    //     return false;
    // }

    // cmox_hash_retval_t hretval = cmox_hash_compute(CMOX_SHA256_ALGO, data, dataLength, hash, CMOX_SHA256_SIZE, &computed_size);
    // if(hretval != CMOX_HASH_SUCCESS)
    // {
    //     // printf("cmox_hash_compute failed, ret=%d\n", (int)hretval);
    //     return false;
    // }
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, dataLength);
    sha256_final(&ctx, hash);

    LOG_TRACE("Hash computed %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15], hash[16], hash[17], hash[18], hash[19], hash[20], hash[21], hash[22], hash[23], hash[24], hash[25], hash[26], hash[27], hash[28], hash[29], hash[30], hash[31]);
    LOG_DUMP("Signature", signature, signatureLength);
    LOG_DUMP("Public key", publicKey, publicKeyLength);

    if(uECC_verify(publicKey, hash, sizeof(hash), signature, uECC_secp256k1()) == 1)
    {
        LOG_DEBUG("uECC_verify success");
    }
    else
    {
        return false;
        LOG_DEBUG("uECC_verify failed");
    }

    // cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));

    // cmox_ecc_retval_t retval = cmox_ecdsa_verify(&Ecc_Ctx, CMOX_ECC_CURVE_SECP256K1, publicKey, publicKeyLength, hash, CMOX_SHA256_SIZE, signature, signatureLength, &faultCheck);

    // cmox_ecc_cleanup(&Ecc_Ctx);
    // if(retval != CMOX_ECC_AUTH_SUCCESS)
    // {
    //     LOG_DEBUG("cmox_ecdsa_verify failed, ret=%d", (int)retval);
    //     return false;
    // }

    // if(faultCheck != CMOX_ECC_AUTH_SUCCESS)
    // {
    //     LOG_DEBUG("cmox_ecdsa_verify failed, faultCheck=%d", (int)faultCheck);
    //     return false;
    // }
    
    return true;
}

void cryptoCleanECC()
{
    // cmox_ecc_cleanup(&Ecc_Ctx);
}