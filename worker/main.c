#include <openssl/sha.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <emmintrin.h>
#include <stdbool.h>

// bool no_bits_set(uint8_t *arr, __m128i mask)
// {
//   __m128i v = _mm_loadu_si128((const __m128i*)arr);
//   return  _mm_testz_si128(mask, v);
// }

bool no_bits_set(uint8_t *arr,uint32_t dif){
    bool res=false;
    for(uint32_t i=0;i<dif;i++){
        res|=(arr[i/8]>>(7-(i%8)))&1;
    }
    return !res;
}

int main(int argc,char** argv){
    
    if(argc!=5)return 1;
    char *pos=NULL;
    char *chall=argv[1];
    int challlen=strlen(chall);
    int diffi=atoi(argv[2]);
    uint64_t start=strtoull(argv[3],&pos,10),
        end=strtoull(argv[4],&pos,10);

    uint8_t res[32];
    char num[challlen+16];
    for(uint64_t i=start;i<end;i++){
        int len=sprintf(num,"%s%lu",chall,i);
        SHA256(num,len,res);
        
        if(no_bits_set(res,diffi)){
            printf("%s:",num);
            for(int i=0;i<32;i++)printf("%02x",res[i]);
            printf("\n");
            break;
        }
    }
}
