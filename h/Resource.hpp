//
// Created by os on 8/29/25.
//

#ifndef PROJECT_BASE_V1_1_RESOURCE_HPP
#define PROJECT_BASE_V1_1_RESOURCE_HPP

#include "syscall_c.hpp"
class Resource {
public:
    Resource(int N);
    void take(int num_of_instances);
    int give_back(int num_of_instances);

private:
    sem_t mutex;
    sem_t avaliable;
    int totalInstances;
    int avaliableInstances;
};


#endif //PROJECT_BASE_V1_1_RESOURCE_HPP
