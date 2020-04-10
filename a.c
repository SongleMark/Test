/*================================================================
 *   Copyright (C) 2020 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：a.c
 *   创 建 者：当年小马哥
 *   创建日期：2020年04月10日
 *   描    述：
 *
 ================================================================*/

/**
 *
 * supervisor 
 *
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define LOG_FILE "/var/log/supervisor.log"

void s_log(char *text) {
	time_t      t;
	struct tm  *tm;
	char log_file[32];
	FILE *fp_log;
	char date[128];

	//log_file = LOG_FILE;
	strcpy(log_file, LOG_FILE);
	fp_log = fopen(log_file, "a+");
	if (NULL == fp_log) {
		fprintf(stderr, "Could not open logfile '%s' for writing\n", log_file);
	}

	time(&t);
	tm = localtime(&t);
	strftime(date, 127, "%Y-%m-%d %H:%M:%S", tm);

	/* write the message to stdout and/or logfile */	
	fprintf(fp_log, "[%s] %s\n", date, text);
	fflush(fp_log);
	fclose(fp_log);
} 

int main(int argc, char **argv) {
	int ret, i, status;
	char *child_argv[100] = {0};
	pid_t pid;
	if (argc < 2) {
		fprintf(stderr, "Usage:%s <exe_path> <args...>", argv[0]);
		return -1;
	}

	for (i = 1; i < argc; ++i) {
		child_argv[i-1] = (char *)malloc(strlen(argv[i])+1);
		strncpy(child_argv[i-1], argv[i], strlen(argv[i]));
		//child_argv[i-1][strlen(argv[i])] = '0';
	}

	while(1) {
		pid = fork(); 
		if (pid == -1) {
			fprintf(stderr, "fork() error.errno:%d error:%s", errno, strerror(errno));
			break;
		}
		if (pid == 0) {
			s_log(child_argv[0]);
			ret = execv(child_argv[0], (char **)child_argv);
			s_log("execv return");
			if (ret < 0) {
				fprintf(stderr, "execv ret:%d errno:%d error:%s", ret, errno, strerror(errno));
				continue;
			}
			s_log("exit child process");
			exit(0);
		}
		if (pid > 0) {
			pid = wait(&status);
			fprintf(stdout, "Child process id: %d\n", pid);
			//fprintf(stdout, "wait return");
			s_log("Wait child process return");
		}
	}

	return 0;
}
