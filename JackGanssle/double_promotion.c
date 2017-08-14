#include <stdio.h>

int main()
{
    float x = 0.1;
    
    printf("x %d %d %d\n", sizeof(x), sizeof(0.1), sizeof(0.1f));
    
    if (x == 0.1)
        printf("IF\n");
    else if (x == 0.1f)
        printf("ELSE IF\n");
    else
        printf("ELSE\n");
        
    float y = 0.5;

    printf("y %d %d %d\n", sizeof(y), sizeof(0.5), sizeof(0.5f));
    
    if (y == 0.5)
        printf("IF\n");
    else if (y == 0.5f)
        printf("ELSE IF\n");
    else
        printf("ELSE\n");        
}