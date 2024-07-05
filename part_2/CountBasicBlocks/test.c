#include <stdio.h>

void testFunction(int x)
{
    if (x == 1337)
    {
        puts("nice one");
    }
    else
    {
        puts("sad");
    }
}

void someFunction()
{
    for (int i = 0; i < 10; i++)
    {
        printf("%d\n", i);
    }
}

void anotherFunction()
{
    while (1)
    {
        puts("abcd");
    }
}

int main()
{
    return 0;
}