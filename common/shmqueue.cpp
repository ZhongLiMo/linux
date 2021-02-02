#include "shm_queue.h"

#include <sys/shm.h>
#include <unordered_map>
#include <string.h>

struct shmqueue
{
    size_t  size;       //size of one cell
    size_t  length;     //total num of cell in queue
    int     shmid;
    void*   pshm;       //each cell has a set bool flag in it's tail
    size_t* head;       //pointer at the head position
    size_t* tail;       //pointer at the tail position
};

static std::unordered_map<int, shmqueue> g_shm;
static std::unordered_map<int, shmqueue>::iterator ite;

int shm_create(int key, size_t length, ShmPermission shmPermission, size_t size, bool initialize)
{
    if (size == 0 || length == 0)
    {
        return -1;
    }
    if (g_shm.find(key) != g_shm.end())
    {
        return -1;
    }
    int shmid = 0;
    size_t total = (size + sizeof(bool)) * length + 2 * sizeof(size_t);
    int shmflg = (shmPermission == SHM_READ) ? 0444 : 0644;
    shmflg = initialize ? (shmflg | IPC_CREAT) : shmflg;
    shmid = shmget(key, total, shmflg);
    if (shmid == -1)
    {
        return -1;
    }
    void* p = shmat(shmid, NULL, 0);
    if (p == (void*)-1)
    {
        return -1;
    }
    if (initialize)
    {
        memset(p, 0, total);
    }
    g_shm[key] = { size, length, shmid, p,
        (size_t*)(reinterpret_cast<size_t>(p) + (size + sizeof(bool)) * length),
        (size_t*)(reinterpret_cast<size_t>(p) + (size + sizeof(bool)) * length + sizeof(size_t)) };
    return 0;
}

int shm_detach(int key)
{
    ite = g_shm.find(key);
    if (ite == g_shm.end())
    {
        return -1;
    }
    if (shmdt(ite->second.pshm) != 0)
    {
        return -1;
    }
    g_shm.erase(key);
    return 0;
}

int shm_delete(int key)
{
    ite = g_shm.find(key);
    if (ite == g_shm.end())
    {
        return -1;
    }
    if (shmctl(ite->second.shmid, IPC_RMID, NULL) != 0)
    {
        return -1;
    }
    g_shm.erase(key);
    return 0;
}

size_t get_tail()
{
    size_t tail = *ite->second.tail;
    while (!__sync_bool_compare_and_swap(ite->second.tail, tail, tail))
    {
        tail = *ite->second.tail;
    }
    return tail;
}

size_t add_tail()
{
    size_t tail = *ite->second.tail;
    size_t next = (tail == (ite->second.length - 1)) ? 0 : (tail + 1);
    while (!__sync_bool_compare_and_swap(ite->second.tail, tail, next))
    {
        tail = *ite->second.tail;
        next = (tail == (ite->second.length - 1)) ? 0 : (tail + 1);
    }
    return tail;
}

size_t get_head()
{
    size_t head = *ite->second.head;
    while (!__sync_bool_compare_and_swap(ite->second.head, head, head))
    {
        head = *ite->second.head;
    }
    return head;
}

size_t add_head()
{
    size_t head = *ite->second.head;
    size_t next = (head == (ite->second.length - 1)) ? 0 : (head + 1);
    *ite->second.head = next;
    return next;
}

int shm_push(int key, void* p)
{
    if (p == NULL)
    {
        return -1;
    }
    ite = g_shm.find(key);
    if (ite == g_shm.end())
    {
        return -1;
    }
    size_t tail = add_tail();
    size_t next = (tail == (ite->second.length - 1)) ? 0 : (tail + 1);
    if (next == get_head())
    {
        return -1;
    }
    memcpy((void*)(reinterpret_cast<size_t>(ite->second.pshm) + tail * (ite->second.size + sizeof(bool))), p, ite->second.size);
    *(bool*)(reinterpret_cast<size_t>(ite->second.pshm) + tail * (ite->second.size + sizeof(bool)) + ite->second.size) = true;
    return 0;
}

int shm_pop(int key, void* p)
{
    if (p == NULL)
    {
        return -1;
    }
    ite = g_shm.find(key);
    if (ite == g_shm.end())
    {
        return -1;
    }
    if (get_tail() == *ite->second.head)
    {
        return -1;
    }
    while (*(bool*)(reinterpret_cast<size_t>(ite->second.pshm) + *ite->second.head * (ite->second.size + sizeof(bool)) + ite->second.size) == false);
    memcpy(p, (void*)(reinterpret_cast<size_t>(ite->second.pshm) + *ite->second.head * (ite->second.size + sizeof(bool))), ite->second.size);
    *(bool*)(reinterpret_cast<size_t>(ite->second.pshm) + *ite->second.head * (ite->second.size + sizeof(bool)) + ite->second.size) = false;
    add_head();
    return 0;
}