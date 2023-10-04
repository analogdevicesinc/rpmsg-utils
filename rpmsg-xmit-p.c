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
#define MAX_PACKET_SIZE 496


typedef struct thread_params {
	int packet_size;
	int total_data;
	char *dev_prefix;
	int dev_num;
	float mbsec;
} thread_params;

static unsigned char data_buf[MAX_PACKET_SIZE];
static unsigned char read_data_buf[MAX_PACKET_SIZE];
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

void *run_xmit_thread(void *data) {
	int total_sent=0;
	int total_recv=0;
	int fd;
	int ret;
	int errnum;
	thread_params *tp  = (thread_params *)data;
	struct timespec ts_s, ts_e, ts_diff;
	double mbsec=0.0,d_sec;

	const int max_path_len=128;
	char device_path[max_path_len];
	//example of names
	//
	snprintf(device_path, max_path_len, "/dev/rpmsg%d",tp->dev_num);


	printf("Opening file %s\n", device_path);

	
	fd = open(device_path, O_RDWR);
	if (fd == -1) {
		errnum = errno;

		printf("!Cant open file %s, return code %d\n", device_path, fd);
		printf("Error %d: %s\n",  errnum, strerror(errnum));
		return NULL;
	};

	printf("Drain ... \n");


	printf("Loop is running ... thread-%d\n", tp->dev_num);
	clock_gettime(CLOCK_BOOTTIME, &ts_s);
	while (total_sent < tp->total_data) {
		int ret = write(fd, data_buf, tp->packet_size);
		if (ret == -1) {
			printf("Thread-%d: Error writing data to %s\n", tp->dev_num, device_path);
			break;
		}
		total_sent += ret;
		
		ret = read(fd, data_buf, tp->packet_size);
		if (ret == -1) {
			printf("Read error %s\n", device_path);
		}
		total_recv += ret;
		if (verbose) {
			printf(".");
		}
	}
	clock_gettime(CLOCK_BOOTTIME, &ts_e);

	close(fd);

	
	timespec_diff( &ts_s, &ts_e, &ts_diff);
	d_sec = ts_diff.tv_sec + ts_diff.tv_nsec/1000000000.0f;
	//printf("dsec %f, %d %d\n", d_sec, ts_diff.tv_sec, ts_diff.tv_nsec);
	mbsec = total_sent/d_sec;
	printf("Thread-%d stoped, total sent %d bytes, total recv %d, total sec %d.%d, tx bytes per sec %f\n",tp->dev_num, total_sent, total_recv, ts_diff.tv_sec, ts_diff.tv_nsec, mbsec);
	tp->mbsec = mbsec;
} 

void usage() {
	printf("Usage: rpmsg-xmit-p\n"\
	"-p device prefix\n"\
	"-n number of endpoints\n"\
	"-e start endpoint\n"\
	"-a start address\n" \
	"-s packet size\n"\
	"-t total data send\n"
	"-v verbose\n"
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
	int arg_start_ep=-1;
	//int arg_start_addr=-1;
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




	while((c = getopt(argc, argv, "p:n:e:a:s:t:v")) != -1) {
		switch (c) {
			case 'p':
				arg_prefix = optarg;
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
			//case 'a':
			//	arg_start_addr = sanitize_int(optarg);
			//	break;
			case 'v':
				verbose = 1;
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

	printf("Start\n");

	

	//create all threads
	for (i=0; i<arg_ep_num;i++) {
		
		params[i].total_data = arg_total_data;
		params[i].packet_size = arg_packet_size;
		params[i].dev_prefix = arg_prefix;
		params[i].dev_num = arg_start_ep + i;

		printf("set dev num to %d\n", params[i].dev_num);

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
	printf("Total %d channel transfer per sec %f\n",arg_ep_num, total_bw);

	return 0;
}


