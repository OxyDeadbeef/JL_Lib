/* Lib Aldaron --- Copyright (c) 2016 Jeron A. Lau */
/* This file must be distributed with the GNU LESSER GENERAL PUBLIC LICENSE. */
/* DO NOT REMOVE THIS NOTICE */

#ifndef LA_THREAD
#define LA_THREAD

#include <stdint.h>

typedef int (*la_thread_fn_t)(void* data);

typedef struct {
	la_thread_fn_t fn; // (SDL_ThreadFunction) Pointer to the function
	void* thread; // (SDL_Thread) Pointer to the SDL thread
	int64_t id; // (SDL_threadID) Integer to define thread.
} la_thread_t;

const char* la_thread_new(la_thread_t* thread, la_thread_fn_t fn,
	const char* name, void* data);
int32_t la_thread_old(la_thread_t* thread);
uint64_t la_thread_current(void);

#endif
