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

#include <stdio.h>
#include <fcntl.h>   // For O_* constants

#include "platform-posix.h"

sem_t * platform_semaphore_create(void)
{
    sem_t * sem = sem_open("/datastore", O_CREAT /* | O_EXCL*/, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
    }
    return sem;
}

void platform_semaphore_delete(sem_t * sem)
{
    sem_close(sem);
    sem_unlink("/datastore");
}

void platform_semaphore_take(sem_t * sem)
{
    if (sem_wait(sem) != 0)
    {
        perror("sem_wait");
    }
}

void platform_semaphore_give(sem_t * sem)
{
    if (sem_post(sem) != 0)
    {
        perror("sem_post");
    }
}
