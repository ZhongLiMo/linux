#ifndef _SHM_QUEUE_H_
#define _SHM_QUEUE_H_

#include <string>

//only used for POD struct.
//not safety for multithreading.
//safety for multiprocess, one process read and multiprocess write

//permission of the process to memory.
enum ShmPermission
{
    SHM_READ,   //read only
    SHM_WRITE,  //read and write
};

int shm_create(int key, size_t length, ShmPermission shmPermission, size_t size, bool initialize = false);
int shm_detach(int key);
int shm_delete(int key);
int shm_push(int key, void* p);
int shm_pop(int key, void* p);
int shm_popout(int key);

#endif // !_SHM_QUEUE_H_