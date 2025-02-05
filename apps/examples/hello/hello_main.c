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
#include <stdio.h>
#include <tinyara/arch.h>
#include <tinyara/sched.h>

void thread1_task(void)
{
	//do nothing
	printf("Inside thread1_task\n");
	/*for (int i = 0; i < CONFIG_MAX_TASKS; i++) {
		printf("%d  ->  %s\n", pid_to_task_name[i]);
	}
	printf("Task name print done\n");*/
}

void thread2_task(void)
{
	//do nothing
	printf("Inside thread2_task\n");
	int *p = (int *)malloc(sizeof(int) * 1024 * 100 * 100 * 1024);
	printf("Memory allocated wihtout freeing\n");
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

	pthread_t thread1;
	pthread_t thread2;
	
	// if (argc == 2) {
	// 	pthread_create(&thread2, NULL, thread2_task, NULL);
	// } else {
	// 	pthread_create(&thread1, NULL, thread1_task, NULL);
	// }

	if (argc == 2) {
		//do nothing
		sched_addDeadtaskinfo(50, "ritesh");
		sched_addDeadtaskinfo(51, "manoj");
		sched_addDeadtaskinfo(52, "aditya");
		sched_addDeadtaskinfo(53, "ritesh");
		sched_addDeadtaskinfo(54, "aditya");
		sched_addDeadtaskinfo(55, "ranjan");
		sched_addDeadtaskinfo(60, "anirudh");
		sched_addDeadtaskinfo(61, "anirudh");

		print_dead_structure();

		printf("Trying to get some name of dead pid:\n");
		printf("50 -> %s\n", sched_getdeadtaskname(50));
		printf("60 -> %s\n", sched_getdeadtaskname(60));
		printf("53 -> %s\n", sched_getdeadtaskname(53));
		printf("3 -> %s\n", sched_getdeadtaskname(3));
		printf("1000 -> %s\n", sched_getdeadtaskname(1000));
		printf("\n");

		printf("Replaceing some dead pid (53, 51, 1000, 2, 1) if present\n");
		sched_checkDeadPid(53);
		sched_checkDeadPid(51);
		sched_checkDeadPid(1000);
		sched_checkDeadPid(2);
		sched_checkDeadPid(1);

		print_dead_structure();

	}

	if (argc == 3) {
		pthread_create(&thread2, NULL, thread2_task, NULL);
	}

	return 0;
}
