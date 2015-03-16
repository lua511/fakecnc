#include <stdio.h>
#include <time.h>
int main(int v,char** c){
    int i = 0;
    time_t rawtime;
    struct tm* timeinfo;
    if(1) return 0;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("halcmd:%d,",v);
    for(i = 0; i < v; ++i){
        if(i > 0) printf(" ");
        printf("%s",c[i]);
    }
    printf(" %s ",asctime(timeinfo));
    printf("\n");
    return 0;
}
