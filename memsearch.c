#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "memsearch.h"

const char* memsearch_ext(const char* haystack, size_t haystackSize, const char* needle, int* out_memSearchExitReason, const char** needleNext) {
  size_t needleLen;
  if (out_memSearchExitReason) *out_memSearchExitReason = kMemSearchExitReason_NoPartsFound;
  if (needleNext) {
    needle = *needleNext;
  }
  needleLen = strlen(needle);
  if (needleLen == 0) {
    if (out_memSearchExitReason) *out_memSearchExitReason = kMemSearchExitReason_Undefined;
    return NULL;
  }
  for (const char* cPtr = haystack; cPtr < haystack + haystackSize; cPtr++) {
    char c = *cPtr;
    // size_t i = cPtr - haystack;
    if (c == needle[0]) {
      if (out_memSearchExitReason) *out_memSearchExitReason = kMemSearchExitReason_SomePartsFound;
      bool found = true; // Assume true.
      for (const char* needlePtr = needle; needlePtr < needle + needleLen; needlePtr++, cPtr++) {
	char needleC = *needlePtr;
	if (cPtr >= haystack + haystackSize) {
	  if (out_memSearchExitReason) *out_memSearchExitReason = kMemSearchExitReason_NeedleRanOutAtEnd;
	  if (needleNext) *needleNext = needlePtr;
	  found = false;
	  return NULL;
	}
	c = *cPtr;
	if (c != needleC) {
	  found = false;
	  break;
	}
      }
      if (found) {
	  if (out_memSearchExitReason) *out_memSearchExitReason = kMemSearchExitReason_Found;
	  if (needleNext) *needleNext = needle;
	return cPtr - needleLen;
      }
    }
  }
  // Returns NULL if not found
  if (needleNext) *needleNext = needle;
  return NULL;
}

#ifndef MAIN_TEST_FN
#define MAIN_TEST_FN memsearch_test
#endif
int MAIN_TEST_FN() {
  const char* hay = "The testa";
  int reason;
  const char* needleNext;
  puts(memsearch_ext(hay, strlen(hay), "e", &reason, &needleNext));
  printf("Reason: %d\n\n", reason);
  
  puts(memsearch_ext(hay, strlen(hay), "ta", &reason, &needleNext));
  printf("Reason: %d\n\n", reason);
  
  puts(memsearch_ext(hay, strlen(hay), "tad", &reason, &needleNext));
  printf("Reason: %d\n", reason);
  printf("Needle next: %s\n\n", needleNext);
  
  puts(memsearch_ext(hay, strlen(hay), "testar", &reason, &needleNext));
  printf("Reason: %d\n", reason);
  printf("Needle next: %s\n\n", needleNext);
  
  return 0;
}
