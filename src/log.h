#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "config_board.h"

#ifdef CONFIG_NDEBUG
#define log_debug(fmt, ...)     do{} while(0)
#define log_error(fmt, ...)       do{} while(0)
#define log_warn(fmt, ...)       do{} while(0)
#define log_info(fmt, ...)       do{} while(0)
#else
#define log_debug(fmt, ...) \
        printf("[DEBUG] %s:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define log_error(fmt, ...) \
        printf("[ERROR] %s:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define log_warn(fmt, ...) \
        printf("[WARN] %s:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define log_info(fmt, ...) \
        printf("[INFO] %s:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif