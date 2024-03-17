// Written by ChatGPT 3.5 on https://chat.openai.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_OCCURRENCES 100 // Maximum number of occurrences to find
#define BUF_SIZE 16         // Size of buffer for reading blocks

// Function to find positions of occurrences of a substring in a block
int* findSubstringPositions(char* haystack, char* needle, int needleLength, int* numOccurrences) {
    int* occurrences = (int*)malloc(MAX_OCCURRENCES * sizeof(int));
    *numOccurrences = 0;

    // Iterate through the haystack block
    for (int i = 0; i <= strlen(haystack) - needleLength; i++) {
        // Check if the substring matches the needle
        if (strncmp(&haystack[i], needle, needleLength) == 0) {
            // Add position of occurrence to the occurrences array
            occurrences[*numOccurrences] = i;
            (*numOccurrences)++;

            // Check if we have reached the maximum number of occurrences
            if (*numOccurrences >= MAX_OCCURRENCES) {
                fprintf(stderr, "Maximum number of occurrences reached. Increase MAX_OCCURRENCES.\n");
                break;
            }
        }
    }

    return occurrences;
}

// Function to read blocks from a file and search for occurrences of the needle string
void searchInBlocks(char* filePath, char* needle, int needleLength) {
    char buf[BUF_SIZE];
    int num;
    int numOccurrences;
    int totalOccurrences = 0;
    int blockNumber = 1;
    int totalBlocksRead = 0;

    // Open file for reading
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    // Read blocks and search for occurrences
    while ((num = read(fd, buf, BUF_SIZE))) {
        if (num == -1) {
            perror("read");
            close(fd);
            return;
        }

        // Find positions of occurrences in the current block
        int* occurrences = findSubstringPositions(buf, needle, needleLength, &numOccurrences);

        // Output the positions of occurrences in the current block
        printf("Occurrences of '%s' in block %d: ", needle, blockNumber++);
        for (int i = 0; i < numOccurrences; i++) {
            printf("%d ", occurrences[i] + totalBlocksRead * BUF_SIZE); // Adjust position by adding offset
        }
        printf("\n");

        // Update total number of occurrences
        totalOccurrences += numOccurrences;

        // Update total number of blocks read
        totalBlocksRead++;

        // Free dynamically allocated memory
        free(occurrences);
    }

    // Close file
    close(fd);
}

int main() {
    char needle[] = "ABC";  // Needle string
    int needleLength = strlen(needle);

    // Call searchInBlocks function with file path and needle string
    searchInBlocks("experiment.txt", needle, needleLength);
    
    searchInBlocks("experiment2.txt", needle, needleLength);

    return 0;
}
