#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "memsearch.h"
#include "hexdump.h"
#include "int_utils.h"

const char* foundStr = "Found at position %ju from the beginning of the file (0-based)\n";

void main_test();

void* buf1 = NULL;
void freeBuf1() {
  free(buf1);
}

int main(int argc, char *argv[])
{
  // main_test();
  // return 0;
  
  const char* filePath; // = "/dev/rdisk1";
  unsigned long long gibibytes = 16;
  unsigned long long updatesOften = gibibytes * 1024 * 1024 * 1024;
  unsigned long long initialOffset = 0;
  if (argc <= 2) {
    printf("Usage: %s <file or disk path> <needle, or empty string -- if empty, provide needle on stdin (useful for needles that contain null bytes)> [give updates this often, or 0 for default] [initial offset]\nPass 0 to skip any argument besides the first one.\n"
	   "\nFor issues on macOS with the file or disk path given, run `diskutil list` and ensure you are not using a \"synthesized\" disk (it will have this next to the name if you are); for example, if /dev/rdisk1 (with or without the \"r\" it is the same disk just cached differently) is synthesized, then it may be that it represents disk0s4 instead (a partition of disk0), so use that, or rdisk0s4.\n"
	   "\nUsage examples:\n"
	   "    python3 -c 'import sys; sys.stdout.buffer.write(bytes.fromhex(sys.argv[1]))' ce24b9a2200000001d7798f4bafa6213 | sudo ./main /dev/sda '' 0 $((4269688830*512))   # Finds the hex sequence ce24b9a2200000001d7798f4bafa6213 starting the search at byte 4269688830*512 (4269688830 is in sectors and assumes 512-byte sectors).\n", argv[0]);
    return 0;
  }
  filePath = argv[1];
  const char* needle = argv[2];
  
  // Read bytes from stdin if `needle` is empty
  if (strcmp(needle, "") == 0) {
  	// Based on https://stackoverflow.com/questions/2496668/how-to-read-the-standard-input-into-string-variable-until-eof-in-c
	int    c;
	size_t s = 0;
	size_t bufSize = 1024;
	char* buf = (char*)malloc(sizeof(char) * bufSize);
	if (buf == NULL) {
	  printf("Failed to allocate memory for %zu byte(s). Exiting.\n", bufSize);
	  return 1;
	}

	while ((c = getchar()) != EOF)
	{
	  if (s >= bufSize) {
	    // Realloc bigger buffer
	    const size_t newSize = bufSize * 2;
	    buf = realloc(buf, newSize);
	    if (buf == NULL) {
	    	printf("Failed to allocate memory for %zu byte(s). Exiting.\n", newSize);
	    	return 1;
	    }
	    bufSize = newSize;
	  }
	  buf[s] = c;
	  s++;
	}

	printf("Read %zu byte(s) from stdin:\n", s);
      	DumpHex(buf, s);
      	
      	needle = buf;
      	buf1 = buf;
      	atexit(freeBuf1);
  }
  
  if (argc >= 4) {
    unsigned long long res = strtoull(argv[3], NULL, 0);
    if (res != 0) {
      updatesOften = res;
    }
  }
  if (argc >= 5) {
    initialOffset = strtoull(argv[4], NULL, 0);
  }
#define BUF_SIZE 8192 //2048
  char buf[BUF_SIZE];
  int num = 0;
  int fd = open(filePath, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }
  if (initialOffset) {
    // (Need to do a few seeks if initialOffset > OFF_T_MAX.)
    //printf("%jd\n", (intmax_t)OFF_T_MAX);
    off_t initial = initialOffset > OFF_T_MAX ? OFF_T_MAX : initialOffset;
    off_t res;
    bool exit = false;
    for (unsigned long long i = initial; i < initialOffset + OFF_T_MAX; i += OFF_T_MAX) {
      if (i > initialOffset) {
	res = i - initialOffset;
	exit = true;
      }
      else {
	res = (i == initial) ? initial : OFF_T_MAX;
      }

      printf("lseek to %jd\n", (intmax_t)res);
      off_t currentOffset = lseek(fd, res, i == initial ? SEEK_SET : SEEK_CUR);
      if (currentOffset == -1) { // Error
	printf("lseek: Error setting given initial offset: %s\n", strerror(errno));
	return 1;
      }
      
      if (exit) break;
    }
  }
  uintmax_t counter = 0;
  const char* needleNext = needle;
  printf("%d\n", fd);
  bool foundAny = false; // Assume false.
  while ((num = read(fd, buf, BUF_SIZE))) {
    if (num == -1) {
      perror("read");
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
      foundAny = true;
      printf("%s %td %llu at counter %ju\n", needleNext != needle ? "true" : "false", needleNext - needle, initialOffset, counter);
      uintmax_t currentOffset = counter + (offset - buf) + (needleNext != needle ? (initialOffset - (needleNext - needle)) : initialOffset);
      /* off_t currentOffset = lseek(fd, 0, SEEK_CUR); /\* Get current offset.*\/ */
      /* if (currentOffset == -1) { /\* Error *\/				 */
      /* 	perror(NULL);						        */
      /* 	uintmax_t found = counter + offset; */
      /* 	printf("Recovering from error: Found string but couldn't get the offset. Using other counter variable, which is: %ju\n", found);  */
      /* }									 */
      printf(foundStr, currentOffset);

      static_assert(sizeof(uintmax_t) >= sizeof(unsigned long long), "This should be true");
      printf("In this hexdump at position %ju:\n", (uintmax_t)(currentOffset - counter - initialOffset));
      DumpHex(buf, num);
      
      //close(fd); return 0;
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

  if (!foundAny) {
    puts("Not found");
  }
  close(fd);
}

void main_test() {
  // https://stackoverflow.com/questions/955962/how-to-buffer-stdout-in-memory-and-write-it-from-a-dedicated-thread
#define MAX_LEN 255
  char buffer[MAX_LEN+1] = {0};
  int out_pipe[2];
  int saved_stdout;

  saved_stdout = dup(STDOUT_FILENO);  /* save stdout for display later */

  if( pipe(out_pipe) != 0 ) {          /* make a pipe */
    perror(NULL); // PROBLEM: this says "Too many open files"...
    exit(1);
  }

  dup2(out_pipe[1], STDOUT_FILENO);   /* redirect stdout to the pipe */
  close(out_pipe[1]);

  /* anything sent to printf should now go down the pipe */

#define TEST(wantFound, offsetWanted, strToFind, updatesOften, initialOffset) {	\
    int ret;								\
    char* argv[] = { "testFile", strToFind, #updatesOften, #initialOffset }; \
    int argc = sizeof(argv);						\
    ret = main(argc, argv);						\
    fflush(stdout);							\
									\
    read(out_pipe[0], buffer, MAX_LEN); /* read from pipe into buffer */ \
    									\
    /*dup2(saved_stdout, STDOUT_FILENO);*/  /* reconnect stdout for testing */ \
    /*printf("read: %s\n", buffer);*/					\
									\
    /* Find where it wrote the offset in the buffer: */			\
    uintmax_t offset;							\
    scanf(foundStr, &offset);						\
    fprintf(stderr, "%ju %ju\n", offset, UINTMAX_C(offsetWanted));	\
    fflush(stderr);							\
    if (offset != UINTMAX_C(offsetWanted)) {				\
      /* Test failed */							\
      fprintf(stderr, "Test failed: expected %ju, got %ju", UINTMAX_C(offsetWanted), offset); \
      fflush(stderr);							\
    }									\
									\
    if (wantFound == (ret == 0)) {					\
      /* Test succeeded */						\
    }									\
    else {								\
      /* Test failed */							\
      fprintf(stderr, "Test failed: expected %s, got %s", wantFound ? "found" : "not found", (ret == 0) ? "found" : "not found"); \
      fflush(stderr);							\
    }									\
  }

  TEST(true, 0, "#include", 0, 0);
  TEST(true, 0, "include", 0, 0); // BROKEN: needs to be offset 1
  // test "on, &ne" or whatever is at buffer border
}
