#include <iostream>
#include <pthread.h> 
#include <vector>
#include <random>
#include <algorithm>

pthread_mutex_t deckAccessMutex = PTHREAD_MUTEX_INITIALIZER; 

pthread_cond_t thread_one_turn_cv = PTHREAD_COND_INITIALIZER; 

pthread_cond_t threadOneToTwo_awk = PTHREAD_COND_INITIALIZER; 
pthread_cond_t threadOneToTwo_signal = PTHREAD_COND_INITIALIZER; 
pthread_cond_t threadTwoToThree_awk = PTHREAD_COND_INITIALIZER;


pthread_cond_t thread_two_turn_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t thread_three_turn_cv = PTHREAD_COND_INITIALIZER;

pthread_cond_t startOfRound; 
pthread_cond_t end_of_round_cv;
pthread_cond_t endOfGame_cv; 



std::vector<int> v {1,2,3,4,5,6,7};


int arr_1[5] = {};
int arr_2[5] = {1};
int arr_3[5] = {2};


void* threadOne(void* arg) {
    // Lock the deckAccessMutex
    pthread_mutex_lock(&deckAccessMutex);
    
    // access gloval vector and gather info
    int f = v.front();
    arr_1[0] = f; 
    v.push_back(f);
    v.erase(v.begin());    
 
   
   
    // Signal to the waiting thread that it's its turn
    pthread_cond_wait(&threadOneToTwo_awk,&deckAccessMutex);
    pthread_cond_signal(&thread_two_turn_cv);


    // Unlock the deckAccessMutex
    pthread_mutex_unlock(&deckAccessMutex);
    
    // Exit successfully
    return NULL;
}

void* threadTwo(void* arg)
{
    

    // Lock the mutex
    pthread_mutex_lock(&deckAccessMutex);

    pthread_cond_signal(&threadOneToTwo_awk);
    pthread_cond_wait(&thread_two_turn_cv, &deckAccessMutex);

    

    int f = v.front();
    arr_2[0] = f; 
    v.push_back(f);
    v.erase(v.begin());

    // Unlock the mutex
    pthread_cond_wait(&threadTwoToThree_awk,&deckAccessMutex);
    pthread_cond_signal(&thread_three_turn_cv);

    pthread_mutex_unlock(&deckAccessMutex);
    
    for(auto x : v )
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    // Return from the thread
    return NULL; 
}


void* threadThree(void* arg)
{
   // Lock the mutex
    pthread_mutex_lock(&deckAccessMutex);
    
    pthread_cond_signal(&threadTwoToThree_awk);
    pthread_cond_wait(&thread_three_turn_cv, &deckAccessMutex);
    
    int f = v.front();
    v.push_back(f);
    arr_3[0] = f; 
    v.erase(v.begin());

    pthread_mutex_unlock(&deckAccessMutex);
    return NULL;
}


/*

learned need to create waiting threads first then the thread that signals


*/

int main()
{
void* result1;
void* result2;
void* result3;

pthread_t agentThread; 

pthread_t playerOne,playerTwo,playerThree; 

pthread_create(&playerOne,NULL,threadOne,NULL);
pthread_create(&playerTwo,NULL,threadTwo,NULL);
pthread_create(&playerThree,NULL,threadThree,NULL);



pthread_join(playerOne, &result1);
pthread_join(playerTwo, &result2); 
pthread_join(playerThree, &result3); 

printf("Thread exited with status %d\n", (intptr_t) result1);
printf("Thread exited with status %d\n", (intptr_t) result2);
printf("Thread exited with status %d\n", (intptr_t) result3);

std::cout << arr_1[0] << std::endl;
std::cout << arr_2[0] << std::endl; 
std::cout << arr_3[0] << std::endl;
return 0; 
}