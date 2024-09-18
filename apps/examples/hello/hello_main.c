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
#include <tinyara/fs/mtd.h>
#include <errno.h>

/****************************************************************************
 * hello_main
 ****************************************************************************/
int g_lldbg_start = 0;
int base = 0x0665000;

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int hello_main(int argc, char *argv[])
#endif
{
	printf("Hello, World!!\n");
	int ret;
	long arg = 0;

	if (argc == 2) {
		char *p;
    	arg = strtol(argv[1], &p, 10);
	}

	if (arg == 6) {
		if (!g_dev_mtd) {
			g_dev_mtd = up_flashinitialize();
			if (!g_dev_mtd) {
				printf("Unable to create up_flashinitilaize\n");
			} else {
				printf("g_dev_mtd initialized\n");
			}
		}
	}

	if (arg == 5) {
		printf("Crash using accessing null pointer\n");
		int *p = NULL;
		printf("p value is %d\n", *p);
	}	

	if (arg == 1) {
		g_lldbg_start = 1;
		/*int *ptr = NULL;
		printf("print %d\n", *ptr);*/
		lldbg("hi there this is text for testing lldbg getting saved in flash or not!!!\n");
		lldbg_noarg("Lets see if it gets stored\n");
		lldbg("have a good day");
	}
	if (arg == 2) {
		/* Read from FLASH */
		FAR struct mtd_dev_s *dev_mtd = NULL;
		char *buffer = (char*)malloc(sizeof(char) * 200);
		int buflen = 0;

		dev_mtd = up_flashinitialize();
		if (!dev_mtd) {
			printf("up_flashinitialize failed\n");
			return 0;
		}
		ret = MTD_READ(dev_mtd, base, 200, (const uint8_t *)buffer);
		if (ret <= 0) {
			printf("ret from MTD_READ failed %d\n", ret);
			return 0;
		}
		printf("ret from MTD_READ is %d\n", ret);
		printf("Data read from MTD is as follows:\n");
		for (int i = 0; i < 200; i++ ){
			printf("%c", buffer[i]);
		}
		printf("\n");
	}
	if (arg == 3) {
		/* Wrtie to FLASH */
		FAR struct mtd_dev_s *dev_mtd = NULL;

		dev_mtd = up_flashinitialize();
		if (!dev_mtd) {
			printf("up_flashinitialize failed\n");
			return 0;
		}
		printf("base is %d\n", (base/4096) + 1);
		ret = MTD_ERASE(dev_mtd, (base/4096), 2);
		if (ret < 0) {
			printf("failed MTD erase ret %d\n", ret);
			return 0;
		}
		printf("ret from MTD_ERASE %d\n", ret);
		/*ret = MTD_WRITE(dev_mtd, base, 20, (const uint8_t *)buffer);
		if (ret <= 0) {
			printf("ret from MTD_WRITE failed %d\n", ret);
			return 0;
		}
		printf("ret from MTD_WRITE is %d\n", ret);
		printf("Data WROTE to MTD is as follows:\n");
		for (int i = 0; i < 20; i++ ){
			printf("%c", buffer[i]);
		}*/
		printf("\n");
	}
	if (arg == 4) {
		/* Wrtie to FLASH */
		FAR struct mtd_dev_s *dev_mtd = NULL;
		char buffer[] = "my name is ritesh!!!";

		dev_mtd = up_flashinitialize();
		if (!dev_mtd) {
			printf("up_flashinitialize failed\n");
			return 0;
		}
		
		ret = MTD_WRITE(dev_mtd, base, 20, (const uint8_t *)buffer);
		if (ret <= 0) {
			printf("ret from MTD_WRITE failed %d\n", ret);
			return 0;
		}
		printf("ret from MTD_WRITE is %d\n", ret);
		printf("Data WROTE to MTD is as follows:\n");
		for (int i = 0; i < 20; i++ ){
			printf("%c", buffer[i]);
		}
		printf("\n");
	}
	
	return 0;
}
