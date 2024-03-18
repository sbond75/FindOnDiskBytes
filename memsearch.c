#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "memsearch.h"

// haystack: abcaabc
// needle: aabc

// haystacks: ca|ab|c
// needle: aabc

// haystacks: ca|ab|aa|bc
// needle: aabc

const char* memsearch_ext(const char* haystack, size_t haystackSize, const char* needle, size_t needleLen, int* out_memSearchExitReason, const char** needleNext) {
  const char* hayPtr = haystack;
  const char* needlePtr = needle;
  size_t i = 0;
  size_t matchedCharCount = 0;
  size_t matchedCharCount_needleNext = (needleNext && *needleNext) ? (*needleNext - needle) : 0;
  for (; i < haystackSize && i < needleLen; i++, hayPtr++, needlePtr++) {
    const char cHay = *hayPtr;
    const char cNeedle = *needlePtr;
    if (cHay == cNeedle) {
      // Found a possible match. Keep scanning for more; increment matchedCharCount
      matchedCharCount++;
    }
    else {
      // No match:
      // Stay at the current position in the needle
      i -= matchedCharCount + 1;
      
      // Reset matchedCharCount to 0
      needlePtr = needle;
      matchedCharCount = 0;
      
      // Stay at the current position in the needle
      needlePtr--;
    }
    
    // Check for matches with `needleNext`:
    if ((needleNext && *needleNext) && matchedCharCount_needleNext < needleLen && **needleNext == cHay) {
      // Found a possible match.
      matchedCharCount_needleNext++;
      (*needleNext)++;
    }
    else {
      // No match
      matchedCharCount_needleNext = 0;
      if (needleNext) {
      	*needleNext = NULL;
      }
    }
    
    // If we reached the end of the needle, reset needleNext and return the match:
    if (matchedCharCount == needleLen || matchedCharCount_needleNext == needleLen) {
      if (needleNext) {
      	*needleNext = NULL;
      }
      if (out_memSearchExitReason) {
        *out_memSearchExitReason = kMemSearchExitReason_Found;
      }
      return hayPtr - i;
    }
    
    // If we reached the end of the hay, set needleNext and return the partial match:
    if (hayPtr == haystack + haystackSize - 1) {
      if ((needlePtr + 1) >= needlePtr + needleLen) {
        // Needle exhausted but we're also at the end of the hay
        if (needleNext) {
          *needleNext = NULL;
        }
        if (out_memSearchExitReason) {
	  *out_memSearchExitReason = kMemSearchExitReason_NoPartsFound;
        }
        // This will return one-past the end of the string (if the string is null terminated this is the null terminator's address)
      	return hayPtr - i;
      }
      else {
        if ((hayPtr - ((needleNext && *needleNext) ? matchedCharCount_needleNext : matchedCharCount)) == haystack) {
          // No matches found
          if (needleNext) {
            *needleNext = NULL;
          }
          if (out_memSearchExitReason) {
	    *out_memSearchExitReason = kMemSearchExitReason_NoPartsFound;
          }
          return haystack;
        }
        
        if (needleNext) {
          *needleNext = needlePtr + 1;
        }
        if (out_memSearchExitReason) {
	  *out_memSearchExitReason = kMemSearchExitReason_SomePartsFound;
        }
      	return hayPtr - i;
      }
    }
  }
  
  // If we reached the end with matchedCharCount > 0, then we could have more in the next call to memsearch_ext, so save a "continuation":
  if (needleNext && matchedCharCount > 0) {
    *needleNext = needlePtr;
  }
  // If we had needle matches, these are also saved, with higher priority than the above:
  if (needleNext && matchedCharCount_needleNext > 0) {
    *needleNext = needlePtr;
  }
  
  if (out_memSearchExitReason) {
    *out_memSearchExitReason = hayPtr > (haystack + haystackSize - 1) ? kMemSearchExitReason_NoPartsFound : kMemSearchExitReason_NeedleRanOutAtEnd;
  }
  return hayPtr - ((needleNext && *needleNext) ? matchedCharCount_needleNext : matchedCharCount);
}

const char* memsearch_reasonToString(int reason) {
  switch (reason) {
  case kMemSearchExitReason_Undefined:
    return "undefined";
  case kMemSearchExitReason_NeedleRanOutAtEnd:
    return "needle ran out at end";
  case kMemSearchExitReason_NoPartsFound:
    return "no parts found";
  case kMemSearchExitReason_SomePartsFound:
    return "some parts found";
  case kMemSearchExitReason_Found:
    return "found";
  default:
    return "unknown";
  }
}

// https://stackoverflow.com/questions/70192113/how-to-call-macro-that-uses-token-pasting
#define STR(x) #x
#define XSTR(x) STR(x)

size_t testsFailed;
#define ensure(check) { \
  if (!(check)) { \
    puts("Expected: " XSTR(check) "."); \
    testsFailed++; \
  } \
}

