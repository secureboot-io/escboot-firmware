#include "crypto/ecc.h"
#include <stdio.h>

/**
 ******************************************************************************
 * @file    ECC/ECDSA_SignVerify/Src/main.c
 * @author  MCD Application Team
 * @brief   This example provides a short description of how to use the
 *          STM32 Cryptographic Library
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "cmox_crypto.h"
#include "crypto/entropy.h"
#include "utils/hexdump.h"
#include "ecc/cmox_ecdsa.h"
/* Global variables ----------------------------------------------------------*/
/* ECC context */
cmox_ecc_handle_t Ecc_Ctx;
/* ECC working buffer */
uint8_t Working_Buffer[2000];

/* Random data buffer */
uint8_t Computed_Random[32];

//__IO TestStatus glob_status = FAILED;

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** Extract from SigGen.txt
  * [P-256,SHA-224]

Msg = ff624d0ba02c7b6370c1622eec3fa2186ea681d1659e0a845448e777b75a8e77a77bb26e5733179d58ef9bc8a4e8b69
71aef2539f77ab0963a3415bbd6258339bd1bf55de65db520c63f5b8eab3d55debd05e9494212170f5d65b3286b8b668705b1
e2b2b5568610617abb51d2dd0cb450ef59df4b907da90cfa7b268de8c4c2
d = 708309a7449e156b0db70e5b52e606c7e094ed676ce8953bf6c14757c826f590
Qx = 29578c7ab6ce0d11493c95d5ea05d299d536801ca9cbd50e9924e43b733b83ab
Qy = 08c8049879c6278b2273348474158515accaa38344106ef96803c5a05adc4800
k = 58f741771620bdc428e91a32d86d230873e9140336fcfb1e122892ee1d501bdc
R = 4a19274429e40522234b8785dc25fc524f179dcc95ff09b3c9770fc71f54ca0d
S = 58982b79a65b7320f5b92d13bdaecdd1259e760f0f718ba933fd098f6f75d4b7

  */
// const uint8_t Message[] =
// {
//   0xff, 0x62, 0x4d, 0x0b, 0xa0, 0x2c, 0x7b, 0x63, 0x70, 0xc1, 0x62, 0x2e, 0xec, 0x3f, 0xa2, 0x18,
//   0x6e, 0xa6, 0x81, 0xd1, 0x65, 0x9e, 0x0a, 0x84, 0x54, 0x48, 0xe7, 0x77, 0xb7, 0x5a, 0x8e, 0x77,
//   0xa7, 0x7b, 0xb2, 0x6e, 0x57, 0x33, 0x17, 0x9d, 0x58, 0xef, 0x9b, 0xc8, 0xa4, 0xe8, 0xb6, 0x97,
//   0x1a, 0xef, 0x25, 0x39, 0xf7, 0x7a, 0xb0, 0x96, 0x3a, 0x34, 0x15, 0xbb, 0xd6, 0x25, 0x83, 0x39,
//   0xbd, 0x1b, 0xf5, 0x5d, 0xe6, 0x5d, 0xb5, 0x20, 0xc6, 0x3f, 0x5b, 0x8e, 0xab, 0x3d, 0x55, 0xde,
//   0xbd, 0x05, 0xe9, 0x49, 0x42, 0x12, 0x17, 0x0f, 0x5d, 0x65, 0xb3, 0x28, 0x6b, 0x8b, 0x66, 0x87,
//   0x05, 0xb1, 0xe2, 0xb2, 0xb5, 0x56, 0x86, 0x10, 0x61, 0x7a, 0xbb, 0x51, 0xd2, 0xdd, 0x0c, 0xb4,
//   0x50, 0xef, 0x59, 0xdf, 0x4b, 0x90, 0x7d, 0xa9, 0x0c, 0xfa, 0x7b, 0x26, 0x8d, 0xe8, 0xc4, 0xc2
// };
const uint8_t Private_Key[] =
    {
        0x70, 0x83, 0x09, 0xa7, 0x44, 0x9e, 0x15, 0x6b, 0x0d, 0xb7, 0x0e, 0x5b, 0x52, 0xe6, 0x06, 0xc7,
        0xe0, 0x94, 0xed, 0x67, 0x6c, 0xe8, 0x95, 0x3b, 0xf6, 0xc1, 0x47, 0x57, 0xc8, 0x26, 0xf5, 0x90};
