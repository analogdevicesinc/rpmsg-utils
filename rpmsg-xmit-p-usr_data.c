/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright 2023, Analog Devices, Inc. All rights reserved. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include <pthread.h>

#define MAX_THREADS 32
#define RPMSG_CHARDEV_DRIVER "rpmsg_chrdev"
#define RPMSG_CHARDEV_CTRL "rpmsg_ctrl"
#define MAX_PACKET_SIZE ((1<<9) - 16) /* 2^11 - 16 */
#define MAX_INPUT_FILE_SIZE (6*(1<<10))

#define debug(fmt, ...) \
		if (verbose) \
			printf(fmt, ##__VA_ARGS__)


typedef struct thread_params {
	int packet_size;
	int total_data;
	char *dev_prefix;
	char *data_file;
	int dev_num;
	float mbsec;
} thread_params;

static unsigned char data_buf[MAX_PACKET_SIZE];
static int verbose=0;

int timespec_diff(struct timespec *ts1, struct  timespec *ts2, struct timespec *tdiff) {
	int sec=0,nsec=0;
	sec = ts2->tv_sec - ts1->tv_sec;
	nsec = ts2->tv_nsec - ts1->tv_nsec;
	if (nsec<0) {
		nsec += 1000000000;
		sec -= 1;
	}
	tdiff->tv_sec = sec;
	tdiff->tv_nsec = nsec;

	return 0;
}

void sig_handler(int signo)
{
    switch(signo)
    {
        case SIGINT:
        default:
            printf("Unknown signal %d\n",signo);
    }
    exit(0);
}

/* returns the current float and offset to the next row 
 * Takes and returns the CHAR offset for data
 *
 * Assuming we always have a perfect start
 * */
int get_next_float(char *data, size_t offset, float *curr_float) {
	int i=offset;
	int curr_boundary_offset = 0;
	char tmp[128];

	/* finding current row end */
	for (i=offset; data[i] != ','; i++);

	curr_boundary_offset = i - 1;
	memcpy(tmp, &data[offset], curr_boundary_offset-offset);

	/* converting %s to float */
	*curr_float = atof(tmp);

	/* finding next row */
	for (i=curr_boundary_offset; data[i] != '\n'; i++);

	/* return the starting byte of the next row */
	return i+1; 
}

void *run_xmit_thread(void *data) {
	int total_sent=0;
	int total_recv=0;
	int fd_dev, fd_data;
	int errnum;
	thread_params *tp  = (thread_params *)data;
	struct timespec ts_s, ts_e, ts_diff;
	double mbsec=0.0,d_sec;

	const int max_path_len=128;
	char device_path[max_path_len];
	char *data_file = tp->data_file;
	int packet_size = tp->packet_size;
	char *file_data_buf;
	uint32_t next_row_offset = 0;

	snprintf(device_path, max_path_len, "/dev/rpmsg%d",tp->dev_num);
	debug("Opening device %s\n", device_path);

	fd_dev = open(device_path, O_RDWR);
	if (fd_dev == -1) {
		errnum = errno;

		printf("!Cant open file %s, return code %d\n", 
				device_path, fd_dev);
		printf("Error %d: %s\n",  errnum, strerror(errnum));
		return NULL;
	};

	if (data_file != NULL) {
		debug("Using data from file: %s\n", tp->data_file);
		fd_data = open(tp->data_file, O_RDONLY);
		
		if (fd_data == -1) {
			errnum = errno;
			printf("Error %d: %s\n",  errnum, strerror(errnum));
			return NULL;
		}

		size_t char_data_size = lseek(fd_data, 0, SEEK_END);

		file_data_buf = (char *) malloc(char_data_size);
		lseek(fd_data, 0, SEEK_SET);
		memset(file_data_buf, 0, char_data_size);
		debug("ready to read\n");
		read(fd_data, file_data_buf, char_data_size);
		debug("data in memory\n");

		//just set an arbitrary size for now
		tp->total_data = char_data_size;
		debug("Total data %d\n", tp->total_data);

	} else {
		debug("Using random data\n");
	}

	debug("Loop is running ... thread-%d\n", tp->dev_num);
	clock_gettime(CLOCK_BOOTTIME, &ts_s);
	debug(" total_data: %d\n", tp->total_data);

	while (total_sent < tp->total_data) {
		int i, ret = 0;

		debug("Sending %d bytes\n", tp->packet_size);
		//read data from file
		if (data_file != NULL) {
			/* Every iteration, move to the next row and 
			 * read the next float, update it in the data data_buf
			 * get the next char offset. 
			 *
			 * Repeat until packet is filled
			 * */

			for(i=0; i<tp->packet_size; i+=sizeof(float)) {

				next_row_offset = 
					get_next_float(file_data_buf, 
							next_row_offset, 
							(float *)&data_buf[i]);

				if (next_row_offset > tp->total_data) {
					debug("End of file\n");
					break;
				}

			}

			if (verbose) {
				printf("Fit %ld floats in packet\n", i/sizeof(float));
				printf("First %ld floats: \n", i/(sizeof(float)*10));

				for(int j=0; j<(i/(sizeof(float)*10)); j++) {
					debug("%9.6f\n", *((float *)data_buf + j));
				}
			}
		}

		ret = write(fd_dev, data_buf, tp->packet_size);
		if (ret == -1) {
			printf("Thread-%d: Error writing data to %s\n", 
					tp->dev_num, device_path);
			break;
		}

		total_sent += ret;
		ret = read(fd_dev, data_buf, tp->packet_size);
		if (ret == -1) {
			printf("Read error %s\n", device_path);
		}
		
		total_recv += ret;
		data_buf[ret] = '\0';

		if (verbose) {
			printf("reply: %d B\n", ret);
			printf("%s\n", data_buf);
		}

		debug("sent %d bytes, recv %d bytes\n", total_sent, total_recv);
		
		/* set the next amount of bytes needed */
		if (data_file != NULL) {
			packet_size = atoi((const char *) data_buf);
			debug("Got requested packet_size: %d\n", packet_size);
			
			if (packet_size > 0) {
				if (packet_size > MAX_PACKET_SIZE) 
					tp->packet_size = MAX_PACKET_SIZE;
				else
					tp->packet_size = 
						(packet_size/sizeof(float)) * sizeof(float); //must be multiple of float
			} else {
				tp->packet_size = 0;
				break;
			}

			debug("Next transfer size %d\n", tp->packet_size);
		}
	}

	// write this data to a file
	FILE *fptr = fopen("/root/output.dat", "w");
	if (fptr == NULL) {
		printf("Cannot open file \n");
		exit(0);
	}

	debug("\n****Input data sent, getting task statuses****\n");
	while (1) {

		int ret = 0;
wait_for_data:
		data_buf[0] = '0';
		ret = write(fd_dev, data_buf, 1);
		if (ret == -1) {
			printf("Thread-%d: Error writing data to %s\n", 
					tp->dev_num, device_path);
			break;
		}

		ret = read(fd_dev, data_buf, MAX_PACKET_SIZE);
		if (ret == -1) 
			printf("Read error %s\n", device_path);

		if (data_buf[0] == 'F') {
			sleep(1);

			if (verbose) {
				printf("Processing\n");
				printf("reply: %c\n", data_buf[0]);
			}

			goto wait_for_data;
		}

		total_recv += ret;
		if (verbose) {
			printf("reply: %d B\n", ret);

			for(int j=0; j<5; j++)
				printf("%9.6f \n", *((float *)data_buf+j));
		}

		for(int j=0; j<ret/sizeof(float); j++) 
			fprintf(fptr, "%9.6f,\n", *((float *)data_buf+j));

		//no more left, this was the last packet
		if (ret < MAX_PACKET_SIZE) 
			break;

	}

	fclose(fptr);
	clock_gettime(CLOCK_BOOTTIME, &ts_e);
	close(fd_dev);
	if (data_file != NULL) {
		free(file_data_buf);
		close(fd_data);
		debug("File closed\n");
	}

	timespec_diff( &ts_s, &ts_e, &ts_diff);
	d_sec = ts_diff.tv_sec + ts_diff.tv_nsec/1000000000.0f;
	mbsec = 8 * total_sent/(d_sec * (1 << 20));
	printf("Thread-%d stoped\n\n total sent %d B\n total recv %d B\n total tx time %f s\n tx speed %f Mib/s\n",tp->dev_num, total_sent, total_recv, d_sec, mbsec);
	tp->mbsec = mbsec;

	return NULL;
} 

void usage() {
	printf("Usage: rpmsg-xmit-p-usr_data\n"\
	"-p device prefix\n"\
	"-f input file\n"\
	"-n number of endpoints\n"\
	"-e start endpoint\n"\
	"-a start address\n" \
	"-s packet size\n"\
	"-t total data send\n"
	"-v verbose\n"
	"-h help\n"
	"ver 0.3 \n"\
	"\n");
	exit(1);
}

int sanitize_int(char *optarg) {
	char *end;
	errno = 0;
	int ret  = strtol(optarg, &end, 0);
	if ((end != (optarg + strlen(optarg))) || (errno != 0)) {
		fprintf(stderr, "Wrong `addr` format\n");
		usage();
	}
	if (ret < 0){
		fprintf(stderr, "Wrong `addr` format: must be positive\n");
	}
	return ret;
}

int main(int argc, char **argv) {

	int c;
	int i;

	char *arg_prefix=NULL;
	char *arg_data_file = NULL;
	int arg_start_ep=-1;
	int arg_ep_num=1;
	int arg_packet_size=1;
	int arg_total_data=1;
	float total_bw=0.0f;

	pthread_t rpmsg_thread[MAX_THREADS];
	int thread_id[MAX_THREADS];
	thread_params params[MAX_THREADS];


	if (signal(SIGINT,sig_handler) == SIG_ERR)//SHOULD DO FOR ALL threads
	{
		printf("cannot register signal handler\n");
		exit(1);
	}

	memset(data_buf, 0x00, MAX_PACKET_SIZE);
	while((c = getopt(argc, argv, "p:f:n:e:a:s:t:vh")) != -1) {
		switch (c) {
			case 'p':
				arg_prefix = optarg;
				break;
			case 'f':
				arg_data_file = malloc(strlen(optarg));
				memcpy(arg_data_file, optarg, strlen(optarg));
				break;
			case 'n':
				arg_ep_num = sanitize_int(optarg);
				break;
			case 'e':
				arg_start_ep = sanitize_int(optarg);
				break;
			case 't':
				arg_total_data = sanitize_int(optarg);
				break;
			case 's':
				arg_packet_size = sanitize_int(optarg);
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				usage();
				break;
			default:
				printf("Unknown command line option\n");
				usage();
		}
	}

	if (arg_ep_num > MAX_THREADS) {
		printf("Maximum supported thread number 32\n");
		exit(1);
	}

	if (arg_packet_size > MAX_PACKET_SIZE) {
		printf("Max packet size %d\n", MAX_PACKET_SIZE);
		exit(1);
	}

	debug("Start\n");

	//create all threads
	for (i=0; i<arg_ep_num;i++) {
		
		params[i].total_data = arg_total_data;
		params[i].data_file = arg_data_file;
		params[i].packet_size = arg_packet_size;
		params[i].dev_prefix = arg_prefix;
		params[i].dev_num = arg_start_ep + i;

		debug("set dev num to %d\n", params[i].dev_num);

		int rc = thread_id[i] = pthread_create( &rpmsg_thread[i], NULL, &run_xmit_thread, (void *)&params[i]);
		if (rc != 0) {
			printf("Failed to start thread %d\n", i);
		}
	}



	//join all threads
	for (i=0; i<arg_ep_num; i++) {
		pthread_join(rpmsg_thread[i], NULL);
	}

	for (i=0; i<arg_ep_num; i++) {
		total_bw += params[i].mbsec;
	}
	
	if (arg_data_file)
		free(arg_data_file);

	printf("\nTotal %d channel bandwidth %f Mib/s\n",arg_ep_num, total_bw);

	return 0;
}


