#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <linux/limits.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "s_file.h"
#include "s_macros.h"

SFile *s_file_create(const char *path, int flag, int mode)
{
    int ret;
    size_t path_len;
    SFile *file = NULL;
    struct stat sb;

    if S_UNLIKELY (path == NULL)
        return NULL;
    /*
     * Todo
     * If path is not absolute path,
     * It is needs to process about the path not return NULL
     */

    path_len = strlen(path);
    if S_UNLIKELY (path_len > PATH_MAX)
        return NULL;

    ret = s_file_is_absolute_path(path);
    if S_UNLIKELY (ret == FALSE)
        return NULL;

    ret = lstat(path, &sb);
    if S_UNLIKELY (ret == -1)
        return NULL;

    if S_UNLIKELY (!(sb.sb_mode & S_IFREG))
        return NULL;

    file = (SFile *)calloc(1, sizeof(SFile));
    if S_UNLIKELY (file == NULL)
        return NULL;

    file->path = strndup(path, PATH_MAX);
    if S_UNLIKELY (file->path == NULL) {
        s_file_destroy(file);
        return NULL;
    }

    file->sb = sb;
	file->status = S_FILE_STATUS_CLOSE;

    return file;
}

void s_file_destroy(SFile *file)
{
    if S_UNLIKELY (file == NULL)
        return;

    if (file->path)
        free(file->path);
    if (file->fd)
        close(file->fd);
    free(file);
    file = NULL;
}

s_file_is_open(SFile *file)
{
	return file->status == S_FILE_STATUS_OPEN ? TRUE : FALSE;
}

int s_file_is_open(SFile *file)
{
	if S_UNLIKELY (file == NULL)
		return -1;

	if (file->status == S_FILE_STATUS_OPEN)
		return -1;

    file->fd = open(path, flag, mode);
    if (file->fd == -1)
        return -1;

	file->status = S_FILE_STATUS_OPEN;

	return 0;
}

int s_file_is_absolute_path(const char *path)
{
    if S_UNLIEKLY (path == NULL)
        return FALSE;

    if (path[0] == '/')
        return TRUE;
    return FALSE;
}
