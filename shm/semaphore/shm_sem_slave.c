#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>

#define retm_if(expr, val, msg) do { \
    if (expr) \
    { \
        printf("%s\n", (msg)); \
        return (val); \
    } \
} while(0)

#define retv_if(expr, val) do { \
    if (expr) \
    { \
        return (val); \
    } \
} while(0)

#define rete_if(expr, val, msg) do { \
    if (expr) \
    { \
        printf("%s, errno : %d, errstr : %s\n", msg, errno, strerror(errno)); \
        return (val); \
    } \
} while(0)


#define SHM_NAME "shm_mem_test"

typedef struct shm_struct {
    sem_t sem;
    int idx;
} shm_struct_t;

static shm_struct_t *shm;

static int __init_shared_memory(void)
{
    int ret;
	int shm_fd;
    int shm_size;

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (shm_fd == -1) {
        printf("shm_open %d %s\n", errno, strerror(errno));
        return -1;
    }

    shm_size = sizeof(shm_struct_t);
    ret = ftruncate(shm_fd, shm_size);
    rete_if(ret == -1, ret, "ftruncate");

    shm = (shm_struct_t *)mmap(NULL, shm_size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    retm_if(shm == MAP_FAILED, -1, "mmap()");

    close(shm_fd);

	return 0;
}

static int __destory_shared_memory(void)
{
    munmap(shm, sizeof(shm_struct_t));
}

int main(int argc, char *argv[])
{
    int ret;
    int i;

    ret = __init_shared_memory();
    retm_if(ret != 0, ret, "__init_shared_memory");

    for (i = 0; i < 1000000; i++)
    {
        sem_wait(&shm->sem);
        shm->idx++;
        sem_post(&shm->sem);
    }

    __destory_shared_memory();

    return 0;
}

