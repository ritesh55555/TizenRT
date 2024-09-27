/****************************************************************************
 *
 * Copyright 2023 Samsung Electronics All Rights Reserved.
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

#ifndef __CRASH_DUMP_INTERNAL_H
#define __CRASH_DUMP_INTERNAL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/
void crash_dump_initialize(void);
void crash_dump_start(void);
void crash_dump_stop(void);
int crash_dump_save(int ch);


#endif							/* __LOG_DUMP_INTERNAL_H */
