/*
    Reference: 無
*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define uint64_t unsigned long long
#define LEN 100001
#define MOD ((1ULL << 40) - 87)

void remove_garble(char D[], char G[]);
void break_string(char D[], bool breaks[]);

int main(void)
{
    int tasks;
    scanf("%d", &tasks);
    while (tasks--)
    {
        // get inputs
        char D[LEN], G[LEN];
        scanf("%s", D);
        scanf("%s", G);

        // remove grable
        remove_garble(D, G);
        
        // break to blocks
        bool breaks[LEN] = {0};
        break_string(D, breaks);

        // print result
        for (int i = 0; D[i] != '\0'; i++)
        {
            printf("%c", D[i]);
            if (breaks[i])
                printf("|");
        }
        printf("\n");
    }
    return 0;
}

void remove_garble(char D[], char G[])
{
    // read G and record occurence
    int n_demands = 0;
    int demand[128] = {0};
    for (int i = 0; G[i] != '\0'; i++)
    {
        n_demands++;
        demand[G[i]]++;
    }
    
    // search garble text
    int n_supplies = 0;
    int supply[128] = {0};

    int end = 0, start = 0;
    int min_end = -1, min_start = -1, min_size = LEN + 1;
    for (end = end; D[end] != '\0'; end++)
    {
        if (demand[D[end]] == 0)
            continue;
        if (++supply[D[end]] > demand[D[end]])
            continue;
        if (++n_supplies < n_demands)
            continue;
        while (demand[D[start]] == 0 || supply[D[start]] > demand[D[start]])
        {
            if (supply[D[start]] > demand[D[start]])
                supply[D[start]]--;
            start++;
        }

        if (end - start + 1 < min_size)
        {
            min_size = end - start + 1;
            min_end = end;
            min_start = start;
        }

        // set new start
        supply[D[start]]--;
        n_supplies--;
        start++;
    }

    // return clean text
    if (min_end >= 0 && min_start >= 0)
    {
        memmove(&(D[min_start]), &(D[min_end+1]), strlen(&(D[min_end+1])) + 1);
    }
}

void break_string(char D[], bool breaks[])
{
    int len = strlen(D);
    if (len <= 1)
        return ;

    int start = 0;
    uint64_t hash_forward = 0, hash_backward = 0, base = 1;
    for (int i = 0, j = len - 1; i <= j; i++, j--)
    {
        // calculate hash value
        hash_forward = (hash_forward*128 + D[i]) % MOD;
        hash_backward = (hash_backward + base*D[j]) % MOD;
        base = (base*128) % MOD;

        // if hash value is equal, check every character
        if (hash_forward != hash_backward)
            continue;

        bool match = true;
        for (int l = start, r = j; l <= i; l++, r++)
        {
            if (D[l] != D[r])
            {
                match = false;
                break;
            }
        }

        if (match)
        {
            breaks[i] = true;
            breaks[j-1] = true;
            hash_forward = 0;
            hash_backward = 0;
            base = 1;
            start = i + 1;
        }
    }
}