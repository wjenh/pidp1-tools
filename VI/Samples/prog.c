/* prog.c -- a small program to try :ta and ^] on */

int count;

int square(int n)
{
    return n * n;
}

int sum(int n)
{
    int i, s;

    s = 0;
    for (i = 1; i <= n; i++) {
        s += square(i);
    }
    return s;
}

int main(void)
{
    count = sum(10);
    return 0;
}
