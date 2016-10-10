#include "monitor_system.h"
#include "sys_config.h"
#ifndef WIN32
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#endif
int 
CLinuxSysinfo::write(int fd, char *buf)
{
	FastWriter json_write;
	Value  json_value;
	string jsonstr;
	get_loadavg(json_value);
	get_systemtime(json_value);
	get_kernel_version(json_value);
	get_os_name(json_value);
	get_diskinfo(json_value);
	get_meminfo(json_value);
	get_monitor_data_sec(json_value);
	jsonstr = json_write.write(json_value);
	memcpy(buf, jsonstr.c_str(), jsonstr.length() + 1);
	printf("%s\n", buf);
	return jsonstr.length() + 1;
	return 0;
}


void CLinuxSysinfo::get_loadavg(Value& json_value){//cpu平均负载
#ifndef WIN32
	int f = 0;
	char buffer[80] = "";                         /* 定义字符串并初始化为'\0' */
	char buf[5][10];
	char *file = "/proc/loadavg";
	f = open(file, O_RDONLY);
	if (f == 0){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}

	read(f, (void *)buffer, 80);
	sscanf(buffer, "%s %s %s %s %s",            /* sscanf()拆分成多个字符串 */
		&buf[0], &buf[1], &buf[2], &buf[3], &buf[4]);
	json_value[CPU_QUEUE_LENGTH] = buf[0];
	close(f);
#endif
}
void CLinuxSysinfo::get_systemtime(Value& json_value)
{
#ifndef WIN32
	int f = 0;
	char buffer[80] = "";
	float alltime,idletime;
	char stridle[30];
	char *file = "/proc/uptime";
	f = open(file, O_RDONLY);
	if (f == 0){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	read(f, (void *)buffer, 80);
	sscanf(buffer, "%f %f", &alltime, &idletime);
	close(f);
	sprintf_s(stridle,sizeof(stridle),"%.2f",idletime/alltime);
	json_value[SYSTEM_IDLE] = stridle;
	close(f);
#endif
}

void CLinuxSysinfo::get_kernel_version(Value& json_value){
#ifndef WIN32
	int f = 0;
	char buffer[80] = "";
	char *file = "/proc/sys/kernel/version";
	f = open(file, O_RDONLY);
	if (f == 0){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	read(f, (void *)buffer, 80);
	buffer[strlen(buffer) - 1] = 0;                 /* 简单实现tr()函数的功能 */
	json_value[OS_VERSION] = buffer;
	close(f);
#endif
}

void CLinuxSysinfo::get_os_name(Value& json_value){
#ifndef WIN32
	int f = 0;
	char buffer[80] = "";
	char *file = "/proc/sys/kernel/ostype";
	f = open(file, O_RDONLY);
	if (f == 0)
	{
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	read(f, (void *)buffer, 80);
	buffer[strlen(buffer) - 1] = 0;
	json_value[OS_NAME] = buffer;
	close(f);
#endif
}
void CLinuxSysinfo::get_diskinfo(Value& json_value){
#ifndef WIN32
	FILE *fp;
	size_t len = 0;
	int nread = 0;
	char *buffer = NULL;
	char buf[4][32];
	char *file = "/proc/partitions";
	fp = fopen(file, "rb");
	if (fp == NULL){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	while ((nread = getline(&buffer, &len, fp)) != -1) {
		sscanf(buffer, "%s %s %s %s",&buf[0],&buf[1],&buf[2],&buf[3]);
		if (strstr(buf[3],"da")!=NULL){
			string keystr = DISK_TOTAL;
			keystr = keystr+"_"+buf[3];
			json_value[keystr] = buf[2];
		}
	}
	if (buffer)
		free(buffer);
	fclose(fp);
#endif
}
void CLinuxSysinfo::get_disk_io(int& io_num)
{
#ifndef WIN32
	FILE *fp;
	int nread = 0;
	int read_num = 0,write_num=0;
	size_t len = 0;
	char *buffer = NULL;
	char buf[12][32];
	char *file = "/proc/diskstats";
	char *p;
	fp = fopen(file, "rb");
	if (fp == NULL){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	while ((nread = getline(&buffer, &len, fp)) != -1) { 
		sscanf(buffer, "%04s%08s%s %d %s %s %s %d %s %s %s %s %s %s",
			&buf[0], &buf[1], &buf[2], &read_num, &buf[3], &buf[4], &buf[5],
			&write_num, &buf[6], &buf[7], &buf[8], &buf[9], &buf[10], &buf[11]);
		if ((p = strstr(buf[2], "da")) != NULL)
			io_num += read_num + write_num;
	}
	if (buffer)
		free(buffer);
	fclose(fp);
#endif
}

void CLinuxSysinfo::get_cpu_time(int &all_time, int& idle_time)//获取CPU利用率进程数
{
#ifndef WIN32
	FILE *fp;
	size_t len = 0;
	char *buffer = NULL;
	char *file = "/proc/stat";
	char cpu[5]; 
	int user,nice,sys,iowait,irq,softirq;
	fp = fopen(file, "rb");
	if (fp == NULL){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	getline(&buffer,&len,fp);
	sscanf(buffer,"%s%d%d%d%d%d%d%d",cpu,
		&user,&nice,&sys,&idle_time,&iowait,&irq,&softirq);
	all_time = user + nice + sys + idle_time + iowait + irq + softirq;
	if (buffer)
		free(buffer);
	fclose(fp);
#endif
}

void CLinuxSysinfo::get_meminfo(Value& json_value)
{
#ifndef WIN32
	FILE *fp;
	int nread = 0;
	size_t len = 0;
	char *buf = NULL;
	char *buffer = NULL;
	char *file = "/proc/meminfo";
	char content[16] = "";
	fp = fopen(file, "rb");
	Int64 vmalloctotal_kb;
	if (fp == NULL){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	while ((nread = getline(&buffer, &len, fp)) != -1) {
		if ((buf = strstr(buffer, "MemTotal")) != NULL){
			buffer[strlen(buffer) - 1] = 0;
			sscanf(buffer, "%s%s", content, content);
			int memtotal_kb = (int)(strtof(content, NULL));
			json_value[MEMORY_TOTAL] = memtotal_kb;
		}
		if ((buf = strstr(buffer, "MemFree")) != NULL){
			buffer[strlen(buffer) - 1] = 0;
			sscanf(buffer, "%s%s", content, content);
			int memfree_kb = (int)(strtof(content, NULL));
			json_value[MEMORY_FREE] = memfree_kb ;
		}
/*
		if ((buf = strstr(buffer, "SwapTotal")) != NULL){
			buffer[strlen(buffer) - 1] = 0;
			sscanf(buffer, "%s%s", content, content);
			int swaptotal_kb = (int)(strtof(content, NULL));
			json_value["SwapTotalKB"] = swaptotal_kb;
		}
*/
/*
		if ((buf = strstr(buffer, "SwapFree")) != NULL){
			buffer[strlen(buffer) - 1] = 0;
			sscanf(buffer, "%s%s", content, content);
			int swapfree_kb = (int)(strtof(content, NULL));
			json_value["SwapFreeKB"] = swapfree_kb;
		}
*/
		if ((buf = strstr(buffer, "VmallocTotal")) != NULL){
			buffer[strlen(buffer) - 1] = 0;
			sscanf(buffer, "%s%s", content, content);
			vmalloctotal_kb = (Int64)(strtof(content, NULL));
			json_value[VIRTUAL_MEM_TATAL] = vmalloctotal_kb ;
		}
		if ((buf = strstr(buffer, "VmallocUsed")) != NULL){
			buffer[strlen(buffer) - 1] = 0;
			sscanf(buffer, "%s%s", content, content);
			Int64 vmallocused_kb = (Int64)(strtof(content, NULL));
			json_value[VIRTUAL_MEM_FREE] = vmalloctotal_kb- vmallocused_kb;
		}
	}
	if (buffer)
		free(buffer);
	fclose(fp);
#endif
}

void
CLinuxSysinfo::get_monitor_data_sec(Value& json_value)
{
#ifndef WIN32
	int disk_io1 = 0;
	int disk_io2 = 0;
	int cpu_all_time1 = 0;
	int cpu_all_time2 = 0;
	int cpu_idle1 = 0;
	int cpu_idle2 = 0;
	char cpu_usage[10];
	
	long network_transfers1 = 0;
	long network_transfers2 = 0;

	get_disk_io(disk_io1);
	get_cpu_time(cpu_all_time1,cpu_idle1);
	get_network_transfers(network_transfers1);
	sleep(1);

	get_disk_io(disk_io2);
	get_cpu_time(cpu_all_time2,cpu_idle2);
	get_network_transfers(network_transfers2);

	json_value[DISK_IO] = disk_io2 - disk_io1;
	sprintf_s(cpu_usage, sizeof(cpu_usage), "%.2f",\
		(float)(cpu_all_time2 - cpu_all_time1 - (cpu_idle2- cpu_idle1)) \
		/ (cpu_all_time2- cpu_all_time1) * 100);
	json_value[CPU_USAGE] = cpu_usage;
	json_value[NETWORK_BYTES_TOTAL_SEC] = network_transfers2 - network_transfers1;
#endif
}

void CLinuxSysinfo::get_network_transfers(long& bytes)
{
#ifndef WIN32
	FILE *fp;
	size_t len = 0;
	int nread = 0;
	char *buffer = NULL;
	char buf[8][32];
	long receive_bytes = 0,transmit_bytes = 0;
	char *file = "/proc/net/dev";
	fp = fopen(file, "rb");
	if (fp == NULL){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	while ((nread = getline(&buffer, &len, fp)) != -1) {
		if (strstr(buffer,"eth")){
			sscanf(buffer, "%s %d %s %s %s %s %s %s %d %s", &buf[0], &receive_bytes, &buf[1], &buf[2],
				&buf[3], &buf[4], &buf[5], &buf[6],&transmit_bytes, &buf[7]);
			bytes+=receive_bytes+transmit_bytes
		}
		
	}
	if (buffer)
		free(buffer);
	fclose(fp);

#endif
}

void CLinuxSysinfo::get_tcp_connections(Value& json_value)
{
#ifndef WIN32
	FILE *fp;
	size_t len = 0;
	int nread = 0;
	char *buffer = NULL;
	char buf[2][32];
	char *file = "/proc/net/sockstat";
	int  tcp_connectios;
	fp = fopen(file, "rb");
	if (fp == NULL){
		printf("error to open: %s\n", file);
		exit(EXIT_FAILURE);
	}
	while ((nread = getline(&buffer, &len, fp)) != -1) {
		if (strstr(buffer, "TCP")){
			sscanf(buffer, "%s %s %d ", &buf[0], &buf[1], &tcp_connectios);
			break;
		}
	}
	json_value[PROCESS_TCP_CONNECTION] = tcp_connectios;
	if (buffer)
		free(buffer);
	fclose(fp);
#endif
}