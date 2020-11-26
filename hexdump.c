// https://gist.github.com/ccbrown/9722406

#include <stdio.h>
#include <limits.h>
#include <string.h>

// https://stackoverflow.com/questions/1068849/how-do-i-determine-the-number-of-digits-of-an-integer-in-c
int numPlaces (int n) {
    if (n < 0) n = (n == INT_MIN) ? INT_MAX : -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    /*      2147483647 is 2^31-1 - add more ifs as needed
       and adjust this final return as well. */
    return 10;
}

void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	size_t numDigits = numPlaces(size);

	const char* format = "[%%%dzu:] "; // %% is an escaped single percent sign.
	char printStr_[strlen(format) + 1]; // Will be something like "[%4zu:] " but with a different number than 4.
	int res = snprintf(printStr_, sizeof(printStr_), format, numPlaces);
	const char* printStr;
	if (!(res > 0 && res < sizeof(printStr_))) {
	  // Failed to write entire string
	  // Fallback on basic format:
	  printStr = "[%4zu:] ";
	}
	else {
	  printStr = printStr_;
	}
	
	for (i = 0; i < size; ++i) {
	  if (i % 8 == 0) { // Print the offset in the buffer:
	    printf(printStr, i);
	  }
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}
