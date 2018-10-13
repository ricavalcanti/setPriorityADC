#include <iostream>
#include <pthread.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>
// Inclua as classes que achar necessario

using namespace BlackLib;
void *readValues(void *arg);
void *setPriority1(void *arg);
void *setPriority2(void *arg);
double valueEntry1, valueEntry2;
pthread_mutex_t lock;

void carga(int k)
{
    float f = 0.999999;
    for (int i = 0; i < k; i++)
    {
        f = f * f * f * f * f;
        f = 1.56;
        for (int j = 0; j < k; j++)
        {
            f = sin(f) * sin(f) * f * f * f;
        }
    }
}

int main(int argc, char *argv[])
{
    int res1, res2, res3,res4;
    //t1 le os valores e t2 e t3 atualizam os valores
    pthread_t t1, t2, t3;
    void *resf1;
    void *resf2;
    void *resf3;

    res1 = pthread_mutex_init(&lock, NULL);
    res2 = pthread_create(&t1, NULL, readValues,(void*) 1);
    res3 = pthread_create(&t2,NULL,setPriority1,(void*) 2);
    res4 = pthread_create(&t3,NULL,setPriority2,(void *) 3);
    if (res1 || res2 || res3 || res4)
    {
        perror("Criacao de uma das Threads falhou");
        exit(EXIT_FAILURE);
    }

    res1 = pthread_join(t1, &resf1);
    res2 = pthread_join(t2, &resf2);
    res3 = pthread_join(t3, &resf3);
    if (res1 || res2 || res3)
    {
        perror("Criacao de uma das joins falhou");
        exit(EXIT_FAILURE);
    }

    
    return 0;
}

void *readValues(void *arg)
{
    ADC en1(AIN0);
    ADC en2(AIN1);

    while (true)
    {
        pthread_mutex_lock(&lock);
        valueEntry1 = en1.getFloatValue();
        valueEntry2 = en2.getFloatValue();
        //cout << valueEntry1 << " " << valueEntry2 << endl;
        pthread_mutex_unlock(&lock);
        //cout << valueEntry1 << " " << valueEntry2 << endl;
        sleep(1);
	}
}

void *setPriority1(void *arg)
{
    BlackGPIO led1(GPIO_44, output);
    int ret;
    bool on = false;
    struct sched_param params;
    int policy = SCHED_RR;
    pthread_t this_thread = pthread_self();
    // pthread_attr_t thAttr;
    int maxPriority = sched_get_priority_max(policy); //99
    int minPriority = sched_get_priority_min(policy); 
    while (true)
    {

	    //pthread_mutex_lock(&lock);
        /*pthread_attr_init(&thAttr);
        pthread_attr_getschedpolicy(&thAttr, &policy);
        maxPriority = sched_get_priority_max(policy);
        minPriority = sched_get_priority_min(policy);*/
        if (valueEntry1 > valueEntry2)
        {
            // params.sched_priority = maxPriority;
            // ret = pthread_setschedparam(this_thread, SCHED_RR,&params);
            params.sched_priority = maxPriority;
            ret = pthread_setschedparam(this_thread, policy, &params); 
	        //ret = pthread_setschedprio(this_thread,maxPriority);
        }
        else
        {
            // params.sched_priority = media;
	        // ret = pthread_setschedparam(this_thread,SCHED_RR,&params);
            params.sched_priority = minPriority;
            ret = pthread_setschedparam(this_thread, policy, &params);             
            //ret = pthread_setschedprio(this_thread,minPriority);
	    }
        //pthread_attr_destroy(&thAttr);
	    //pthread_mutex_unlock(&lock);
        if (ret != 0)
        {
            std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
           // exit(-1);
        }
        if (on)
        {
            carga(2000);
            led1.setValue(high);
            on = false;
        }
        else
        {
            carga(2000);
            led1.setValue(low);
            on = true;
        }
    }
}

void *setPriority2(void *arg)
{
    BlackGPIO led2(GPIO_45, output);
    int ret;
    bool on = false;
    struct sched_param params;
    int policy = SCHED_RR;
    pthread_t this_thread = pthread_self();
    //pthread_attr_t thAttr;
    int maxPriority = sched_get_priority_max(policy); //99
    int minPriority = sched_get_priority_min(policy); 

    while (true)
    {
        //pthread_mutex_lock(&lock);
        /*pthread_attr_init(&thAttr);
        pthread_attr_getschedpolicy(&thAttr, &policy);
        maxPriority = sched_get_priority_max(policy);
        minPriority = sched_get_priority_min(policy);*/
        if (valueEntry1 < valueEntry2)
        {
            // params.sched_priority = maxPriority;
            // ret = pthread_setschedparam(this_thread, SCHED_RR,&params);
	        params.sched_priority = maxPriority;
            ret = pthread_setschedparam(this_thread, policy, &params); 
            //ret = pthread_setschedprio(this_thread,maxPriority);
        }
        else
        {
            // params.sched_priority = media;
	        // ret = pthread_setschedparam(this_thread,SCHED_RR,&params);
            params.sched_priority = minPriority;
            ret = pthread_setschedparam(this_thread, policy, &params); 
            //ret = pthread_setschedprio(this_thread,minPriority);
	    }
        //pthread_attr_destroy(&thAttr);
	    //pthread_mutex_unlock(&lock);
        if (ret != 0)
        {
            std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
           // exit(-1);
        }
        if (on)
        {
            carga(2000);
            led2.setValue(high);
            on = false;
        }
        else
        {
            carga(2000);
            led2.setValue(low);
            on = true;
        }
    }
}


/*void *setPriority2(void *arg)
{
    BlackGPIO led2(GPIO_45, output);
    int ret;
    bool on = false;
    struct sched_param params;
	int maxPriority = sched_get_priority_max(SCHED_RR);
	int minPriority = sched_get_priority_min(SCHED_RR);
	int media = (maxPriority-minPriority)/2;
    while (true)
    {
        //pthread_mutex_lock(&lock);
        //int prio = valueEntry1 * 60;
        //cout << prio << endl;
        //std::cout << valueEntry1 << " : " << valueEntry2 << std::endl;
        //pthread_mutex_unlock(&lock);
        pthread_t this_thread = pthread_self();

        if (valueEntry1 > valueEntry2)
        {	
            params.sched_priority = sched_get_priority_max(SCHED_RR);
			//cout << "value1: " << sched_get_priority_max(SCHED_RR) << endl;
        }
        else
        {
            params.sched_priority = media;
        }

        ret = pthread_setschedparam(this_thread, SCHED_RR, &params);

        if (ret != 0)
        {
            std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
            exit(-1);
        }
        if (on)
        {
            carga(2000);
            led2.setValue(high);
			sleep(500);
            on = false;
        }
        else
        {
            carga(2000);
            led2.setValue(low);
			sleep(500);
            on = true;
        }
    }
}*/
