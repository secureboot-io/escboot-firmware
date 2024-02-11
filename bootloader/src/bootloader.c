#include "bootloader.h"

#include <stdint.h>
#include <stddef.h>
#include <checksum.h>
#include <stdio.h>
#include <string.h>

#include "io/serialwire.h"
#include "io/gpio.h"
#include "utils/hexdump.h"
#include "flash.h"

uint8_t deviceInfo[9] = {0x34, 0x37, 0x31, 0x00, 0x1f, 0x06, 0x06, 0x01, 0x30};

typedef union
{
    uint16_t word;
    uint8_t bytes[2];
} uint8_16_u;

bool checkCrc(uint8_t *pBuff, uint16_t length)
{
    uint8_16_u receivedCrc16;

    uint16_t calculatedCrc16 = crc_16(pBuff, length - 2);

    receivedCrc16.bytes[0] = pBuff[length - 2];
    receivedCrc16.bytes[1] = pBuff[length - 1];
    return receivedCrc16.word == calculatedCrc16;
}

void processCmd(uint8_t *packet, size_t packetSize)
{
    uint8_t resp[256];
    static bool receiveBuffer = false;
    static uint16_t bufferSize = 0;
    static uint32_t address = 0;
    static uint8_t buffer[1024];

    if(receiveBuffer)
    {
        receiveBuffer = false;
        memcpy(buffer, packet, packetSize);
        resp[0] = ACK;
        writeBuffer(resp, 1);
        printf("receiveBuffer, size=%d\n", packetSize);
        return;
    }

    uint8_t command = packet[0];
    switch(command) 
    {
        case CMD_RUN:
            resp[0] = ACK;
            writeBuffer(resp, 1);
            printf("CMD_RUN\n");
            break;

        case CMD_SET_BUFFER:
            resp[0] = ACK;
            receiveBuffer = true;
            bufferSize = (packet[2] << 8) | packet[3];
            writeBuffer(resp, 1);
            printf("CMD_SET_BUFFER: %d\n", bufferSize);
            break;

        case CMD_SET_ADDRESS:
            address = 0x08000000 + (packet[2] << 8 | packet[3]);
            resp[0] = ACK;
            writeBuffer(resp, 1);
            printf("CMD_SET_ADDRESS: %08x\n", (unsigned int) address);
            break;

        case CMD_PROG_FLASH:
            save_flash_nolib(buffer, bufferSize, address);
            resp[0] = ACK;
            writeBuffer(resp, 1);
            printf("CMD_PROG_FLASH\n");
            break;

        case CMD_VERIFY_FLASH:
            resp[0] = ACK;
            writeBuffer(resp, 1);
            printf("CMD_VERIFY_FLASH\n");
            break;

        default:
            resp[0] = NACK_BAD_CMD;
            writeBuffer(resp, 1);
            printf("NAK: bad cmd = %02x\n", command);
    }
}

int bootloader()
{
    printf("initializing\n");
    pinInit();
    uint8_t resp[256];

    while (1)
    {
        uint8_t buf[248];
        size_t outLen;
        if (readBuffer(buf, 248, &outLen))
        {
            //hexdump(buf, outLen);
            if (outLen == 21 && buf[20] == 0x7d)
            {
                writeBuffer(deviceInfo, 9);
            }
            else
            {
                if (outLen >= 3)
                {
                    if (checkCrc(buf, outLen))
                    {
                        processCmd(buf, outLen - 2);
                    }
                    else
                    {
                        resp[0] = NACK_BAD_CRC;
                        writeBuffer(resp, 1);
                        printf("NAK: bad crc\n");
                    }
                }
            }
        }
    }

    return 0;
}