#ifndef DEF_SECUREBOOT_H
#define DEF_SECUREBOOT_H

#include <stdint.h>
#include <stdbool.h>

#define SECUREBOOT_SECURE_FLAG_ADDRESS      0x0800F000
#define SECUREBOOT_SECURE_FLAG_SIZE         1
#define SECUREBOOT_ENABLED_FLAG_ADDRESS     0x0800F001
#define SECUREBOOT_ENABLED_FLAG_SIZE        1

#ifdef __cplusplus
extern "C" {
#endif

bool secureboot_startup();
bool secureboot_write(uint32_t address, uint32_t bufferSize, uint8_t *buffer);
bool secureboot_read(uint32_t address, uint32_t bufferSize, uint8_t *buffer);

#ifdef __cplusplus
}
#endif

#endif //DEF_SECUREBOOT_H