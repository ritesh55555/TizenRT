/****************************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <tc_common.h>
#include <tinyara/os_api_test_drv.h>
#include <tinyara/compression.h>
#include "tc_internal.h"

#define INPUT_OUTPUT_SIZE 10000

static struct compress_header *comp_tc_data;
static struct compress_header *decomp_tc_data;
static char *tc_comp_type_name;

static void uninitialize_tc_data(void)
{
	/* No need to free decomp_tc_data->input_buffer because 
	 * it is same as comp_tc_data->output_buffer */
	free(comp_tc_data->input_buffer);
	free(comp_tc_data->output_buffer);
	free(decomp_tc_data->output_buffer);
	free(comp_tc_data);
	free(decomp_tc_data);
	free(tc_comp_type_name);
}

static int initialize_tc_data(void)
{
	comp_tc_data = (struct compress_header *)malloc(sizeof(struct compress_header));
	if (comp_tc_data == NULL) {
		return ERROR;
	}
		
	comp_tc_data->input_buffer = (unsigned char *)malloc(INPUT_OUTPUT_SIZE);
	if (comp_tc_data->input_buffer == NULL) {
		goto exit_with_comp_input;
	}

	comp_tc_data->input_size = INPUT_OUTPUT_SIZE;
	
	for(int i = 0; i < comp_tc_data->input_size; i++) {
		comp_tc_data->input_buffer[i] = 'a' + (rand() % 26);
	}
	
	comp_tc_data->output_size = INPUT_OUTPUT_SIZE;
	comp_tc_data->output_buffer = (unsigned char *)malloc(INPUT_OUTPUT_SIZE);
	if (comp_tc_data->output_buffer == NULL) {
		goto exit_with_comp_output;
	}
	
	decomp_tc_data = (struct compress_header *)malloc(sizeof(struct compress_header));
	if (decomp_tc_data == NULL) {
		goto exit_with_decomp;
	}
		
	decomp_tc_data->input_buffer = comp_tc_data->output_buffer;
	decomp_tc_data->input_size = comp_tc_data->output_size;
	decomp_tc_data->output_size = comp_tc_data->input_size;
	decomp_tc_data->output_buffer = (unsigned char *)malloc(comp_tc_data->input_size);
	if (decomp_tc_data->output_buffer == NULL) {
		goto exit_with_decomp_output;	
	}

	tc_comp_type_name = (char *)malloc(10);
	
	return OK;
	
exit_with_decomp_output:
	free(decomp_tc_data);
exit_with_decomp:
	free(comp_tc_data->output_buffer);
exit_with_comp_output:
	free(comp_tc_data->input_buffer);
exit_with_comp_input:
	free(comp_tc_data);
	
	return ERROR;
}

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: tc_compress_decompress
 *
 * Description:
 *   Block-wise compression and decompression of a file.
 *
 * Returned Value:
 *   None
 ****************************************************************************/
static void tc_compressed_file_read(void)
{
	int fd;
	int ret_chk;
	fd = tc2_get_drvfd();

	/*If FAIL : Failed to open testcase driver*/
	TC_ASSERT_GEQ("tc2_get_drvfd", fd, OK);

	ret_chk = ioctl(fd, TESTIOC_COMPRESSION_TEST, 0);
	TC_ASSERT_EQ("Compression_read", ret_chk, OK);

	TC_SUCCESS_RESULT();
}

static void tc_comp_decomp_buffer(void)
{
	int fd;
	int ret_chk;
	fd = get_compfd();

	/*If FAIL : Failed to open testcase driver*/
	TC_ASSERT_GEQ("get_compfd", fd, OK);
	
	// initializing struct to store compressed and decompressed data
	ret_chk = initialize_tc_data();
	TC_ASSERT_EQ("compression data initialize", ret_chk, OK);

	// Test to check compression type 
	ret_chk = ioctl(fd, COMPIOC_GET_COMP_TYPE, NULL);
	TC_ASSERT_EQ_CLEANUP("compression_type_check", ret_chk, CONFIG_COMPRESSION_TYPE, uninitialize_tc_data());

	// Test to check compression type string name
	ret_chk = ioctl(fd, COMPIOC_GET_COMP_NAME, tc_comp_type_name);
	TC_ASSERT_EQ_CLEANUP("compression type string check", ret_chk, OK, uninitialize_tc_data());
	if ( CONFIG_COMPRESSION_TYPE == MINIZ_TYPE) {
		TC_ASSERT_EQ_CLEANUP("compression type string check", strcmp(tc_comp_type_name, MINIZ_NAME), OK, uninitialize_tc_data());
	} else {
		TC_ASSERT_EQ_CLEANUP("compression type string check", strcmp(tc_comp_type_name, LZMA_NAME), OK, uninitialize_tc_data());
	}	
	
	// Test compression method 
	ret_chk = ioctl(fd, COMPIOC_COMPRESS, comp_tc_data);
	TC_ASSERT_EQ_CLEANUP("compress block", ret_chk, OK, uninitialize_tc_data());
	
	// Test decompression method
	ret_chk = ioctl(fd, COMPIOC_DECOMPRESS, decomp_tc_data);
	TC_ASSERT_EQ_CLEANUP("decompress block", ret_chk, OK, uninitialize_tc_data());
	
	//check the data before compression and after decompression
	for (int i = 0; i < comp_tc_data->input_size; i++){
		TC_ASSERT_EQ_CLEANUP("output after decompress", comp_tc_data->input_buffer[i], decomp_tc_data->output_buffer[i], uninitialize_tc_data());
	}

	uninitialize_tc_data();
	TC_SUCCESS_RESULT();

}

/****************************************************************************
 * Public Function
 ****************************************************************************/
int tc_compress_main(void)
{
	tc_comp_decomp_buffer();
	tc_compressed_file_read();

	return 0;
}
