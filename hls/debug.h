#ifndef VITIS_SYSTEM_DEBUG_H
#define VITIS_SYSTEM_DEBUG_H
#ifndef __SYNTHESIS__
// #include "spdlog/spdlog.h"
// #include "spdlog/sinks/basic_file_sink.h"
#endif

#ifndef __SYNTHESIS__
#define DBG(x) do { \
std::stringstream ss; \
ss  << __LINE__ << " [" << __func__ << "-" << id << "]: " << x; \
std::cerr << ss.str(); \
} while (0)
#define DBGI(x) do { \
std::stringstream ss;\
ss << __LINE__ << " [" << __func__ << "]: " << x; \
std::cerr << ss.str();                                               \
} while (0)
#else
#define DBG(x)
#define DBGI(x)
#endif

#endif //VITIS_SYSTEM_DEBUG_H
