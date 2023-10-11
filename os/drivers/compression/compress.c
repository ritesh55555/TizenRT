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
/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <tinyara/config.h>

#include <tinyara/fs/fs.h>
#include <tinyara/fs/ioctl.h>
#include <tinyara/compression.h>

#include <errno.h>
#include <debug.h>
#include <assert.h>

/****************************************************************************
 * Function Prototypes
 ****************************************************************************/

static int comp_ioctl(FAR struct file *filep, int cmd, unsigned long arg);
static ssize_t comp_read(FAR struct file *filep, FAR char *buffer, size_t len);
static ssize_t comp_write(FAR struct file *filep, FAR const char *buffer, size_t len);

/****************************************************************************
 * Data
 ****************************************************************************/
static const struct file_operations compress_fops = {
	0,                          /* open */
	0,                          /* close */
	comp_read,                  /* read */
	comp_write,                 /* write */
	0,                          /* seek */
	comp_ioctl              /* ioctl */
#ifndef CONFIG_DISABLE_POLL
	, 0                         /* poll */
#endif
};
/****************************************************************************
 * Private Functions
 ****************************************************************************/
static ssize_t comp_read(FAR struct file *filep, FAR char *buffer, size_t len)
{
	return 0;
}

static ssize_t comp_write(FAR struct file *filep, FAR const char *buffer, size_t len)
{
	return 0;
}


/************************************************************************************
 * Name: comp_ioctl
 *
 * Command description:
 * 	 COMPIOC_COMPRESS - for compression of input data
 * 	 COMPIOC_DECOMPRESS - for decompression of input data
 * 	 COMPIOC_GET_COMP_TYPE - for getting compression type
 *
 * Arguments:
 * 	 filep is ioctl fd, cmd is required command, arg is required argument for
 * 	 the command. For compression and decompression, arg is a struct compress_header
 * 	 pointer in which user stores input_buffer, input_size data. 
 * 	 For getting compression_type, arg is NULL.
 *
 * Description:
 * 	 The ioctl method for compress apis. For compression and decompression,
 * 	 ouput result is stored in outpu_buffer and output_size of arg(struct compress_header *).
 * 	 For compression type, it just returns the compression type value.
 *
 ************************************************************************************/
static int comp_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
	int ret = -ENOSYS;
	struct compress_header *comp_info = (struct compress_header *)arg;

	DEBUGASSERT(comp_info);
	/* Handle built-in ioctl commands */
	switch (cmd) {
	case COMPIOC_COMPRESS:
		ret = compress_block(comp_info->output_buffer, &comp_info->output_size, comp_info->input_buffer, comp_info->input_size);
		break;
	case COMPIOC_GET_COMP_TYPE:
		/* CONFIG_COMPRESSION_TYPE 1 for LZMA and 2 for MINIZ */
		ret = CONFIG_COMPRESSION_TYPE;
		break;
	case COMPIOC_DECOMPRESS:
		ret = decompress_block(comp_info->output_buffer, &comp_info->output_size, comp_info->input_buffer, &comp_info->input_size);
		break;
	}
	return ret;
}

/****************************************************************************
 * Name: compress_register
 *
 * Description:
 *   Register compress driver path, COMP_DRVPATH
 *
 ****************************************************************************/
void compress_register(void)
{
	(void)register_driver(COMP_DRVPATH, &compress_fops, 0666, NULL);
}