const uint8_t Public_Key[] =
    {
        0x29, 0x57, 0x8c, 0x7a, 0xb6, 0xce, 0x0d, 0x11, 0x49, 0x3c, 0x95, 0xd5, 0xea, 0x05, 0xd2, 0x99,
        0xd5, 0x36, 0x80, 0x1c, 0xa9, 0xcb, 0xd5, 0x0e, 0x99, 0x24, 0xe4, 0x3b, 0x73, 0x3b, 0x83, 0xab,
        0x08, 0xc8, 0x04, 0x98, 0x79, 0xc6, 0x27, 0x8b, 0x22, 0x73, 0x34, 0x84, 0x74, 0x15, 0x85, 0x15,
        0xac, 0xca, 0xa3, 0x83, 0x44, 0x10, 0x6e, 0xf9, 0x68, 0x03, 0xc5, 0xa0, 0x5a, 0xdc, 0x48, 0x00};
const uint8_t Known_Random[] = /* = k - 1 */
    {
        0x58, 0xf7, 0x41, 0x77, 0x16, 0x20, 0xbd, 0xc4, 0x28, 0xe9, 0x1a, 0x32, 0xd8, 0x6d, 0x23, 0x08,
        0x73, 0xe9, 0x14, 0x03, 0x36, 0xfc, 0xfb, 0x1e, 0x12, 0x28, 0x92, 0xee, 0x1d, 0x50, 0x1b, 0xdb};
const uint8_t Known_Signature[] =
    {
        0x4a, 0x19, 0x27, 0x44, 0x29, 0xe4, 0x05, 0x22, 0x23, 0x4b, 0x87, 0x85, 0xdc, 0x25, 0xfc, 0x52,
        0x4f, 0x17, 0x9d, 0xcc, 0x95, 0xff, 0x09, 0xb3, 0xc9, 0x77, 0x0f, 0xc7, 0x1f, 0x54, 0xca, 0x0d,
        0x58, 0x98, 0x2b, 0x79, 0xa6, 0x5b, 0x73, 0x20, 0xf5, 0xb9, 0x2d, 0x13, 0xbd, 0xae, 0xcd, 0xd1,
        0x25, 0x9e, 0x76, 0x0f, 0x0f, 0x71, 0x8b, 0xa9, 0x33, 0xfd, 0x09, 0x8f, 0x6f, 0x75, 0xd4, 0xb7};

/* Computed data buffer */
uint8_t Computed_Hash[CMOX_SHA224_SIZE];
uint8_t Computed_Signature[CMOX_ECC_SECP256R1_SIG_LEN];

/* Private function prototypes -----------------------------------------------*/
// static void SystemClock_Config(void);
// static void Error_Handler(void);
/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */


bool crypto_ecc_generate(uint8_t *privateKey, size_t privateKeyLength, uint8_t *publicKey, size_t publicKeyLength)
{
    printf("Generating ECC keypair\n");
    uint8_t rand[32];
    Entropy_Gather(256, rand);
    cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));
    cmox_ecc_retval_t retval = cmox_ecdsa_keyGen(&Ecc_Ctx, CMOX_ECC_SECP256R1_HIGHMEM, rand, 32, privateKey, &privateKeyLength, publicKey, &publicKeyLength);

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
    uint8_t pub[64];
    uint8_t prv[32];
    if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    {
        return false;
    }
    return true;
}

