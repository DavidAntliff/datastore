/*
 * MIT License
 *
 * Copyright (c) 2018 David Antliff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef PLATFORM_POSIX_H
#define PLATFORM_POSIX_H

#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

#define platform_error(f, ...)   do { fprintf(stdout /*stderr*/, f"\n", ##__VA_ARGS__); } while (0)
#define platform_warning(f, ...) do { fprintf(stdout, f"\n", ##__VA_ARGS__); } while (0)
#define platform_info(f, ...)    do { fprintf(stdout, f"\n", ##__VA_ARGS__); } while (0)
#define platform_debug(f, ...)   do { /*fprintf(stdout, f"\n", ##__VA_ARGS__);*/ } while (0)
#define platform_hexdump(P, S)

typedef sem_t * platform_semaphore_t;
#define platform_semaphore_create()   sem_open("datastore", O_CREAT | O_EXCL)
#define platform_semaphore_delete(S)  do { sem_close(S); } while (0)
#define platform_semaphore_take(S)    sem_wait(S)
#define platform_semaphore_give(S)    sem_post(S)


#ifdef __cplusplus
}
#endif

#endif // PLATFORM_POSIX_H

