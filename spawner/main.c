#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
// #include <emmintrin.h>
#include <stdbool.h>


bool no_bits_set(uint8_t *arr,uint32_t dif){
    bool res=false;
    for(uint32_t i=0;i<dif;i++){
        res|=(arr[i/8]>>(7-(i%8)))&1;
    }
    return !res;
}

void find_pow(char *chall, int challlen, int diffi,uint64_t start,uint64_t end){
    uint8_t res[32];
    char num[challlen+32];
    strcpy(num,chall);
    for(uint64_t i=start;i<end;i++){
        int len=sprintf(num+challlen,"%lu",i);
        SHA256(num,len+challlen,res);
        
        if(no_bits_set(res,diffi)){
            printf("%s:",num);
            for(int i=0;i<32;i++)printf("%02x",res[i]);
            printf("\n");
            break;
        }
    }
}

int main(int argc,char** argv){

    if(argc<3)return 1;
    // char *pos=NULL;
    char *chall=argv[1];
    int challlen=strlen(chall);
    int diffi=atoi(argv[2]);
    // TODO: need sanity check
    int proc = argc==3?get_nprocs():atoi(argv[3]);
    
    
    int link[proc][2];
    int pids[proc];
    __uint64_t rk=(__UINT64_MAX__/proc);
    int mainpid=0;
    
    // for(int i=0;i<proc;i++){
    //     printf("worker %s %s %lu %lu\n",argv[1], argv[2],rk*i,rk*i+rk);
    // }

    for(int i=0;i<proc;i++){
        if (pipe(link[i])==-1) exit(printf("pipe Failed : %d",errno));
        if (fcntl(link[i][0], F_SETFL, O_NONBLOCK)==-1) exit(printf("fcntl Failed : %d",errno));
        if ((pids[i] = fork()) == -1) exit(printf("fork Failed : %d",errno));
        if(pids[i] == 0) {
            prctl( PR_SET_PDEATHSIG, SIGKILL );
            dup2(link[i][1], STDOUT_FILENO);
            close(link[i][0]);
            close(link[i][1]);
            // char st[10]={0,}, ed[10]={0,};
            // sprintf(st,"%lu",rk*i);sprintf(ed,"%lu",rk*i+rk);
            find_pow(chall,challlen,diffi,rk*i,rk*i+rk);
            exit(0);
        } else {
            close(link[i][1]);
        }
    }
    char foo[4096];
    int nbytes=-1;
    
    while(nbytes<=0){
        for(int i=0;i<proc;i++){
            nbytes = read(link[i][0], foo, sizeof(foo));
            if(nbytes>0){
                //TODO : need to read all till end.
                printf("Process {%d} found answer!\n",i);
                break;
            }
        }
        sleep(1);
    }
    for(int i=0;i<proc;i++)kill(pids[i],SIGKILL);
    printf("%s\n",foo);
}
