#pragma once
#include <cxxabi.h>    // for __cxa_demangle
#include <dlfcn.h>     // for dladdr
#include <execinfo.h>  // for backtrace

#include <sstream>
#include <string>

#include "fmt/format.h"

namespace ilog {
// This function produces a stack backtrace with demangled function & method names.
static inline std::string stacktrace(int skip = 1) {
  static const int precision = static_cast<int>(2 + sizeof(void *) * 2);
  void *           callstack[128];
  static const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);
  int              nFrames    = backtrace(callstack, nMaxFrames);
  char **          symbols    = backtrace_symbols(callstack, nFrames);

  std::string result;
  for (int i = skip; i < nFrames; i++) {
    Dl_info info;
    if (dladdr(callstack[i], &info)) {
      char *demangled  = nullptr;
      int   status     = -1;
      demangled        = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
      const char *name = status == 0 ? demangled : info.dli_sname;
      result += fmt::format("{:<3} {:>{}} {} + {}\n",
                            i,
                            callstack[i],
                            precision,
                            name ? name : "(null)",
                            static_cast<size_t>((char *)callstack[i] - (char *)info.dli_saddr));
      free(demangled);
    } else {
      result += fmt::format("{:<3} {:>{}}\n", i, precision, callstack[i]);
    }
    result += fmt::format("{}\n", symbols[i]);
  }
  free(symbols);
  if (nFrames == nMaxFrames)
    result += "[truncated]\n";
  return result;
}
};  // namespace ilog