#ifndef MAIN_TEST_FN
#define MAIN_TEST_FN memsearch_test
#endif
int MAIN_TEST_FN() {
  testsFailed = 0; // Assume 0.
  
  const char* hay = "The testa";
  int reason;
  const char* needleNext;
  const char* res = memsearch_ext(hay, strlen(hay), "e", 1, &reason, &needleNext);
  #define putsN(x) puts(x == NULL ? "null" : x)
  putsN(res);
  printf("Reason: %s\n", memsearch_reasonToString(reason));
  ensure(reason == kMemSearchExitReason_Found);
  ensure(strcmp(res, "e testa") == 0);
  printf("\n");
  
  res = memsearch_ext(hay, strlen(hay), "ta", 2, &reason, &needleNext);
  putsN(res);
  printf("Reason: %s\n", memsearch_reasonToString(reason));
  ensure(reason == kMemSearchExitReason_Found);
  ensure(strcmp(res, "ta") == 0);
  printf("\n");
  
  const char* needle = "tad";
  res = memsearch_ext(hay, strlen(hay), needle, strlen(needle), &reason, &needleNext);
  putsN(res);
  printf("Reason: %s\n", memsearch_reasonToString(reason));
  printf("Needle next: %s\n", needleNext);
  ensure(reason == kMemSearchExitReason_SomePartsFound);
  ensure(strcmp(res, "ta") == 0);
  printf("\n");
  
  res = memsearch_ext(hay, strlen(hay), "testar", 6, &reason, &needleNext);
  putsN(res);
  printf("Reason: %s\n", memsearch_reasonToString(reason));
  printf("Needle next: %s\n", needleNext);
  ensure(reason == kMemSearchExitReason_SomePartsFound);
  ensure(strcmp(res, "testa") == 0);
  printf("\n");
  
  // The above is single-call tests, fairly basic. Now to try calling again repeatedly to find all matches:
  {
  	  needle = "e";
	  res = memsearch_ext(hay, strlen(hay), needle, strlen(needle), &reason, &needleNext);
	  putsN(res);
	  printf("Reason: %s\n", memsearch_reasonToString(reason));
	  printf("Needle next: %s\n", needleNext);
	  ensure(reason == kMemSearchExitReason_Found);
	  ensure(strcmp(res, "e testa") == 0);
	  printf("\n");
	  
	  res = memsearch_ext(res+strlen(needle), strlen(hay) - (res - hay) - strlen(needle), needle, strlen(needle), &reason, &needleNext);
	  putsN(res);
	  printf("Reason: %s\n", memsearch_reasonToString(reason));
	  printf("Needle next: %s\n", needleNext);
	  ensure(reason == kMemSearchExitReason_Found);
	  ensure(strcmp(res, "esta") == 0);
	  printf("\n");
	  
	  res = memsearch_ext(res+strlen(needle), strlen(hay) - (res - hay) - strlen(needle), needle, strlen(needle), &reason, &needleNext);
	  putsN(res);
	  printf("Reason: %s\n", memsearch_reasonToString(reason));
	  printf("Needle next: %s\n", needleNext);
	  ensure(reason == kMemSearchExitReason_NoPartsFound);
	  //ensure(reason == kMemSearchExitReason_SomePartsFound);
	  ensure(strcmp(res, "") == 0);
	  printf("\n");
  }
  
  // Like the above but with longer needle string:
  {
  	  hay = "The ultimate teesta 1ee2";
  	  
  	  needle = "ee";
	  res = memsearch_ext(hay, strlen(hay), needle, strlen(needle), &reason, &needleNext);
	  putsN(res);
	  printf("Reason: %s\n", memsearch_reasonToString(reason));
	  printf("Needle next: %s\n", needleNext);
	  ensure(reason == kMemSearchExitReason_Found);
	  ensure(strcmp(res, "eesta 1ee2") == 0);
	  printf("\n");
	  
	  res = memsearch_ext(res+strlen(needle), strlen(hay) - (res - hay) - strlen(needle), needle, strlen(needle), &reason, &needleNext);
	  putsN(res);
	  printf("Reason: %s\n", memsearch_reasonToString(reason));
	  printf("Needle next: %s\n", needleNext);
	  ensure(reason == kMemSearchExitReason_Found);
	  ensure(strcmp(res, "ee2") == 0);
	  printf("\n");
	  
	  res = memsearch_ext(res+strlen(needle), strlen(hay) - (res - hay) - strlen(needle), needle, strlen(needle), &reason, &needleNext);
	  putsN(res);
	  printf("Reason: %s\n", memsearch_reasonToString(reason));
	  printf("Needle next: %s\n", needleNext);
	  ensure(reason == kMemSearchExitReason_NoPartsFound);
	  //ensure(strcmp(res, "") == 0);
	  ensure(strcmp(res, "2") == 0);
	  printf("\n");
  }
  
  // Now try the most advanced form: reading a block and then continuing onto the next one:
  
  if (testsFailed == 0) {
    puts("All tests passed.");
  }
  else {
    printf("%zu test(s) failed.\n", testsFailed);
  }
  return 0;
}
