#include <stdlib.h>

enum MemSearchExitReason {
  kMemSearchExitReason_Undefined,
  kMemSearchExitReason_NeedleRanOutAtEnd, // Needle ran out at the end of the haystack. If this is
					  // set, then const char* needleNext is set to where to
					  // continue looking at from here in the needle on the next
					  // call to memsearch.
  kMemSearchExitReason_NoPartsFound, // Not even the first letter was found.
  kMemSearchExitReason_SomePartsFound,
  kMemSearchExitReason_Found // It was found
};

// Extended version of `memsearch`.
// Call and if it returns Found, re-run with the return value as the haystack (and subtract haystackSize by `(retval - haystack)`) until it returns needle ran out or no parts found.
extern const char* memsearch_ext(const char* haystack, size_t haystackSize, const char* needle, size_t needleLen, int* out_memSearchExitReason, const char** needleNext);

// strstr but can handle haystacks that aren't limited to null termination. Useful when your string
// can indeed contain null terminators out in the middle of it.
inline const char* memsearch(const char* haystack, size_t haystackSize, const char* needle, size_t needleLen) {
  return memsearch_ext(haystack, haystackSize, needle, needleLen, NULL, NULL);
}

extern const char* memsearch_reasonToString(int reason);
