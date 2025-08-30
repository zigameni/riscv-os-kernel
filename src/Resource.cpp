//
// Created by os on 8/29/25.
//

#include "../h/Resource.hpp"
#include "../test/printing.hpp"
Resource::Resource(int N): totalInstances(N), avaliableInstances(N) {

    // Initialize mutex semaphore
    if(sem_open(&mutex, 1)!=0){
        printString("Error: Failed to create mutex semaphore\n");
    }

    if(sem_open(&avaliable, N)!=0){
        printString("Error: Failed to create avaliable semaphore\n");
    }

    printString("Resource created with ");
    printInt(N);
    printString(" Instances\n");
}
void Resource::take(int num_of_instances) {
    if(num_of_instances<=0){
        printString("Error: Invalid number of instances requested\n");
        return;
    }

    if(num_of_instances > totalInstances){
        printString("Error: Requested more instances than total avaliable\n");
        return;
    }

    // Wait for enough resources to become avaliable
    for(int i= 0; i<num_of_instances; i++){
        sem_wait(avaliable);
    }

    // critical section
    sem_wait(mutex);

    avaliableInstances -= num_of_instances;

    sem_signal(mutex);

    printString("Thread acquired ");
    printInt(num_of_instances);
    printString(" instances. Available: ");
    printInt(avaliableInstances);
    printString("\n");
}

int Resource::give_back(int num_of_instances) {
    if(num_of_instances <= 0){
        printString("Error: Invalid number of instances to return\n");
        return -1;
    }

    if(num_of_instances > (totalInstances-avaliableInstances)){
        printString("Error: Trying to return more instances that were taken");
        return -1;
    }

    sem_wait(mutex);

    avaliableInstances+=num_of_instances;

    sem_signal(mutex);

    for(int i = 0; i< num_of_instances; i++){
        sem_signal(avaliable);
    }

    printString("Thread returned ");
    printInt(num_of_instances);
    printString(" instances. Available: ");
    printInt(avaliableInstances);
    printString("\n");

    return 0;


}