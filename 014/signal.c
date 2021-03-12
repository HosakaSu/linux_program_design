#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/sem.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);

static int sem_id;

int main(int argc,char *argv[]){
    int i;
    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime(&t);
    int pause_time;
    char op_char = 'O';

    srand((unsigned int)getpid());//根据当前进程识别码获取随机数
    /*semget(key_t key, int num_sems, int sem_flags) 初始化信号
    key ：不相关进程通过同一key值访问同一信号量
    num_sems：指定需要的信号量数量
    sme_flags：低端的9位 设置信号量权限
    */
    sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);

    if(argc >1){
        printf ( " %d:%d:%d\n",lt->tm_hour, lt->tm_min, lt->tm_sec);
        if(!set_semvalue()){//执行程序带参数，初始化信号
            fprintf(stderr,"failed to init semaphore\n");
            exit(EXIT_FAILURE);
        }
        op_char = 'X';//如果程序是第一个被调用的，则初始化信号量并赋值op_char
        sleep(2);
    }

    for(i = 0;i<10;i++)
    {
        if(!semaphore_p()) exit(EXIT_FAILURE);
        printf("%c",op_char);
        fflush(stdout);
        pause_time = rand() % 3;
        sleep(pause_time);
        printf("%c",op_char);
        fflush(stdout);

        if(!semaphore_v()) exit(EXIT_FAILURE);

        pause_time = rand() % 2;
        sleep(pause_time);
    
    }

    printf("\n%d - finished\n",getpid());
    time(&t);
    lt = localtime(&t);
    printf ( " %d:%d:%d\n",lt->tm_hour, lt->tm_min, lt->tm_sec);
    if(argc>1)
    {
        sleep(10);
        time(&t);
        lt = localtime(&t);
        printf ( " %d:%d:%d\n",lt->tm_hour, lt->tm_min, lt->tm_sec);
        del_semvalue();
    }

    exit(EXIT_SUCCESS);
}


static int set_semvalue(void)
{
    union semun sem_union;
    sem_union.val = 1;
    if(semctl(sem_id,0,SETVAL,sem_union) == -1) return(0);
    return(1);

}

static void del_semvalue(void)
{
    union semun sem_union;
    if(semctl(sem_id,0,IPC_RMID,sem_union) == -1){
        fprintf(stderr,"Failed to delete semaphore\n");
    }
}

static int semaphore_p(void)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = -1;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(sem_id,&sem_b,1) == -1)
    {
        fprintf(stderr,"semaphore_p failed\n");
        return(0);
    }
    return(1);
}


static int semaphore_v(void)
{
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(sem_id,&sem_b,1) == -1)
    {
        fprintf(stderr,"semaphore_v failed\n");
        return(0);
    }
    return(1);
}
