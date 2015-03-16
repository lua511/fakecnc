#include <stdio.h>
int main(int v,char** c){
    int i = 0;
    if(v >= 6){
        if(c[6][0] == 'D' && c[6][1] == 'I' && c[6][2] == 'S'){
            printf("testnothing\n");
            return 0;
        }
    }
    printf("fmilltask\n");
    for(i = 1;i < v; ++i){
        printf("%s ",c[i]);
    }
    printf("\n");
    return 0;
}