void test()
{
    cmox_hash_retval_t hretval;
    cmox_ecc_retval_t retval;
    size_t computed_size;
    /* Fault check verification variable */
    // uint32_t fault_check = CMOX_ECC_AUTH_FAIL;

    /* STM32G0xx HAL library initialization:
         - Configure the Flash prefetch
         - Systick timer is configured by default as source of time base, but user
           can eventually implement his proper time base source (a general purpose
           timer for example or other time source), keeping in mind that Time base
           duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
           handled in milliseconds basis.
         - Low Level Initialization
       */
    // HAL_Init();

    /* Configure the System clock */
    // SystemClock_Config();

    /* Configure LED4 */
    // BSP_LED_Init(LED4);

    /* Initialize cryptographic library */
    //   if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    //   {
    //     Error_Handler();
    //   }

    printf("cmox_initialize success\n");
    /* Compute directly the digest passing all the needed parameters */
    hretval = cmox_hash_compute(CMOX_SHA224_ALGO,             /* Use SHA224 algorithm */
                                (uint8_t *)0x8001000, 0x8000, /* Message to digest */
                                Computed_Hash,                /* Data buffer to receive digest data */
                                CMOX_SHA224_SIZE,             /* Expected digest size */
                                &computed_size);              /* Size of computed digest */

    printf("cmox_hash_compute success\n");
    /* Verify API returned value */
    if (hretval != CMOX_HASH_SUCCESS)
    {
        printf("cmox_hash_compute failed, ret=%d\n", (int)hretval);
        return;
    }

    /* Verify generated data size is the expected one */
    if (computed_size != CMOX_SHA224_SIZE)
    {
        printf("computed_size failed, ret=%d\n", (int)computed_size);
        return;
    }

    printf("Computed_Hash: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
           Computed_Hash[0], Computed_Hash[1], Computed_Hash[2], Computed_Hash[3], Computed_Hash[4], Computed_Hash[5], Computed_Hash[6], Computed_Hash[7],
           Computed_Hash[8], Computed_Hash[9], Computed_Hash[10], Computed_Hash[11], Computed_Hash[12], Computed_Hash[13], Computed_Hash[14], Computed_Hash[15],
           Computed_Hash[16], Computed_Hash[17], Computed_Hash[18], Computed_Hash[19], Computed_Hash[20], Computed_Hash[21], Computed_Hash[22], Computed_Hash[23],
           Computed_Hash[24], Computed_Hash[25], Computed_Hash[26], Computed_Hash[27]);

    /* --------------------------------------------------------------------------
     * KNOWN RANDOM USAGE
     * --------------------------------------------------------------------------
     */

    /* Construct a ECC context, specifying mathematics implementation and working buffer for later processing */
    /* Note: CMOX_ECC256_MATH_FUNCS refer to the default mathematics implementation
     * selected in cmox_default_config.h. To use a specific implementation, user can
     * directly choose:
     * - CMOX_MATH_FUNCS_SMALL to select the mathematics small implementation
     * - CMOX_MATH_FUNCS_FAST to select the mathematics fast implementation
     * - CMOX_MATH_FUNCS_SUPERFAST256 to select the mathematics fast implementation optimized for 256 bits computation
     */
    cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));

    /* Compute directly the signature passing all the needed parameters */
    /* Note: CMOX_ECC_CURVE_SECP256R1 refer to the default SECP256R1 definition
     * selected in cmox_default_config.h. To use a specific definition, user can
     * directly choose:
     * - CMOX_ECC_SECP256R1_LOWMEM to select the low RAM usage definition (slower computing)
     * - CMOX_ECC_SECP256R1_HIGHMEM to select the high RAM usage definition (faster computing)
     */
    retval = cmox_ecdsa_sign(&Ecc_Ctx,                            /* ECC context */
                             CMOX_ECC_CURVE_SECP256R1,            /* SECP256R1 ECC curve selected */
                             Known_Random, sizeof(Known_Random),  /* Random data buffer */
                             Private_Key, sizeof(Private_Key),    /* Private key for signature */
                             Computed_Hash, CMOX_SHA224_SIZE,     /* Digest to sign */
                             Computed_Signature, &computed_size); /* Data buffer to receive signature */

    /* Verify API returned value */
    if (retval != CMOX_ECC_SUCCESS)
    {
        printf("cmox_ecdsa_sign failed, ret=%d\n", (int)retval);
        return;
    }

    /* Verify generated data size is the expected one */
    if (computed_size != sizeof(Known_Signature))
    {
        printf("computed_size failed, ret=%d\n", computed_size);
        return;
    }

    /* Verify generated data are the expected ones */
    if (memcmp(Computed_Signature, Known_Signature, computed_size) != 0)
    {
        printf("memcmp failed\n");
        return;
    }

    /* Cleanup context */
    cmox_ecc_cleanup(&Ecc_Ctx);

    //   /* Construct a ECC context, specifying mathematics implementation and working buffer for later processing */
    //   /* Note: CMOX_ECC256_MATH_FUNCS refer to the default mathematics implementation
    //    * selected in cmox_default_config.h. To use a specific implementation, user can
    //    * directly choose:
    //    * - CMOX_MATH_FUNCS_SMALL to select the mathematics small implementation
    //    * - CMOX_MATH_FUNCS_FAST to select the mathematics fast implementation
    //    * - CMOX_MATH_FUNCS_SUPERFAST256 to select the mathematics fast implementation optimized for 256 bits computation
    //    */
    //   cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));

    //   /* Verify directly the signature passing all the needed parameters */
    //   /* Note: CMOX_ECC_CURVE_SECP256R1 refer to the default SECP256R1 definition
    //    * selected in cmox_default_config.h. To use a specific definition, user can
    //    * directly choose:
    //    * - CMOX_ECC_SECP256R1_LOWMEM to select the low RAM usage definition (slower computing)
    //    * - CMOX_ECC_SECP256R1_HIGHMEM to select the high RAM usage definition (faster computing)
    //    */
    //   retval = cmox_ecdsa_verify(&Ecc_Ctx,                                  /* ECC context */
    //                              CMOX_ECC_CURVE_SECP256R1,                  /* SECP256R1 ECC curve selected */
    //                              Public_Key, sizeof(Public_Key),            /* Public key for verification */
    //                              Computed_Hash, CMOX_SHA224_SIZE,           /* Digest to verify */
    //                              Known_Signature, sizeof(Known_Signature),  /* Data buffer to receive signature */
    //                              &fault_check);                             /* Fault check variable:
    //                                                             to ensure no fault injection occurs during this API call */

    //   /* Verify API returned value */
    //   if (retval != CMOX_ECC_AUTH_SUCCESS)
    //   {
    //     Error_Handler();
    //   }
    //   /* Verify Fault check variable value */
    //   if (fault_check != CMOX_ECC_AUTH_SUCCESS)
    //   {
    //     Error_Handler();
    //   }

    //   /* Cleanup context */
    //   cmox_ecc_cleanup(&Ecc_Ctx);

    //   /* --------------------------------------------------------------------------
    //    * TRUE RANDOM USAGE
    //    * --------------------------------------------------------------------------
    //    */

    //   /* Construct a ECC context, specifying mathematics implementation and working buffer for later processing */
    //   /* Note: CMOX_ECC256_MATH_FUNCS refer to the default mathematics implementation
    //    * selected in cmox_default_config.h. To use a specific implementation, user can
    //    * directly choose:
    //    * - CMOX_MATH_FUNCS_SMALL to select the mathematics small implementation
    //    * - CMOX_MATH_FUNCS_FAST to select the mathematics fast implementation
    //    * - CMOX_MATH_FUNCS_SUPERFAST256 to select the mathematics fast implementation optimized for 256 bits computation
    //    */
    //   cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));

    //   /* Note: The random value must satisfy some arithmetic constraints versus the selected curve and points
    //    * to minimize the statictical vulnerability.
    //    * In case this is not satisfied, cmox_ecdsa_sign returns CMOX_ECC_ERR_WRONG_RANDOM: new random has to be
    //    * generated and API call again.
    //    */
    //   do
    //   {
    //     memset(Computed_Random, 0x00, sizeof(Computed_Random) / sizeof(uint8_t));
    //     Entropy_Gather(sizeof(Computed_Random) * 8, Computed_Random);

    //     /* Compute directly the signature passing all the needed parameters */
    //     /* Note: CMOX_ECC_CURVE_SECP256R1 refer to the default SECP256R1 definition
    //      * selected in cmox_default_config.h. To use a specific definition, user can
    //      * directly choose:
    //      * - CMOX_ECC_SECP256R1_LOWMEM to select the low RAM usage definition (slower computing)
    //      * - CMOX_ECC_SECP256R1_HIGHMEM to select the high RAM usage definition (faster computing)
    //      */
    //     retval = cmox_ecdsa_sign(&Ecc_Ctx,                                            /* ECC context */
    //                              CMOX_ECC_CURVE_SECP256R1,                            /* SECP256R1 ECC curve selected */
    //                              (uint8_t *)Computed_Random, sizeof(Computed_Random), /* Random data buffer */
    //                              Private_Key, sizeof(Private_Key),                    /* Private key for signature */
    //                              Computed_Hash, CMOX_SHA224_SIZE,                     /* Digest to sign */
    //                              Computed_Signature, &computed_size);                 /* Data buffer to receive signature */

    //   } while (retval == CMOX_ECC_ERR_WRONG_RANDOM);

    //   /* Verify API returned value */
    //   if (retval != CMOX_ECC_SUCCESS)
    //   {
    //     Error_Handler();
    //   }

    //   /* Verify generated data size is the expected one */
    //   if (computed_size != sizeof(Known_Signature))
    //   {
    //     Error_Handler();
    //   }

    //   /* Cleanup context */
    //   cmox_ecc_cleanup(&Ecc_Ctx);

    //   /* Construct a ECC context, specifying mathematics implementation and working buffer for later processing */
    //   /* Note: CMOX_ECC256_MATH_FUNCS refer to the default mathematics implementation
    //    * selected in cmox_default_config.h. To use a specific implementation, user can
    //    * directly choose:
    //    * - CMOX_MATH_FUNCS_SMALL to select the mathematics small implementation
    //    * - CMOX_MATH_FUNCS_FAST to select the mathematics fast implementation
    //    * - CMOX_MATH_FUNCS_SUPERFAST256 to select the mathematics fast implementation optimized for 256 bits computation
    //    */
    //   cmox_ecc_construct(&Ecc_Ctx, CMOX_ECC256_MATH_FUNCS, Working_Buffer, sizeof(Working_Buffer));

    //   /* Verify directly the signature passing all the needed parameters */
    //   /* Note: CMOX_ECC_CURVE_SECP256R1 refer to the default SECP256R1 definition
    //    * selected in cmox_default_config.h. To use a specific definition, user can
    //    * directly choose:
    //    * - CMOX_ECC_SECP256R1_LOWMEM to select the low RAM usage definition (slower computing)
    //    * - CMOX_ECC_SECP256R1_HIGHMEM to select the high RAM usage definition (faster computing)
    //    */
    //   retval = cmox_ecdsa_verify(&Ecc_Ctx,                                        /* ECC context */
    //                              CMOX_ECC_CURVE_SECP256R1,                        /* SECP256R1 ECC curve selected */
    //                              Public_Key, sizeof(Public_Key),                  /* Public key for verification */
    //                              Computed_Hash, CMOX_SHA224_SIZE,                 /* Digest to verify */
    //                              Computed_Signature, sizeof(Computed_Signature),  /* Data buffer to receive signature */
    //                              &fault_check);                                   /* Fault check variable:
    //                                                             to ensure no fault injection occurs during this API call */

    //   /* Verify API returned value */
    //   if (retval != CMOX_ECC_AUTH_SUCCESS)
    //   {
    //     Error_Handler();
    //   }
    //   /* Verify Fault check variable value */
    //   if (fault_check != CMOX_ECC_AUTH_SUCCESS)
    //   {
    //     Error_Handler();
    //   }

    //   /* Cleanup context */
    //   cmox_ecc_cleanup(&Ecc_Ctx);

    //   /* No more need of cryptographic services, finalize cryptographic library */
    //   if (cmox_finalize(NULL) != CMOX_INIT_SUCCESS)
    //   {
    //     Error_Handler();
    //   }

    //   /* Turn on LED4 in an infinite loop in case of ECC ECDSA operations are successful */
    //   BSP_LED_On(LED4);
    //   glob_status = PASSED;
    //   while (1)
    //   {}
}

void crypto_ecc_cleanup()
{
    cmox_ecc_cleanup(&Ecc_Ctx);
}