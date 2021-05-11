#include <stdio.h>

int nextPowerOf2(int n){
    int count = 0;

    if(n && !(n & (n-1)))
        return n;

    while(n != 0){
        n >>= 1;
        count++;
    }



    return 1 << count;
}

int main(){
    int n = 6;
    printf("%d" , nextPowerOf2(n));

    return 0;
}