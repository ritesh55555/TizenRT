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
 * pm/pm_timer/pm_timer.h
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

#ifndef __PM_TIMER_H
#define __PM_TIMER_H

/************************************************************************
 * Included Files
 ************************************************************************/

#include <tinyara/config.h>

#include <assert.h>
#include <debug.h>
#include <stdint.h>
#include <stdbool.h>
#include <queue.h>

#define PM_TIMER_FAIL     -1
#define PM_TIMER_SUCCESS   0

/************************************************************************
 * Pre-processor Definitions
 ************************************************************************/

/************************************************************************
 * Public Type Declarations
 ************************************************************************/

/************************************************************************
 * Public Variables
 ************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/* The g_pmTimer_freeList is a singly linked list of pm timers available
 * to the system */

extern sq_queue_t g_pmTimer_freeList;

/* The g_pmTimer_activeList data structure is a singly linked list ordered by
 * pm wakeup timer expiration time.
 */

extern sq_queue_t g_pmTimer_activeList;

/* This is the number of free, pre-allocated pm wakeup timer structures in the
 * g_pmTimer_freeList. 
 */

extern uint16_t g_pmTimer_nfree;

/* This is a map from process ids to their respective pm lock status. 
 * This will be used to lock the pm after pm timer expire and unlock again
 * when the process completes it work 
 * */

extern uint8_t is_pm_lock[CONFIG_MAX_TASKS];

enum pm_pid_lock_status_e {
    PM_PID_NONE = 0,        /* This means timer is not used */
    PM_PID_LOCK,            /* This means pm transition is locked for the pid */
    PM_PID_UNLOCK,          /* This means pm transition is relaxed for the pid */
};

/************************************************************************
 * Public Function Prototypes
 ************************************************************************/

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif							/* __PM_TIMER_H */
