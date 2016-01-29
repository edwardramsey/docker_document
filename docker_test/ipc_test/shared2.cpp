#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define SHMSZ     27

int shm_rd() {
    int shmid;
    key_t key;
    char *shm, *s;

    key = 5678;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        return -1;
    }

    shm = (char*)shmat(shmid, NULL, 0);
    if ((int)(*shm) == -1) {
        return -2;
    }

    s = shm;

    for (int i = 0; i < SHMSZ-1; i++)
	{
        printf("%c ", *(s+i));
	}

    printf("\n");
    s = NULL;

    return 0;
}

int main()
{
	int ret = shm_rd();
	if(ret == 0)
	{
		printf("read OK\n");
	}
	return 0;
}
