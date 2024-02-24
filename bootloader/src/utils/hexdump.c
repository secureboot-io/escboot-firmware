// #include "utils/hexdump.h"

// #include <stdio.h>

// static char hex[] = "0123456789ABCDEF";

// void byte2hex(char *dest, uint8_t byte)
// {
//     dest[0] = hex[byte >> 4];
//     dest[1] = hex[byte & 0x0f];
// }

// void hexline(char *dest, uint8_t *buffer, size_t len)
// {
//     int i;
//     for(i = 0; i < len; i++)
//     {
//         byte2hex(dest, buffer[i]);
//         dest += 2;
//         *dest++ = ' ';
//     }
//     for(; i < 16; i++)
//     {
//         *dest++ = ' ';
//         *dest++ = ' ';
//         *dest++ = ' ';
//     }
//     *dest++ = '\0';
// }

// void printline(char *dest, uint8_t *buffer, size_t len)
// {
//     int i;
//     for(i = 0; i < len; i++)
//     {
//         if(buffer[i] >= 32 && buffer[i] < 128)
//             *dest++ = (char) buffer[i];
//         else
//             *dest++ = '.';
//     }
//     for(; i < 16; i++)
//     {
//         *dest++ = ' ';
//     }
//     *dest++ = '\0';
// }

// void hexdump(uint8_t* buffer, size_t len)
// {
//     char hexes[64];
//     char prints[20];
//     for(int i = 0; i < len; i += 16)
//     {
//         int lineSize = (len - i) > 16 ? 16 : (len - i);
//         hexline(hexes, buffer + i, lineSize);
//         printline(prints, buffer + i, lineSize);
//         printf("%08x: %s   %s\n", i, hexes, prints);
//     }
// }