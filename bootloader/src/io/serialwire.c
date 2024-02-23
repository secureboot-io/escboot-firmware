#include "io/serialwire.h"

#include "utils/millis.h"
#include "utils/micros.h"
#include "io/gpio.h"

bool readChar(uint8_t *buffer)
{
    uint32_t bitTime = millis() + START_BIT_TIMEOUT_MS;

    // Wait for RX to go low
    while (pinRead()) {
    	if(millis() > bitTime) {
    		return false;
    	}
    }

    // Go to 1/4 of start bit
    bitTime = micros() + (BIT_TIME_HALVE>>2);
    while (micros() < bitTime);

    uint16_t bitmask = 0;
    uint8_t bit = 0;
    while (1) {
    	//Read bit
        if (pinRead())
        {
            bitmask |= (1 << bit);
        }

        // Increment bit and return if 10 bits are read
        // don't wait for stop bit
        bit++;
        if(bit == 10)
        	break;

        // go to 1/4 of next bit
        bitTime = bitTime + BIT_TIME;
        while (micros() < bitTime);
    }

    // check start bit and stop bit
    if ((bitmask & 1) || (!(bitmask & (1 << 9)))) {
        return false;
    }

    // Set char to buffer
    *buffer = bitmask >> 1;

    // Success
    return true;
}

bool writeChar(uint8_t buffer)
{

	uint16_t bitmask = buffer;

	// Add start/stop bits
	bitmask <<= 1;
	bitmask |= 1 << 9;

	// Start time
	uint32_t bitTime = micros();
	for(int i = 0; i < 10; i++) {
		//Send bit
		pinWrite((bitmask & 1) == 1);

		//Delay
		bitTime += BIT_TIME;
		while (micros() < bitTime);

		//Shift
		bitmask >>= 1;
	}

    // Success
    return true;
}

bool readBuffer(uint8_t *buffer, size_t bufferLen, size_t *outLen) {
	int i = 0;

	//
	pinSetInputPullUp();

	// Read specified number of bytes
	while(i < bufferLen) {
		if(readChar(buffer + i)) {
			i++;
		} else {
			// return if error reading
			break;
		}
	}

	// Set output length
	*outLen = i;

	// Success if atleast 1 byte read
	return i != 0;
}

bool writeBuffer(uint8_t *buffer, size_t bufferLen) {
	bool success;

	//
	pinSetOutput();

	while(bufferLen--)
	{
		success = writeChar(*buffer++);
		if(!success) {
			return false;
		}
	}

	return success;
}