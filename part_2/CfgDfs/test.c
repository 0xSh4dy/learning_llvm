#include <stdio.h>

int testFunction()
{
    for (int i = 0; i < 10; i++)
    {
        printf("%d\n", i);
    }
    return 1337;
}

int main()
{
    int x = testFunction();
    if (x > 1000)
    {
        puts("Yay");
    }
    else
    {
        puts("Ne");
    }
    return 0;
}