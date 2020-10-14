#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define YAZ_WINDOWS
#elif __APPLE__
    #define YAZ_MACOSX
#elif __linux__
    #define YAZ_LINUX
#elif __FreeBSD__
    #define YAZ_FREEBSD
#else
#   error "Unknown platform"
#endif