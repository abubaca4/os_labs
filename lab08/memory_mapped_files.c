#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main()
{
    const char *mapname = "datafile.dat";

    int fmapd = open(mapname, O_RDWR | O_CREAT, 0600);
    if (fmapd < 0)
    {
        perror("error with open of map file");
        return -1;
    }

    int maxdatalength = 1024;

    ftruncate(fmapd, maxdatalength);

    char *data = (char *)mmap(NULL, maxdatalength, PROT_WRITE | PROT_READ, MAP_SHARED, fmapd, 0);
    if (MAP_FAILED == data)
    {
        perror("error with mmap");
        return -2;
    }

    printf("pointer value = %p\n", data);

    sprintf(data, "hello world!");

    for (int i = 0; i < 10; ++i)
    {
        data[20 + i] = 'a';
    }

    data[50] = 55;
    data[52] = 'z';
    data[54] = 33;

    int res = munmap(data, maxdatalength);
    if (0 != res)
    {
        perror("error with unmapping");
        return -3;
    }
    return 0;
}
