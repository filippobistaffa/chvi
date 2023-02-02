#ifndef MAIN_HPP_
#define MAIN_HPP_

// Profiling
//#include <gperftools/profiler.h>
#define BREAKPOINT(MSG) do { puts(MSG); fflush(stdout); while (getchar() != '\n'); } while (0)

#endif
