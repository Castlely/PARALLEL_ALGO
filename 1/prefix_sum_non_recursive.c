#include<pthread.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>
#include<memory.h>
#include<stdlib.h>
#include<math.h>


#define RANGE 99
#define M 1000

void generate(int n);
void Read(int n);
void *func1(void *arg);
void *func2(void *arg);
void *func3(void *arg);
void calculate();
void resultToFile();

int A[M];
int B[20][1000];
int prefix_sum[20][1000]={0};
pthread_t tids[20][1000];
pthread_mutex_t mutex;
int n;

struct S
{
	int i ;
	int j ;
};

int main()
{
	printf("THE ARRAY'S SCALE IS:  ");
	scanf("%d",&n);
	generate(n);
	Read(n);

	clock_t  start = clock();
	calculate();
	resultToFile();
	clock_t finish = clock();

	printf("COST TIME  %.20f SEC\n",(double)(finish-start)/CLOCKS_PER_SEC);
	return 0;
}


void calculate()
{
	int h,i,j,k,l,flag;
	/*======set B[0][j]=A[j] in parallel==================*/
	for(i=1;i<=n;i++)
	{
		struct S *s;
		s = (struct S *)malloc(sizeof(struct S));
		s->i = i;
		if( pthread_create(&tids[0][i],NULL,func1,s) )//第四个参数arg需要一个独立的内存空间，因为i打值在不停的变化，很有可能若干个thread同时引用来相同的i值
		{
			perror("Fail to create thread!");			
			exit(1);
		}
	}
	for(j=1;j<=n;j++)
		pthread_join(tids[0][j],NULL);

	/***Show the B[0][j]***************************/
//#if 1
	printf("A[i]:   ");
	for(j=1;j<=n;j++)
		printf("%d ",A[j]);
	printf("\nAFTER COPY, B EQUALS A:");
	for(j=1;j<=n;j++)
		printf("%d ",B[0][j]);
	printf("\n");
//#endif
	/***********************************************/

	/**Data Flow Forward Algorithm:build the tree***/
	for(h=1;h<=log2(n);h++)
	{
		for(j=1;j<=n/pow(2,h);j++)
		{
			struct S *s;
			s = (struct S*)malloc(sizeof(struct S));
			s->i = h;
			s->j = j;
			if( pthread_create(&tids[h][j],NULL,func2,s) )
			{
				perror("sth is wrong");
				exit(1);
			}
		}
		/*******等待以每一层的操作完成***/
		for(j=1;j<=n/pow(2,h);j++)
			pthread_join(tids[h][j],NULL);
	}
	
//#if 1
	/******Print Data Flow Forward************/
	printf("SUM FORWARD:\n");
	for(h=0;h<=log2(n);h++)
	{
		for(l=1;l<=2*h+1;l++)
			printf(" _ ");
		for(j=1;j<=n/pow(2,h);j++)
		{
			printf("%d",B[h][j]);
			for(l=1;l<=1+h;l++)
				printf(" _ ");
		}
		printf("\n");
	}
//#endif
	
	/*********************************************/

	/***********Data Flow Backward:Traverse*******/
	for(h=log2(n);h>=0;h--)	
	{
		for(j=1;j<=n/pow(2,h);j++)
		{
			struct S *s;
			s = (struct S*)malloc(sizeof(struct S));
			s->i = h;
			s->j = j;
			if( pthread_create(&tids[h][j],NULL,func3,s) )
			{
			perror("sth is wrong");
			exit(1);
			}	
		}
		for(j=1;j<=n/pow(2,h);j++)
			pthread_join(tids[h][j],NULL);
	}

//#if 1
	/********Print Data Flow Backward************/
	printf("SUM BACKWARD:\n");
	for(h=log2(n);h>=0;h--)	
	{
		for(l=1;l<=2*h+1;l++)
			printf(" _ ");
		for(j=1;j<=n/pow(2,h);j++)
		{
			printf("%d ",prefix_sum[h][j]);
			for(l=1;l<=h;l++)
				printf(" _ ");
		}	
		printf("\n");
	}
//#endif
	/*********************************************/
}

void resultToFile()
{
	int i;
	/***********write the result to file**********/		
	FILE *file	;
	file = fopen("/home/ly/parallel/1/prefixSumResult.txt","wt");
	for(i=1;i<=n;i++)
	{
		fprintf(file,"%-6d",prefix_sum[0][i]);
	}
	/*********************************************/		

}

void *func1(void *arg)//set B[0][j]=A[j]
{
	int i;
	struct S *p;
	p = (struct S*)arg;
	i = p->i;
	pthread_mutex_lock(&mutex);
	B[0][i]=A[i];
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *func2(void *args)//B[h][j]=B[h-1][2*j-1]+B[h-1][2*j];
{
	int h , j;
	struct S *p;
	p = (struct S*)args;
	h = p->i;
	j = p->j;
	pthread_mutex_lock(&mutex);
	B[h][j]=B[h-1][2*j-1]+B[h-1][2*j];
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *func3(void *arg)//get the prefix sum
{
	int h,j;
	struct S *p;
	p = (struct S*)arg;
	h = p->i;
	j = p->j;
	pthread_mutex_lock(&mutex);
	if(j==1) prefix_sum[h][1] = B[h][1];
	else if (j%2==0)  prefix_sum[h][j] = prefix_sum[h+1][j/2] ;
	else prefix_sum[h][j] = prefix_sum[h+1][(j-1)/2] + B[h][j] ;
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void generate(int n)
{
	FILE *file1;
	if( (file1=fopen("/home/ly/parallel/1/arrayA.txt","wt") )==NULL )
	{
		perror("fopen");
		exit(1);
	}
	
	int i,j;
	srand( (unsigned)time(NULL) );
	for( i = 1 ; i <= n ;i++)
		fprintf(file1,"%-8d",rand()%RANGE);
	fprintf(file1,"\n");
	fclose(file1);
}

void Read(int n)
{
	FILE *file1;
	if( (file1=fopen("/home/ly/parallel/1/arrayA.txt","rt") )==NULL )
	{
		perror("fopen");
		exit(1);
	}
	int i,j;
	srand( (unsigned)time(NULL) );
	for( i = 1 ; i <= n ;i++)
		fscanf(file1,"%d",&A[i]);
	fclose(file1);
}









