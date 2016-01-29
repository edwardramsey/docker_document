#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>

#define SHMSZ     27

int shm_wr() {
    int shmid;
    key_t key;
    char *shm, *s;

    key = 5678;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        return -1;
    }

    shm = (char*)shmat(shmid, NULL, 0);
    if ((int)(*shm) == -1) {
		printf("shm address error\n");
        return -2;
    }

    s = shm;
    for (char c = 'a'; c <= 'z'; c++)
	{
        *s++ = c;
	}
    s = NULL;

    return 0;
}

int main()
{
	int ret = shm_wr();		
	if(ret == 0)
	{
		printf("shared memory OK\n");
	}

	sleep(100);
}
