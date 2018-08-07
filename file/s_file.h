#ifndef __S_FILE_H__
#define __S_FILE_H__

#include <sys/stat.h>

typedef struct _SFile SFile;

struct _SFile {
    char *path;
    int fd;
    struct stat sb;
};

SFile *s_file_create(const char *path);

void s_file_destroy(SFile *file);

int s_file_is_absolute_path(const char *path);



#endif /* __S_FILE_H__ */
