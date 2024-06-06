#ifndef CRACON_LOG_HPP
#define CRACON_LOG_HPP

#include <cstdio>

#ifdef CRACON_ENABLE_LOG
#define CRACON_LOG_IMPL_DEBUG(...) fprintf(stdout, __VA_ARGS__)
#define CRACON_LOG_IMPL_INFO(...) fprintf(stdout, __VA_ARGS__)
#define CRACON_LOG_IMPL_WARNING(...) fprintf(stderr, __VA_ARGS__)
#define CRACON_LOG_IMPL_ERROR(...) fprintf(stderr, __VA_ARGS__)
#else
#define CRACON_LOG_IMPL_DEBUG(...) {}
#define CRACON_LOG_IMPL_INFO(...) {}
#define CRACON_LOG_IMPL_WARNING(...) {}
#define CRACON_LOG_IMPL_ERROR(...) {}
#endif

#define CRACON_LOG_DEBUG(...) CRACON_LOG_IMPL_DEBUG("[cracon] [DEBUG] " __VA_ARGS__)
#define CRACON_LOG_INFO(...) CRACON_LOG_IMPL_INFO("[cracon] [INFO] " __VA_ARGS__)
#define CRACON_LOG_WARNING(...) CRACON_LOG_IMPL_WARNING("[cracon] [WARN] " __VA_ARGS__)
#define CRACON_LOG_ERROR(...) CRACON_LOG_IMPL_ERROR("[cracon] [ERROR] " __VA_ARGS__)

#endif
