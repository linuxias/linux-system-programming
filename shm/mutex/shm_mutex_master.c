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
    pthread_mutex_t mtx;
    int idx;
} shm_struct_t;

static shm_struct_t *shm;
static pthread_mutexattr_t mtx_attr;

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

    memset(shm, 0, shm_size);

    pthread_mutexattr_init(&mtx_attr);
    ret = pthread_mutexattr_setpshared(&mtx_attr, PTHREAD_PROCESS_SHARED);
    rete_if(ret != 0, ret, "pthread_mutexattr_setpshared");

    ret = pthread_mutex_init(&shm->mtx, &mtx_attr);
    rete_if(ret != 0, ret, "pthread_mutex_init");

	return 0;
}

static int __destory_shared_memory(void)
{
    pthread_mutex_destroy(&shm->mtx);
    pthread_mutexattr_destroy(&mtx_attr);

    munmap(shm, sizeof(shm_struct_t));
}

int main(int argc, char *argv[])
{
    int ret;
    int i;

    ret = __init_shared_memory();
    retm_if(ret != 0, ret, "__init_shared_memory");

    while (shm->idx < 10);

    for (i = 0; i < 1000000; i++)
    {
        pthread_mutex_lock(&shm->mtx);
        shm->idx++;
        pthread_mutex_unlock(&shm->mtx);
    }

	sleep(3);
    printf("%d\n", shm->idx);
    __destory_shared_memory();

    return 0;
}

