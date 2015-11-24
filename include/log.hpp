#ifndef _LOG_H
#define _LOG_H

#include <iostream>

#if !defined(LOG_THIS_FILE)
#define LOG_THIS_FILE 1
#endif

#if LOG_THIS_FILE
#define LOG(x) (std::cout << "[LOG]" << __FILE__ << ":" << __LINE__ << ": " << x << std::endl)
#else
#define LOG(x)
#endif

#if !defined(LOG_ERROR_THIS_FILE)
#define LOG_ERROR_THIS_FILE 1
#endif

#if LOG_ERROR_THIS_FILE
#define ERROR(x) (std::cerr << "[ERROR]" << __FILE__ << ":" << __LINE__ << ": " << x << std::endl)
#else
#define ERROR(x)
#endif

#if !defined(LOG_WARN_THIS_FILE)
#define LOG_WARN_THIS_FILE 1
#endif

#if LOG_WARN_THIS_FILE
#define WARN(x) (std::cerr << "[WARN]" << __FILE__ << ":" << __LINE__ << ": " << x << std::endl)
#else
#define WARN(x)
#endif

#endif
