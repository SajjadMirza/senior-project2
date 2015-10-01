#ifndef _LOG_H
#define _LOG_H

#include <iostream>

#define LOG(x) (std::cout << __FILE__ << ":" << __LINE__ << ": " << x << std::endl)
#define ERROR(x) (std::cerr << "!! ERROR: " << __FILE__ << __LINE__ << ": " << x << std::endl)

#endif
