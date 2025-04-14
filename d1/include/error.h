#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

static void RT_LogV(RT_LogSeverity a_serverity, const char *a_message, va_list va_format_list) {
	size_t buffersize = vsnprintf(NULL, 0, a_message, va_format_list) + 1;
	char* formatted_message = (char*)malloc(buffersize);
	vsnprintf(formatted_message, buffersize, a_message, va_format_list);

	PrintMessage(a_serverity, formatted_message, NULL, 0);

	free(formatted_message);
}

static inline void Error(const char *msg, ...) {
	va_list vp;
	va_start(vp, msg);
	RT_LogV(RT_LOGSERVERITY_HIGH, msg, vp);
	va_end(vp);
}

static inline void Warning(const char *msg, ...) {
	va_list vp;
	va_start(vp, msg);
	RT_LogV(RT_LOGSERVERITY_MEDIUM, msg, vp);
	va_end(vp);
}

#endif
