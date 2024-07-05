int fib(int n)
{
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}

int testFunction()
{
    return 1000;
}

void anotherFunction()
{
    while (1)
        ;
}

int main()
{
    fib(10);
    return 0;
}