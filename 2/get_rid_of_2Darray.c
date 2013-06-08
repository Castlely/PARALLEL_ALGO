#include<stdio.h>
#include<time.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<memory.h>
#include<stdlib.h>
#include<math.h>

#define RANGE 99
#define M 1000

void generate();
void Read();
void *func1(void *arg);
void *func2(void *arg);
void calculate();
void resultToFile();

int A[M]={0};
int prefix_sum[M]={0};
int temp[M]={0};
int n;
pthread_t tids[1000][1000];
pthread_mutex_t mutex;

struct S
{
	int i ;
	int j ;
};

int main()
{
	printf("ARRAY'S SCALE : ");
	scanf("%d",&n);
	generate();
	Read();

	clock_t start = clock();
	calculate();
	resultToFile();
	clock_t finish = clock();
	
	printf("COST TIME IS  : %.20f second\n",(double)(finish-start)/CLOCKS_PER_SEC);
	return 0 ;
}

void resultToFile()
{
	int i;
	/***********write the result to file**************/
	FILE *file;
	file = fopen("/home/ly/parallel/2/GetRidOf2DArray.txt","wt");
	for(i=1;i<=n;i++)
	{
		fprintf(file,"%-6d",prefix_sum[i]);
	}
	/*************************************************/
}

void calculate()
{
	int h,i,j,k,l ;
	/************************************************/
	for(i=1;i<=n;i++)
	{
		struct S* s;
		s = (struct S*)malloc(sizeof(struct S));
		s->i = i;
		if( pthread_create(&tids[0][i],NULL,func1,s))
		{
			perror("Fail to create thread!");
			exit(1);
		}
	}
	for(j=1;j<=n;j++)
		pthread_join(tids[0][j],NULL);
	/************************************************/

	k=2;
	while(k<n)
	{
		for(i=1;i<=n;i++)
			temp[i] = prefix_sum[i];
		for(i=k+1;i<=n;i++)
		{
			struct S* s;
			s = (struct S*)malloc(sizeof(struct S));
			s->i = i;
			s->j = k;
			if( pthread_create(&tids[k][i],NULL,func2,s))
			{
				perror("Fail to create thread!");
				exit(1);
			}
		}
		for(j=k+1;j<=n;j++)
			pthread_join(tids[k][j],NULL);

		for(i=1;i<=n;i++)
			prefix_sum[i] = temp[i];
		k = k + k;
	}

#if 0
	/************show the result***************/
	printf("A: \n");
	for(i=1;i<=n;i++)
		printf("%d ",A[i]);
	printf("\nprefix_sum:\n");
	for(i=1;i<=n;i++)
		printf("%d ",prefix_sum[i]);
	printf("\n");
	/******************************************/
#endif
	
}

void *func1(void *arg)
{
	int i ;
	struct S *p;
	p = (struct S*)arg;
	i = p->i;
	pthread_mutex_lock(&mutex);
	if(i==1)  prefix_sum[i] = A[i];
	else	  prefix_sum[i] = A[i-1] + A[i];
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *func2(void *arg)
{
	int i ,k ;
	struct S *p;
	p = (struct S*)arg;
	i = p->i;
	k = p->j;
	pthread_mutex_lock(&mutex);
	temp[i] = prefix_sum[i-k] + prefix_sum[i];
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}


void generate()
{
	FILE *file;
	if( (file = fopen("//home/ly/parallel/2/arrayB.txt","wt"))==NULL )
	{
		perror("fopen");
		exit(1);
	}
	int i , j ;
	srand((unsigned)time(NULL) );
	for( i=1;i<=n;i++ )
		fprintf(file,"%-6d",rand()%RANGE);
	fprintf(file , "\n");
	fclose(file);
}

void Read()
{
	FILE *file;
	if( (file = fopen("/home/ly/parallel/2/arrayB.txt","rt"))==NULL )
	{
		perror("fopen");
		exit(1);
	}
	int i , j ;
	srand((unsigned)time(NULL) );
	for( i=1;i<=n;i++ )
		fscanf(file,"%d",&A[i]);
	fclose(file);
}

