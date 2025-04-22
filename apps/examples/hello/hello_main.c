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
#include <fcntl.h>
#include <tinyara/fs/ioctl.h>
#include <sys/types.h>
#include <tinyara/mminfo.h>
#include <debug.h>
#include <tinyara/mqueue.h>

#define MQ_NONE_STR  "none"
#define MQ_SEND_STR  "send"
#define MQ_RECEIVE_STR  "receive"

extern struct mq_debug_info g_mq_debug_info[CONFIG_MAX_TASKS];
extern int mq_debug_cnt;


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

	if (argc == 2) {
		printf("Printing g_mq_debug_info list data:\n\n");

		printf("  mq_des     |  name       | thread pid | open address |  mq_type  | mq_type_call_addr | is waiting | send cnt | recv cnt  \n");
		printf("*****************************************************************************************************************************\n");
		for (int i = 0; i < mq_debug_cnt; i++) {
			char *str;
			if (g_mq_debug_info[i].curr_type == MQ_NONE) {
				str = MQ_NONE_STR;
			} else if (g_mq_debug_info[i].curr_type == MQ_SEND) {
				str = MQ_SEND_STR;
			} else {
				str = MQ_RECEIVE_STR;
			}
			printf(" %p  |  %-10s | %-10d |  0x%08x  |  %-8s |    0x%08x     |  %-8s  | %-7d  | %-8d \n", g_mq_debug_info[i].mq_des, g_mq_debug_info[i].mq_des->msgq->inode->i_name, g_mq_debug_info[i].pid, g_mq_debug_info[i].open_call_addr, str, g_mq_debug_info[i].curr_type_call_addr, g_mq_debug_info[i].is_waiting == true ? "yes" : "no", g_mq_debug_info[i].send_cnt, g_mq_debug_info[i].receive_cnt);
		}
	}

	return 0;
}
