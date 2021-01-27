#ifndef _NET_BASE_H_
#define _NET_BASE_H_


#include <string>			//size_t

/**
 * @brief ����ָ��IP�˿�
 *
 * @param[in] ip ����IP
 * @param[in] port ���˶˿�
 *
 * @retval >0 -- successful, listen fd
 * @retval <=0 -- failed
 *
 * @note
 *
 * @see
 * 
 * @pre ip ����ΪNULL
 * @pre port ����ΪNULL
 */
int listen_ip_port(const char* sIP, const char* sPort);
int create_socket();

int set_socket(int fd);

int epoll_create_fd(int size);
int epoll_add(int eplfd, int fd, void *ptr);
int epoll_del(int eplfd, int fd);


int fd_close(int fd);
int fd_read(int fd, char *buf, size_t len);
int fd_write(int fd, char *buf, size_t len);




#endif //!_NET_BASE_H_