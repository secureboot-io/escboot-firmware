#ifndef DEF_BOOTLOADER_INCLUDE_CRYPTO_ECC_H
#define DEF_BOOTLOADER_INCLUDE_CRYPTO_ECC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool crypto_ecc_startup();
void crypto_ecc_cleanup();

#ifdef __cplusplus
}
#endif


#endif // DEF_BOOTLOADER_INCLUDE_CRYPTO_ECC_H