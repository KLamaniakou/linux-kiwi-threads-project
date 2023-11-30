#include "bench.h"

void _random_key(char *key,int length) {
	int i;
	char salt[36]= "abcdefghijklmnopqrstuvwxyz0123456789";

	for (i = 0; i < length; i++)
		key[i] = salt[rand() % 36];
}

void _print_header(int count)
{
	double index_size = (double)((double)(KSIZE + 8 + 1) * count) / 1048576.0;
	double data_size = (double)((double)(VSIZE + 4) * count) / 1048576.0;

	printf("Keys:\t\t%d bytes each\n", 
			KSIZE);
	printf("Values: \t%d bytes each\n", 
			VSIZE);
	printf("Entries:\t%d\n", 
			count);
	printf("IndexSize:\t%.1f MB (estimated)\n",
			index_size);
	printf("DataSize:\t%.1f MB (estimated)\n",
			data_size);

	printf(LINE1);
}

void _print_environment()
{
	time_t now = time(NULL);

	printf("Date:\t\t%s", 
			(char*)ctime(&now));

	int num_cpus = 0;
	char cpu_type[256] = {0};
	char cache_size[256] = {0};

	FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
	if (cpuinfo) {
		char line[1024] = {0};
		while (fgets(line, sizeof(line), cpuinfo) != NULL) {
			const char* sep = strchr(line, ':');
			if (sep == NULL || strlen(sep) < 10)
				continue;

			char key[1024] = {0};
			char val[1024] = {0};
			strncpy(key, line, sep-1-line);
			strncpy(val, sep+1, strlen(sep)-1);
			if (strcmp("model name", key) == 0) {
				num_cpus++;
				strcpy(cpu_type, val);
			}
			else if (strcmp("cache size", key) == 0)
				strncpy(cache_size, val + 1, strlen(val) - 1);	
		}

		fclose(cpuinfo);
		printf("CPU:\t\t%d * %s", 
				num_cpus, 
				cpu_type);

		printf("CPUCache:\t%s\n", 
				cache_size);
	}
}

int main(int argc,char** argv)
{
	long int count;

	srand(time(NULL));
	if (argc < 3) {
		fprintf(stderr,"Usage: db-bench <write | read | readwrite | readwrite_v2> <count> <threads | pososto> <pososto>\n");
		exit(1);
	}
	
	if (strcmp(argv[1], "write") == 0) {
		int r = 0;
		int nimata; //we use it to take the number of threads from main arg
		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 5) //we change this from 4 to 5 
			r = 1;
		if (argc == 4){//the argument 4 exist we have threads from console
			nimata=atoi(argv[3]); //take the number of threads and make it int with atoi
		}else{
			nimata=1; //else we dont have threads from console and take the default to 1
		}
		_write_test(count, r,nimata); //put the number inside of write test to use it on db_add
	} else if (strcmp(argv[1], "read") == 0) {
		int r = 0;
		int nimata;//we use it to take the number of threads from main arg
		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 5) //we change this from 4 to 5 
			r = 1;
		if (argc == 4){ //the argument 4 exist we have threads from console
			nimata=atoi(argv[3]); //take the number of threads and make it int with atoi
		}else{
			nimata=1; //else we dont have threads from console and take the default to 1
		}
		_read_test(count, r,nimata); //put the number inside of write test to use it on db_get
	}else if(strcmp(argv[1], "readwrite") == 0) {
		int r = 0;
		double pososto; //we use it to take the persentage to calculate how many threads to use.
		int nimata; //we use it to take the number of threads from main arg
		nimata=atoi(argv[3]); //take arg 3 as integer 
		pososto=atof(argv[4]); //take arg 4 as float 
		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 6) //we change this from 5 to 6.
			r = 1;
		_readwrite(count,r,nimata,pososto); //new readwrite
	}else if(strcmp(argv[1] , "readwrite_v2") == 0){
		int r = 0;
		int nimata;//we use it to take the number of threads from main arg
		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 5) //we change this from 4 to 5 
			r = 1;
		if (argc == 4){ //the argument 4 exist we have threads from console
			nimata=atoi(argv[3]); //take the number of threads and make it int with atoi
		}else{
			nimata=1; //else we dont have threads from console and take the default to 1
		}
		_readwrite_v2(count, r,nimata); //put the number inside of write test to use it on db_get
	}else{
		fprintf(stderr,"Usage: db-bench <write | read> <count> <random>\n");
		exit(1);
	}

	return 1;
}
