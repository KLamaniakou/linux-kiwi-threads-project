#include <string.h>
#include "../engine/db.h"
#include "../engine/variant.h"
#include "bench.h"
#include "kiwi.h"
#define DATAS ("testdb")

void _write_test(long int count, int r,int nimata)
{
	pthread_t tid[nimata];   //create 
	struct data thread_args; //struct
	int i;
	double cost;
	long long start,end;
	void *status ; //the return argument of join instead of ret

	char key[KSIZE + 1];
	char val[VSIZE + 1];
	char sbuf[1024];
	memset(key, 0, KSIZE + 1);
	memset(val, 0, VSIZE + 1);
	memset(sbuf, 0, 1024);

	thread_args.db = db_open(DATAS);//arguments for db get forward to struct

	start = get_ustime_sec();
	for (i = 0; i < count; i++) {
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);

		thread_args.sk.length = KSIZE; //arguments for db get forward to struct
		thread_args.sk.mem = key; //arguments for db get forward to struct
		thread_args.sv.length = VSIZE; //arguments for db get forward to struct
		thread_args.sv.mem = val; //arguments for db get forward to struct
		int k;
		for (k=0; k<nimata; k++){
			pthread_create(&tid[k],NULL,db_add,(void *) &thread_args);  //create thread
		}
		for(k=0; k<nimata; k++){
			pthread_join(tid[k],&status);//waits for a thread to terminate
		}
		//db_add(db, &sk, &sv);
		if ((i % 10000) == 0) {
			fprintf(stderr,"random write finished %d ops%30s\r", 
					i, 
					"");

			fflush(stderr);
		}
	}

	db_close(thread_args.db);//we use it for the struct

	end = get_ustime_sec();
	cost = end -start;

	printf(LINE);
	printf("|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n"
		,count, (double)(cost / count)
		,(double)(count / cost)
		,cost);
}

