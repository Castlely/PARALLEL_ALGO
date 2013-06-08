#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>



void add(int process_no,int range);

#define num 10
int A[num];
int B[num];

//Semaphore turn;
pthread_barrier_t barrier;

int main()
   {
      int i=0;
      pthread_barrier_init(&barrier,NULL,num/2);
   
      pthread_t p[num/2];
  
      for(i=0;i<num/2;i++)
      {
         pthread_create(&p[i],NULL,
      (void *)&add(i+1,2), NULL);
      }
   
   
      pthread_barrier_destroy(&barrier);
   
   
   
   
     
      
   }
   
void add(int process_no,int range)
{
   
      B[process_no*range-1]=B[process_no*range-1]+B[process_no*range];
      
      pthread_barrier_wait(&barrier);
      
         int a=0;
         int k=2;
         
         while(range<=num/2)
         {
            a=B[process_no*range];
            
            while(k<=process_no*range)
            {
               a=a+B[process_no*range-k];
               k=k+k;
            }
            
            
            pthread_barrier_wait(&barrier);
            a=B[process_no*range];
            
         }        
     
   
}
