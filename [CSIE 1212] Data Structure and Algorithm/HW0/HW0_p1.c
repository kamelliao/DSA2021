#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define MAX_LEN 300

void print_array(int arr[]);
void to_int_arr(char num_str[], int num[]);
bool comparator(int a[], int b[]);
bool is_zero(int num[]);
bool is_even(int num[]);
void subtraction(int a[], int b[]);
void division_by2(int num[]);
void multiplication_by2k(int num[], int two_k);
void swap(int a[], int b[]);

int main()
{
    // input integer
    char a_str[MAX_LEN], b_str[MAX_LEN];
    scanf("%s%s", a_str, b_str);

    int a[MAX_LEN] = {0}, b[MAX_LEN] = {0};
    to_int_arr(a_str, a);
    to_int_arr(b_str, b);

    // initialization
    int n[MAX_LEN] = {0}, m[MAX_LEN] = {0}, ans = 1;
    if (comparator(a, b))
    {
        for (int i = 0; i < MAX_LEN; i++)
        {
            n[i] = b[i];
            m[i] = a[i];
        }
    }
    else
    {
        for (int i = 0; i < MAX_LEN; i++)
        {
            n[i] = a[i];
            m[i] = b[i];
        }
    }

    // start algorithm
    while (!is_zero(n) && !is_zero(m))
    {
        // step 1
        if (is_even(n) && is_even(m))
        {
            ans *= 2;
            division_by2(n);
            division_by2(m);
        }
        else if (is_even(n))
        {
            division_by2(n);
        }
        else if (is_even(m))
        {
            division_by2(m);
        }

        // step 2
        if (comparator(n, m))
            swap(n, m);

        // step 3
        subtraction(m, n);
    }

    // output result
    multiplication_by2k(n, ans);
    print_array(n);

    return 0;
}

// swap
void swap(int a[], int b[])
{
    for (int i = 0; i < MAX_LEN; i++)
    {
        int tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

// multiplication by 2k
void multiplication_by2k(int num[], int two_k)
{
    // multiply
    for (int i = 0; i < MAX_LEN; i++)
    {
        num[i] *= two_k;
    }

    // add carry
    for (int i = 0; i < MAX_LEN; i++)
    {
        int j = i;
        while (num[i] >= 10)
        {
            num[j + 1] += num[j] / 10;
            num[j] %= 10;
            j += 1;
        }
    }
}

// division by 2
void division_by2(int num[])
{
    int quotient = 0, mod = 0;
    for (int i = MAX_LEN - 1; i >= 0; i--)
    {
        quotient = (mod * 10 + num[i]) / 2;
        mod = (mod * 10 + num[i]) % 2;
        num[i] = quotient;
    }
}

// subtraction
void subtraction(int a[], int b[])
{
    for (int i = 0; i < MAX_LEN; i++)
    {
        while (a[i] - b[i] < 0)
        {
            a[i] += 10;
            a[i + 1] -= 1;
        }

        a[i] -= b[i];
    }
}

// is even
bool is_even(int num[])
{
    return (num[0] % 2 == 0) ? true : false;
}

// is zero
bool is_zero(int num[])
{
    for (int i = 0; i < MAX_LEN; i++)
    {
        if (num[i] != 0)
            return false;
    }
    return true;
}

// comparator
bool comparator(int a[], int b[])
{
    for (int i = MAX_LEN - 1; i >= 0; i--)
    {
        if (a[i] > b[i])
            return true;
        else if (a[i] < b[i])
            return false;
        else
            continue;
    }
}

// to int array
void to_int_arr(char* num_str, int num[])
{
    for (int i = strlen(num_str) - 1, j = 0; i >= 0; i--, j++)
        num[j] = (int)(num_str[i] - '0');
}

// print array
void print_array(int arr[])
{
    bool first_none_zero = 0;
    for (int i = MAX_LEN - 1; i >= 0; i--)
    {
        if (first_none_zero == 0)
        {
            if (arr[i] != 0)
                first_none_zero = 1;
        }

        if (first_none_zero == 1)
            printf("%d", arr[i]);
    }
    printf("\n");
}