void _read_test(long int count, int r,int nimata)
{
	pthread_t tid[nimata];   //create 5 threads
	struct data thread_args; //use struct in function
	int i;
	int found = 0;
	double cost;
	long long start,end;
	char key[KSIZE + 1];
	void *status ; //the variable for the return

	thread_args.db = db_open(DATAS);//arguments for db get forward to struct
	start = get_ustime_sec();
	for (i = 0; i < count; i++) {
		memset(key, 0, KSIZE + 1);

		/* if you want to test random write, use the following */
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d searching %s\n", i, key);
		thread_args.sk.length = KSIZE; //arguments for db get forward to struct
		thread_args.sk.mem = key; //arguments for db get forward to struct
		int k;
		for (k=0; k<nimata; k++){
			pthread_create(&tid[k],NULL,db_get,(void *) &thread_args);  //create thread
		}
		for(k=0; k<nimata; k++){
			pthread_join(tid[k],&status);//releasing thread resources
		}
		if ((intptr_t)status) {//the return of db get 
			//db_free_data(sv.mem);
			found++;
		} 

		if ((i % 10000) == 0) {
			fprintf(stderr,"random read finished %d ops%30s\r", 
					i, 
					"");

			fflush(stderr);
		}
		
	}

	db_close(thread_args.db);

	end = get_ustime_sec();
	cost = end - start;
	printf(LINE);
	printf("|Random-Read	(done:%ld, found:%d): %.6f sec/op; %.1f reads /sec(estimated); cost:%.3f(sec)\n",
		count, found,
		(double)(cost / count),
		(double)(count / cost),
		cost);
}
void _read_write_test(long int count, int r,int nimata_r,int nimata_w){
	//initialalze read's variables

	pthread_t tid[nimata_r];   //create 5 threads
	struct data thread_args; //use struct in function
	int i;
	int found = 0;
	double cost;
	long long start,end;
	char key[KSIZE + 1];
	void *status ; //the variable for the return

	thread_args.db = db_open(DATAS);//arguments for db get forward to struct
	start = get_ustime_sec();

	//initalize write's variables

	pthread_t tidwrite[nimata_w];   //create 
	double writecost;
	long long writestart,writeend;

	char val[VSIZE + 1];
	char sbuf[1024];
	memset(key, 0, KSIZE + 1);
	memset(val, 0, VSIZE + 1);
	memset(sbuf, 0, 1024);

	
	//code for read

	start = get_ustime_sec();
	
	for (i = 0; i < count; i++) {
		memset(key, 0, KSIZE + 1);

		/* if you want to test random write, use the following */
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d searching %s\n", i, key);
		thread_args.sk.length = KSIZE; //arguments for db get forward to struct
		thread_args.sk.mem = key; //arguments for db get forward to struct
		int k;
		for (k=0; k<nimata_r; k++){
			pthread_create(&tid[k],NULL,db_get,(void *) &thread_args);  //create thread
		}
		for(k=0; k<nimata_r; k++){
			pthread_join(tid[k],&status);//releasing thread resources
		}
		if ((intptr_t)status) {//the return of db get 
			//db_free_data(sv.mem);
			found++;
		} 

		if ((i % 10000) == 0) {
			fprintf(stderr,"random read finished %d ops%30s\r", 
					i, 
					"");

			fflush(stderr);
		}
		
	}

	end = get_ustime_sec();

	//code for write 

	writestart = get_ustime_sec();
	
	for (i = 0; i < count; i++) {
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);

		thread_args.sk.length = KSIZE; //arguments for db get forward to struct
		thread_args.sk.mem = key; //arguments for db get forward to struct
		thread_args.sv.length = VSIZE; //arguments for db get forward to struct
		thread_args.sv.mem = val; //arguments for db get forward to struct
		int k;
		for (k=0; k<nimata_w; k++){
			pthread_create(&tidwrite[k],NULL,db_add,(void *) &thread_args);  //create thread
		}
		for(k=0; k<nimata_w; k++){
			pthread_join(tidwrite[k],&status);//waits for a thread to terminate
		}
		//db_add(db, &sk, &sv);
		if ((i % 10000) == 0) {
			fprintf(stderr,"random write finished %d ops%30s\r", 
					i, 
					"");

			fflush(stderr);
		}
	}
	//--------------------------------------------------------------------------------------------
	//db close
	db_close(thread_args.db);//we use it for the struct

	cost = end -start;//read total time 

	writeend = get_ustime_sec();//stop write stopwatch 
	writecost=writeend-writestart;//write total time 

	printf(LINE);
	printf("|Random-Read	(done:%ld, found:%d): %.6f sec/op; %.1f reads /sec(estimated); cost:%.3f(sec)\n",
		count, found,
		(double)(cost / count),
		(double)(count / cost),
		cost);

	printf(LINE);
	printf("|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); writecost:%.3f(sec);\n"
		,count, (double)(writecost / count)
		,(double)(count / writecost)
		,writecost);

	printf(LINE);
	printf("we have %d threads for read \n we have %d threads for write\n",nimata_r,nimata_w);	
}


void _readwrite(int count,int r,int nimata,double pososto){
	int nimata_r;
	int nimata_w;
	if(pososto<49.0){
		pososto=pososto-100;//take the difference 
		pososto=abs(pososto);//absolute the result
		pososto = pososto/100;//make it %
		nimata_w=pososto*nimata;//give the %
		nimata_r=nimata-nimata_w;//give the %
	}else{
		pososto = pososto/100;//make it %
		nimata_r=pososto*nimata;//give the %
		nimata_w=nimata-nimata_r;//give the %
	}
	_read_write_test(count,r,nimata_r,nimata_w);
		
}
void _readwrite_v2(int count,int r,int nimata){
	srand(time(NULL));//makes use of the computer's internal clock to control the choice of the seed
	int n = rand() % 2;//if odd make it 0 else 1
	if (n==0){
		_write_test(count,r,1);
	}else{
		_read_test(count,r,nimata);
	}
}
