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
 * pm/pm_timer/pm_timer_activity.c
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

#include <tinyara/pm/pm.h>
#include <tinyara/irq.h>

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

/************************************************************************
 * Private Functions
 ************************************************************************/

/************************************************************************
 * Public Functions
 ************************************************************************/

/****************************************************************************
 * Name: pm_set_wakeup_timer
 *
 * Description:
 *   This function decides which pm wakeup timer to be set just before sleep.
 *   It finds the wakeup timer with nearest future expiration time and sets it.
 *   When the board goes to sleep , it starts the timer and sleeps.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   0 - system can go to sleep
 *   -1 - system should not go to sleep
 *
 ****************************************************************************/

int pm_set_wakeup_timer(void)
{
	/* if no timer present , do we need to wake up board ??? */
	if (g_pmTimer_activeList.head == NULL) {
		pmdbg("the g_pmTimer_activeList list is empty!!!\n");
		return PM_TIMER_SUCCESS;
	}

	pm_wakeup_timer_t *curr;
    	pm_wakeup_timer_t *prev;
    	prev = curr = (pm_wakeup_timer_t *)g_pmTimer_activeList.head;

	unsigned int now = clock_systimer();

	/* Remove the wakeup timers that are already expired. */
	while (curr->expire_timetick < now && curr->next) {
		prev = curr;
		curr = curr->next;
		remove_pm_timer(get_pm_timer(prev->id));
		pmdbg("removed timer with id %d from the list \n", prev->id);

		/* system should not go to sleep now, should do pm lock until
		 * removed timer's work get done */
		if (prev->is_periodic) {
			pm_stay(PM_IDLE_DOMAIN, PM_NORMAL);
			prev->is_pm_lock = true;
			pmdbg("setting pm lock for timer with id %d\n", prev->id);
			return PM_TIMER_FAIL;
		}
		/* if timer was not periodic, no need to lock*/ 
		else {
			prev->status = FREE;
			g_pmTimer_nfree++;
			DEBUGASSERT(g_pmTimer_nfree <= CONFIG_PM_MAX_WAKEUP_TIMER);
		}
		
	}

	if (curr->expire_timetick > now) {
		pmdbg("setting timer with id %d\n", curr->id);
		pmdbg("Board will wake up after %d millisecond\n", (curr->expire_timetick - now));
		pm_set_timer(PM_WAKEUP_TIMER, (curr->expire_timetick - now) * 1000);
		curr->status = RUNNING;
	} 
	/* Case where every timer expire time is less than current time. 
	 * No need to start any timer, But do we need to wake up after a fixed timer? */
	else {
		pmdbg("every timer expire time is less than current time, last timer id is %d\n", curr->id);
		remove_pm_timer(get_pm_timer(curr->id));
	}
	
	return PM_TIMER_SUCCESS;
}

/****************************************************************************
 * Name: pm_timer_callback
 *
 * Description:
 *   This function runs after any of the wakeup timers expire . 
 *   It will be used to remove the latest running wakeup timer from the 
 *   g_pmTimer_activeList.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None.
 *
 ****************************************************************************/

void pm_timer_callback(void)
{
	/* Check to see if the list is empty which should not be the case */
	if (g_pmTimer_activeList.head == NULL) {
		pmdbg("the linked list is empty!!!\n");
		return;
	}
	pm_wakeup_timer_t *head;
    	head = (pm_wakeup_timer_t *)g_pmTimer_activeList.head;

	/* Removing the first timer from the list because that is the one we started
	 * before sleep. */
	sq_remfirst(&g_pmTimer_activeList);
	head->status = INACTIVE;

	/* Making sure to lock pm until the app completes its timer related work */
	if (head->is_periodic) {
		pm_stay(PM_IDLE_DOMAIN, PM_NORMAL);
		head->is_pm_lock = true;
		pmdbg("setting pm lock for timer with id %d\n", head->id);
	}
	/* if timer was not periodic, no need to lock*/ 
	else {
		head->status = FREE;
		g_pmTimer_nfree++;
		DEBUGASSERT(g_pmTimer_nfree <= CONFIG_PM_MAX_WAKEUP_TIMER);
	}
}
