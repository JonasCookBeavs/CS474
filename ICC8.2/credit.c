#include <stdio.h>

unsigned int digits_sum(unsigned int v)
{
    unsigned int total = 0;

    while (v)
    {
        total += v % 10;
        v /= 10;
    }

    return total;
}

/*
 * Given an account number with no check digit, compute and return the
 * check digit.
 */
unsigned int luhn_compute_check_digit(unsigned long long v)
{
    int sum = 0;
    for (int i = 0; i < 15; i++)
    {
        int new_digit = v % 10;
        if (i % 2 == 0)
        {
            if (new_digit > 4)
            {
                new_digit = new_digit * 2 - 9;
            }
            else
            {
                new_digit = new_digit * 2;
            }
        }
        sum += new_digit;
        v /= 10;
    }
    return (10 - (sum % 10)) % 10;
}

int luhn_is_valid(unsigned long long v)
{
    unsigned int check_digit, computed_check_digit;

    check_digit = v % 10;
    computed_check_digit = luhn_compute_check_digit(v / 10);

    return check_digit == computed_check_digit;
}

#define TEST_COUNT 8

int main(void)
{
    unsigned long long account_numbers[TEST_COUNT] = {
        4539908860705595, // valid
        4916056182581170, // valid
        5137532317317391, // valid
        5456126701113392, // valid
        4539918860705595, // invalid
        4916056187581170, // invalid
        5137532317318391, // invalid
        5456126701113395  // invalid
    };

    for (int i = 0; i < TEST_COUNT; i++)
        printf("Card number %lld: %s\n", account_numbers[i],
               luhn_is_valid(account_numbers[i]) ? "valid" : "invalid");
}