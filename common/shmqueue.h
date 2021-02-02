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

/**
 * @brief 创建共享内存
 *
 * @param[in] key 共享内存shmkey
 * @param[in] length 共享内存可存放的元素个数
 * @param[in] size 每个元素内存大小
 * @param[in] initialize 是否初始化memset内存区域
 *
 * @retval =0 -- successful
 * @retval !0 -- failed
 *
 * @note
 *
 * @see ShmPermission
 * 
 * @pre key 大于0
 */
int shm_create(int key, size_t length, ShmPermission shmPermission, size_t size, bool initialize = false);
int shm_detach(int key);
int shm_delete(int key);
int shm_push(int key, const void* p);
int shm_pop(int key, void* p);
int shm_popout(int key);

#endif // !_SHM_QUEUE_H_