/****************************************************************************
 *
 * Copyright 2024 Samsung Electronics All Rights Reserved.
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
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>
#include <tinyara/mm/mm.h>
#include <tinyara/sched.h>
#include <tinyara/compression.h>

#include <mqueue.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <queue.h>
#include <debug.h>
#include <assert.h>

#include <tinyara/fs/mtd.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define PARTITION_BASE_ADDRESS   0x0665000
#define MAX_BUFFER_SAVE_SIZE     200

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/
static struct mtd_dev_s *dev_mtd = NULL;
static bool is_started_to_save = false;

/****************************************************************************
 * Public Variables
 ****************************************************************************/

/****************************************************************************
 * Private Variables
 ****************************************************************************/
static char buffer[MAX_BUFFER_SAVE_SIZE];
static int cur_index;
static int addr_offset;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void crash_dump_initialize()
{
    /* Initialize crash dump related things , some are follows:
     * 1. initialize dev_mtd, offset, is_started_to_save*/
    dev_mtd = up_flashinitialize();
    if (!dev_mtd) {
        printf("Unable to create up_flashinitilaize\n");
    } else {
        printf("dev_mtd initialized\n");
    }
    is_started_to_save = false;
    cur_index = 0;
    addr_offset = 0;
}

void crash_dump_start()
{
    is_started_to_save = true;
    g_lldbg_start = 1;
}

int crash_dump_save(int ch)
{
    /* Save each character from lldbg */
    if (is_started_to_save) {
        if (cur_index == MAX_BUFFER_SAVE_SIZE) {
            //int ret = MTD_WRITE(dev_mtd, PARTITION_BASE_ADDRESS + addr_offset, cur_index, (uint8_t *)buffer);
            int ret = amebasmart_flash_write( PARTITION_BASE_ADDRESS + addr_offset, (const uint8_t *)buffer, cur_index);
            addr_offset += cur_index;
            cur_index = 0;	
        }
        buffer[cur_index] = ch;
        cur_index++;
        return cur_index;
    } else {
        return -1;
    }
}

void crash_dump_stop() 
{
    /* Save partially left char of buffer to flash */
    is_started_to_save = false;
    if (cur_index > 0) {
        //int ret = MTD_WRITE(dev_mtd, PARTITION_BASE_ADDRESS + addr_offset, cur_index, (uint8_t *)buffer);
        int ret = amebasmart_flash_write( PARTITION_BASE_ADDRESS + addr_offset, (const uint8_t *)buffer, cur_index);
        addr_offset += cur_index;
        cur_index = 0;
    }
    g_lldbg_start = 0;
}

int crash_dump_read(char *buffer, int buflen)
{
    /* Store the flash crash dump data to buffer and return the size stored */
    int size = addr_offset;
    return size;
}
