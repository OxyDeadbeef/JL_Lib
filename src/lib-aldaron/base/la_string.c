/* Lib Aldaron --- Copyright (c) 2016 Jeron A. Lau */
/* This file must be distributed with the GNU LESSER GENERAL PUBLIC LICENSE. */
/* DO NOT REMOVE THIS NOTICE */

#include <la_string.h>
#include <la_memory.h>

#include <stdio.h>
#include <string.h>

const char* la_string_ffloat(float value) {
	void* instant = la_memory_instant();
	sprintf(instant, "%f", value);
	return instant;
}

const char* la_string_fint(int32_t value) {
	void* instant = la_memory_instant();
	sprintf(instant, "%d", value);
	return instant;
}

void la_string_append(char* string, const char* appender) {
	la_memory_copy(appender, string + strlen(string), strlen(appender));
}

uint32_t la_string_upto(const char* string, char chr) {
	int i = 0;

	for(; i < strlen(string); i++) {
		if(string[i] == chr) return i;
	}
	return strlen(string);
}

uint8_t la_string_next(const char* string, const char* match) {
	return strstr(string, match) == string;
}
