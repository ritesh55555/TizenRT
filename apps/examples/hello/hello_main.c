/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 * examples/hello/hello_main.c
 *
 *   Copyright (C) 2008, 2011-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>
#include <tinyara/sched.h>
#include <tinyara/arch.h>
#include <stdio.h>
#include <mqueue.h>
#include <errno.h>
#include <debug.h>
#include <fcntl.h>

void fun(int size) {
	volatile int a[size];
	for (int i = 0; i < size; i++) {
		a[i] = 99999;
	}
}

void recursion() {
	volatile int a[500];
	int cnt = 0;
	int r = 0;
	while(1) {
		printf("stack base ptr of recursive thread before alloc is %p\n", *(uint32_t *)(sched_self()->stack_base_ptr));
		if (*(uint32_t *)(sched_self()->stack_base_ptr) != STACK_COLOR) {
			printf("stack base ptr of recursive thread before alloc is %p\n", *(uint32_t *)(sched_self()->stack_base_ptr));
			printf("stack base ptr of recursive thread before alloc is %p\n", *(uint32_t *)(sched_self()->stack_base_ptr));
			printf("stack base ptr of recursive thread before alloc is %p\n", *(uint32_t *)(sched_self()->stack_base_ptr));
			printf("stack base ptr of recursive thread before alloc is %p\n", *(uint32_t *)(sched_self()->stack_base_ptr));
			printf("stack base ptr of recursive thread before alloc is %p\n", *(uint32_t *)(sched_self()->stack_base_ptr));
			sleep(1);
		}
		for (int i = 0; i < 500; i++) {
			a[i] = 99999;
		}
		printf("stack base ptr of recursive thread after alloc is %p\n", *(uint32_t *)(sched_self()->stack_base_ptr));
		//printf("data in recursive thread is filled with cnt %d\n", cnt+1);
		for (int i = 1 ; i < 10000000; i++) {
			for (int j = 1 ; j < 10000000; j++) {
				for (int k = 1; k < 1000000; k++) {
					r++;
				}
			}
		}
		printf("cycle in recursive thread done with cnt %d\n", cnt);
		fun(cnt);
		cnt++;
	}
}

void normal_thread() {
	int r = 0;
	while(1) {
		//do nothing
		for (int i = 1 ; i < 10000000; i++) {
			for (int j = 1 ; j < 10000000; j++) {
				for (int k = 1; k < 1000000; k++) {
					r++;
				}
			}
		}
		printf("cycle in normal thread done\n");
		printf("stack base ptr of normal thread is %p\n", *(uint32_t *)(sched_self()->stack_base_ptr));
	}
}

/****************************************************************************
 * hello_main
 ****************************************************************************/

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int hello_main(int argc, char *argv[])
#endif
{
	printf("Hello, World!!\n");
	//recursion();

	if (argc == 2) {
		int *p = (int *)malloc(sizeof(int) * 10);
		int pid = kernel_thread("normal_thread", 200, 4096, normal_thread, NULL);
		if (pid < 0) {
			printf("Failed to start normal_thread");
		}
		int *pl = (int *)malloc(sizeof(int) * 10);
		int pdid = kernel_thread("recusrsive_thread", 200, 4096, recursion, NULL);
		if (pdid < 0) {
			printf("Failed to start recursive_thread");
		}
	}

	return 0;
}
