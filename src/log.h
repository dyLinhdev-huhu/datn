#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <string.h>
#include <Arduino.h>

// #define getSysTicks()               millis()
// #define debug_sendBuffer(bufer)     Serial.println(bufer)

#define LOG_SIZE_DEFAULT    512

#define KNRM  "\x1B[0m "
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define log_print(type, fmt, args...) do { \
	snprintf(LOG_BUFF, LOG_SIZE - 1, "%s[%s:%03d] " fmt KNRM "\r\n", type, __FILENAME__, __LINE__, ##args); \
    Serial.write(LOG_BUFF); \
} while (0)

#define log_prints(type, fmt, args...) do { \
	snprintf(LOG_BUFF, LOG_SIZE - 1, fmt KNRM, ##args); \
    Serial.write(LOG_BUFF); \
} while (0)

#define logi(fmt, args...) log_print("\x1B[32m[I]", fmt, ##args)
#define logw(fmt, args...) log_print("\x1B[33m[W]", fmt, ##args)
#define loge(fmt, args...) log_print("\x1B[31m[E]", fmt, ##args)

#define logis(fmt, args...) log_prints("\x1B[32m[I]", fmt, ##args)
#define logws(fmt, args...) log_prints("\x1B[33m[W]", fmt, ##args)
#define loges(fmt, args...) log_prints("\x1B[31m[E]", fmt, ##args)

extern char* LOG_BUFF;
extern int LOG_SIZE;

void log_init();
void log_deInit(void);
// void debug_sendBuffer(uint8_t* buffer);
// void debug_sendChar(char c);

#endif