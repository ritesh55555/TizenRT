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

#include <tinyara/fs/fs.h>
#include <tinyara/fs/ioctl.h>
#include <tinyara/pm/pm.h>
#include <tinyara/clock.h>

#include <errno.h>
#include <debug.h>
#include <assert.h>

#define PM_DRVPATH           "/dev/pm"
#define PM_NO_TIMER_USING    -1

/* Added to record timer countdown interrupt */
struct pm_timer_s g_pm_timer = {0, 0};

/* Map to store pm timer id for each process */
static int pid_pmTimerMap[CONFIG_MAX_TASKS];

/****************************************************************************
 * Function Prototypes
 ****************************************************************************/

static int pm_ioctl(FAR struct file *filep, int cmd, unsigned long arg);
static ssize_t pm_read(FAR struct file *filep, FAR char *buffer, size_t len);
static ssize_t pm_write(FAR struct file *filep, FAR const char *buffer, size_t len);

/****************************************************************************
 * Data
 ****************************************************************************/
static const struct file_operations pm_fops = {
	0,                          /* open */
	0,                          /* close */
	pm_read,                    /* read */
	pm_write,                   /* write */
	0,                          /* seek */
	pm_ioctl                    /* ioctl */
#ifndef CONFIG_DISABLE_POLL
	, 0                         /* poll */
#endif
};
/****************************************************************************
 * Private Functions
 ****************************************************************************/
static ssize_t pm_read(FAR struct file *filep, FAR char *buffer, size_t len)
{
	return 0;
}

static ssize_t pm_write(FAR struct file *filep, FAR const char *buffer, size_t len)
{
	return 0;
}


/************************************************************************************
 * Name: pm_ioctl
 *
 * Command description:
 *   PMIOC_LOCK - for locking PM transition 
 *   PMIOC_UNLOCK - for unlocking PM transiton
 *   PMIOC_TIMER_LOCK - for locking PM transition for certain time interval
 *   PMIOC_TIMER_SET - to set a wakeup timer
 *   PMIOC_TIMER_CANCEL - to delete a specific wakeup timer and make it available for other apps
 *   PMIOC_TUNEFREQ - for changing the operating frequency of the core to save power
 * 
 * Arguments:
 *   filep is ioctl fd, cmd is required command, arg is required argument for
 *   the command. 
 *   for TIMER_LOCK, arg should be a long unsigned type (timer interval in micro second).
 *   for TIMER_SET, arg should be a pointer to struct pm_timer_header type.
 *   for TIMER_CANCEL, arg should be the pid of the process using the pm driver.
 *   for TUNEFREQ, arg should be an int type.
 *
 * Description:
 *   This api can be used to perform PM operation.
 *
 ************************************************************************************/
static int pm_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
	int ret = -ENOSYS;
    
	/* Handle built-in ioctl commands */
	switch (cmd) {
    	case PMIOC_LOCK:
        	pm_stay(PM_IDLE_DOMAIN, PM_NORMAL);
        	pmvdbg("State locked!\n");
        	ret = OK;
        	break;
        case PMIOC_UNLOCK:
            	pm_relax(PM_IDLE_DOMAIN, PM_NORMAL);
            	pmvdbg("State unlocked!\n");
            	ret = OK;
            	break;
	case PMIOC_TIMER_LOCK:
		if (arg > 0) {
			pm_stay(PM_IDLE_DOMAIN, PM_NORMAL);
			pmvdbg("State locked!\n");
			pm_set_timer(PM_LOCK_TIMER, arg);
			ret = OK;
		} else {
			pmvdbg("Please input positive timer interval\n");
		}
		break;
	case PMIOC_TIMER_SET:
		
		if ((struct pm_timer_header *)arg == NULL) {
			return -EINVAL;
		}
		
		/* Check if the process is already using a pm timer, if no, create one */
		if (pid_pmTimerMap[((struct pm_timer_header *)arg)->pid] == PM_NO_TIMER_USING) {
			pid_pmTimerMap[((struct pm_timer_header *)arg)->pid] = pm_timer_create(((struct pm_timer_header *)arg)->is_periodic);
		}
		
		/* Set the timer as required */
		ret = pm_timer_set(pid_pmTimerMap[((struct pm_timer_header *)arg)->pid], ((struct pm_timer_header *)arg)->timer_interval);

		/* If the timer is not periodic, we dont need to reuse it */
		if (!((struct pm_timer_header *)arg)->is_periodic) {
			pid_pmTimerMap[((struct pm_timer_header *)arg)->pid] = PM_NO_TIMER_USING;
		}
		break;
	case PMIOC_TIMER_CANCEL:
		if (pid_pmTimerMap[(uint16_t)arg] == PM_NO_TIMER_USING) {
			pmvdbg("Trying to cancel a pm timer that is not set\n");
		} else {
			ret = pm_timer_cancel(pid_pmTimerMap[(uint16_t)arg]);
			pid_pmTimerMap[(uint16_t)arg] = PM_NO_TIMER_USING;
		}
		break;
#ifdef CONFIG_PM_DVFS
        case PMIOC_TUNEFREQ:
        	pm_dvfs(arg);
        	ret = OK;
        	break;
#endif
	default:
		pmvdbg("Invalid command passed!\n");
		break;
        }
        return ret;
}

/****************************************************************************
 * Name: pm_register
 *
 * Description:
 *   Register pm driver path, PM_DRVPATH
 *
 ****************************************************************************/
void pm_driver_register(void)
{
	(void)register_driver(PM_DRVPATH, &pm_fops, 0666, NULL);
	for (int i = 0; i < CONFIG_MAX_TASKS; i++) {
		pid_pmTimerMap[i] = PM_NO_TIMER_USING;
	}
}
