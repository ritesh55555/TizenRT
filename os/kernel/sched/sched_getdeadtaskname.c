/****************************************************************************
 *
 * Copyright 2025 Samsung Electronics All Rights Reserved.
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
#include <tinyara/sched.h>
#include <tinyara/irq.h>

#include <assert.h>
#include <debug.h>
#include <queue.h>
#include <string.h>

#include "sched/sched.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/
struct dead_pid_s {
    struct dead_pid_s *next;
    int pid;
    struct dead_name_s *namePtr;
};

typedef struct dead_pid_s dead_pid_t;

struct dead_name_s {
    struct dead_name_s *next;
    int pid_cnt;
    char name[CONFIG_TASK_NAME_SIZE];
};

typedef struct dead_name_s dead_name_t;

sq_queue_t dead_pidList;

sq_queue_t dead_nameList;

/****************************************************************************
 * Global Variables
 ****************************************************************************/

/****************************************************************************
 * Private Variables
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

dead_pid_t *allocate_pid_node(int pid)
{
    dead_pid_t *node = (dead_pid_t *)malloc(sizeof(dead_pid_t));
    if (!node) {
        printf("Malloc Error in allocate_pid_node\n");
        return NULL;
    }
    node->pid = pid;
    return node;
}

dead_name_t *allocate_name_node(char *name)
{
    dead_name_t *node = (dead_name_t *)malloc(sizeof(dead_name_t));
    if (!node) {
        printf("Malloc Error in allocate_name_node\n");
        return NULL;
    }
    strncpy(node->name, name, CONFIG_TASK_NAME_SIZE);
    node->pid_cnt = 0;
    return node;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void sched_deadtasklistinit()
{
    sq_init(&dead_pidList);
    sq_init(&dead_nameList);
}

void print_dead_structure() 
{
    dead_pid_t *pid_head = (dead_pid_t *)dead_pidList.head;
    dead_name_t *name_head = (dead_name_t *)dead_nameList.head;

    printf("printing dead pid list:\n");
    while(pid_head) {
        printf("%d ->", pid_head->pid);
        pid_head = pid_head->next;
    }
    printf("\n");
    printf("printing dead name list:\n");
    while(name_head) {
        printf("%s %d->", name_head->name, name_head->pid_cnt);
        name_head = name_head->next;
    }
    printf("\n\n");
}

void sched_addDeadtaskinfo(int pid, char *name)
{
    dead_pid_t *pid_head = (dead_pid_t *)dead_pidList.head;
    dead_pid_t *pid_node = NULL;

    /* Insert pid node in pid list */
    if (!pid_head) {
        pid_node = allocate_pid_node(pid);
        sq_addfirst((sq_entry_t *)pid_node, &dead_pidList);
    } else {
        dead_pid_t *pid_prev;
        while (pid_head) {
            if (pid_head->pid == pid) {
                printf("ERROR: pid is already present in the list of dead pid\n");
                return;
            }
            pid_prev = pid_head;
            pid_head = pid_head->next;
        }
        pid_node = allocate_pid_node(pid);
        sq_addafter((sq_entry_t *)pid_prev, (sq_entry_t *)pid_node, &dead_pidList);
    }

    /* Insert name node if the name not present in name list already */
    dead_name_t *name_head = (dead_name_t *)dead_nameList.head;
    dead_name_t *name_node = NULL;

    if (!name_head) {
        name_node = allocate_name_node(name);
        sq_addfirst((sq_entry_t *)name_node, &dead_nameList);
    } else {
        dead_name_t *name_prev;
        while (name_head) {
            if (strcmp(name_head->name, name) == 0) {
                // point to this name_node
                printf("Name already present in dead name list\n");
                pid_node->namePtr = name_head;
                name_head->pid_cnt++;
                return;
            }
            name_prev = name_head;
            name_head = name_head->next;
        }
        name_node = allocate_name_node(name);
        sq_addafter((sq_entry_t *)name_prev, (sq_entry_t *)name_node, &dead_nameList);
    }

    //point to newly created name node
    pid_node->namePtr = name_node;
    name_node->pid_cnt++;
    return;
}

void *sched_checkDeadPid(int pid)
{
    dead_pid_t *pid_head = (dead_pid_t *)dead_pidList.head;
    dead_name_t *name_node = NULL;

    while(pid_head) {
        if (pid_head->pid == pid) {
            name_node = pid_head->namePtr;
            break;
        }
        pid_head = pid_head->next;
    }

    if (pid_head) {
        sq_rem((sq_entry_t *)pid_head, &dead_pidList);
        name_node->pid_cnt--;
        if (!name_node->pid_cnt) {
            sq_rem((sq_entry_t *)name_node, &dead_nameList);
        }
    }
}

/****************************************************************************
 * Name: sched_getdeadtaskname
 *
 * Description:
 *   This function returns the name of a dead thread given its pid
 *
 * Inputs:
 *   pid
 *
 ****************************************************************************/

char *sched_getdeadtaskname(int pid)
{
    dead_pid_t *head = (dead_pid_t *)dead_pidList.head;
    
    while(head) {
        if (head->pid == pid) {
            return head->namePtr->name;
        }
        head = head->next;
    }
    return "ERROR";
}
