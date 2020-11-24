#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "memsearch.h"

int main(int argc, char *argv[])
{
  const char* filePath; // = "/dev/rdisk1";
  unsigned long long gibibytes = 16;
  unsigned long long updatesOften = gibibytes * 1024 * 1024 * 1024;
  unsigned long long initialOffset = 0;
  if (argc <= 2) {
    printf("Usage: %s <file or disk path> <needle> [give updates this often] [initial offset]\nPass 0 to skip any argument besides the first one.\n", argv[0]);
    return 0;
  }
  filePath = argv[1];
  const char* needle = argv[2];
  if (argc >= 4) {
    updatesOften = strtoull(argv[3], NULL, 0);
  }
  if (argc >= 5) {
    initialOffset = strtoull(argv[4], NULL, 0);
  }
#define BUF_SIZE 2048
  char buf[BUF_SIZE];
  int num = 0;
  int fd = open(filePath, O_RDONLY);
  if (initialOffset) {
    off_t currentOffset = lseek(fd, initialOffset, SEEK_SET);
    if (currentOffset == -1) { // Error
      perror("Error setting given initial offset");
      return 1;
    }
  }
  if (fd == -1) {
    perror(NULL);
    return 1;
  }
  uintmax_t counter = 0;
  const char* needleNext = needle;
  while ((num = read(fd, buf, sizeof(buf)))) {
    if (num == -1) {
      perror(NULL);
      close(fd);
      return 1;
    }
    
    //printf("%s", buf);
    const char* offset = buf, *offset2;
    // Check for parts of the string "jlox"
    /* if ((offset = memsearch(offset, num, "j")) || foundLevel > 0) { */
    /*   foundLevel++; */
    /*   if ((offset2 = memsearch(offset, num, "l")) || foundLevel > 1) { */
    /* 	foundLevel++; */
    /* 	if ((offset = memsearch(offset, num, "o")) || foundLevel > 2) { */
    /* 	  foundLevel++; */
    /* 	  if ((offset = memsearch(offset, num, "x"))) { */
    /* 	    found(); */
    /* 	    foundLevel = 0; */
    /* 	  } */
    /* 	} */
    /*   } */
    /*   else { */
    /* 	offsetBack = offset; */
    /*   } */
    /* } */

    int reason;
    offset = memsearch_ext(offset, num, needle, &reason, &needleNext);
    if (reason == kMemSearchExitReason_Found) {
      // Found it
      uintmax_t currentOffset = counter + (offset - buf) + initialOffset;
      /* off_t currentOffset = lseek(fd, 0, SEEK_CUR); /\* Get current offset.*\/ */
      /* if (currentOffset == -1) { /\* Error *\/				 */
      /* 	perror(NULL);						        */
      /* 	uintmax_t found = counter + offset; */
      /* 	printf("Recovering from error: Found string but couldn't get the offset. Using other counter variable, which is: %ju\n", found);  */
      /* }									 */
      printf("Found at position %ju from the beginning of the file (0-based)\n", currentOffset);					
      close(fd); return 0;
    }
    
    counter += num;
    if (updatesOften && counter % updatesOften == 0) {
      //double gib = (double)counter / 1024.0 / 1024.0 / 1024.0;
      double kib = (double)counter / 1024.0, mib, gib, *use = &kib;
      const char* useStr = "KiB";
      if (kib > 1024.0) {
	mib = kib / 1024.0;
	use = &mib;
	useStr = "MiB";
	if (mib > 1024.0) {
	  gib = mib / 1024.0;
	  use = &gib;
	  useStr = "GiB";
	}
      }
      printf("%f %s processed so far\n", *use, useStr);
    }
  }

  puts("Not found");
  close(fd);
}
