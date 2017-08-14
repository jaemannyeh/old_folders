
#include "yawrz.h"

#if 0
int main(int argc, char *argv[]) {
    
    yawrz::cli_main(argc,argv);

    return 0;
}
#else

void three_numbers_recursion(int *numbers, int size, int target) {
    if (size == 0)
        return;
    three_numbers_recursion(numbers++,size-1,target);
}

void three_numbers_loop(int *numbers, int size, int target) {
    for (int d1=0; d1<size; d1++) {
        for (int d2=d1+1; d2<size; d2++) {
            if (d1 == d2)
                continue;
            for (int d3=d2+1; d3<size; d3++) {
                if (d1 == d3 || d2 == d3)
                    continue;
                if (numbers[d1]+numbers[d2]+numbers[d3] == target) {
                    printf("%d + %d + %d = %d\n",numbers[d1],numbers[d2],numbers[d3],target);
                }
            }
        }
    }
}

int main() {
    int numbers[] = { 1,3,5,7,9 };
    three_numbers_loop(numbers,sizeof(numbers)/sizeof(numbers[0]),19);
    three_numbers_recursion(numbers,sizeof(numbers)/sizeof(numbers[0]),19);    
    return 0;
}

#endif
