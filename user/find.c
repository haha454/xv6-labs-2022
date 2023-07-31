#include <stdbool.h>
#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

bool ends_with(const char *path, const char *name)
{
    uint path_len = strlen(path);
    uint name_len = strlen(name);
    if (path_len < name_len)
    {
        return false;
    }

    int i;
    for (i = name_len - 1; i >= 0; i--)
    {
        if (path[path_len - name_len + i] != name[i])
        {
            return false;
        }
    }

    return true;
}

int find(const char *path, const char *name)
{
    int fd;
    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open %s (%d)\n", path, strlen(path));
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s (%d)\n", path, strlen(path));
        close(fd);
        return 1;
    }

    if (ends_with(path, name))
    {
        printf("%s\n", path);
    }

    if (st.type != T_DIR)
    {
        close(fd);
        return 0;
    }

    struct dirent de;
    char buf[512];
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2, "find: path %s too long\n", path);
        close(fd);
        return 1;
    }
    strcpy(buf, path);
    char *p = buf + strlen(buf);
    *p = '/';
    p++;
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, name);
    }
    close(fd);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        return find(".", argv[1]);
    }
    else if (argc == 3)
    {
        return find(argv[1], argv[2]);
    }
    else
    {
        fprintf(2, "Usage: find <name> or find <path> <name>\n");
        return 1;
    }
}