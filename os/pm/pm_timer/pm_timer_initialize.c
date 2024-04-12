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
/************************************************************************
 * pm/pm_timer/pm_timer_initialize.c
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
 ************************************************************************/

/************************************************************************
 * Included Files
 ************************************************************************/

#include <tinyara/config.h>
#include <tinyara/pm/pm.h>

#include "pm_timer.h"

/************************************************************************
 * Pre-processor Definitions
 ************************************************************************/

/************************************************************************
 * Private Type Declarations
 ************************************************************************/

/************************************************************************
 * Public Variables
 ************************************************************************/

/* The g_pmTimer_freeList is a singly linked list of pm timers available
 * to the system */

sq_queue_t g_pmTimer_freeList;

/* The g_pmTimer_activeList data structure is a singly linked list ordered by
 * pm wakeup timer expiration time.
 */

sq_queue_t g_pmTimer_activeList;

/* This is the number of free, pre-allocated pm wakeup timer structures in the
 * g_pmTimer_freeList. 
 */

uint16_t g_pmTimer_nfree;

/* g_pm_list is a list of pre-allocated pm wakeup timers. The number of pm timers
 * in the list is a configuration item.
 */

pm_wakeup_timer_t g_pmTimer_pool[CONFIG_PM_MAX_WAKEUP_TIMER];

/* This is a map from process ids to their respective pm lock status. 
 * This will be used to lock the pm after pm timer expire and unlock again
 * when the process completes it work 
 * */

uint8_t is_pm_lock[CONFIG_MAX_TASKS];

/************************************************************************
 * Private Functions
 ************************************************************************/

/************************************************************************
 * Public Functions
 ************************************************************************/

/****************************************************************************
 * Name: pm_timer_initialize
 *
 * Description:
 *   This function will initialize a static array of pm wakeup timers that will be
 *   used in the g_pmTimer_activeList. 
 *  
 *   This function should be called when OS starts.
 * 
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None.
 *
 ****************************************************************************/

void pm_timer_initialize(void)
{
        pm_wakeup_timer_t *timer = g_pmTimer_pool;

        for (int i = 0; i < CONFIG_MAX_TASKS; i++) {
                is_pm_lock[i] = PM_PID_NONE;
        }

        /* Initialize pm timer list */

        sq_init(&g_pmTimer_freeList);
        sq_init(&g_pmTimer_activeList);

        /* The g_pmTimer_list must be initiated */
        for (int i = 0; i < CONFIG_PM_MAX_WAKEUP_TIMER; i++) {
                sq_addlast((pm_wakeup_timer_t *)timer++, &g_pmTimer_freeList);
        }

        /* All pm timers are free */
        g_pmTimer_nfree = CONFIG_PM_MAX_WAKEUP_TIMER;

}
