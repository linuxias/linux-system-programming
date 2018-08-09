#ifndef __S_FILE_H__
#define __S_FILE_H__

#include <sys/stat.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FASLE 0
#endif

typedef struct _SFile SFile;

typedef enum _s_file_status s_file_status;

enum _s_file_status {
	S_FILE_STATUS_CLOSE
	S_FILE_STATUS_OPEN,
};

struct _SFile {
    char *path;
    int fd;
	s_file_status status;
    struct stat sb;
};

SFile *s_file_create(const char *path);

void s_file_destroy(SFile *file);

int s_file_is_absolute_path(const char *path);

int s_file_is_open(SFile *file);

#endif /* __S_FILE_H__ */
