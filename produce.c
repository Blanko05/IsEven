#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

static const char *ones[] = {
    "", "one", "two", "three", "four", "five", "six", "seven", "eight",
    "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
    "sixteen", "seventeen", "eighteen", "nineteen"};

static const char *tens[] = {
    "", "", "twenty", "thirty", "forty", "fifty",
    "sixty", "seventy", "eighty", "ninety"};

static const char *thousands[] = {
    "", "thousand", "million", "billion", "trillion"};

/* Convert string to UPPERCASE in-place */
static void to_upper(char *s)
{
    for (; *s; ++s)
        *s = (char)toupper((unsigned char)*s);
}

/* Title-case: capitalise the first letter of every word */
static void to_title(char *s)
{
    int new_word = 1;
    for (; *s; ++s)
    {
        if (*s == ' ' || *s == '-')
        {
            new_word = 1;
        }
        else if (new_word)
        {
            *s = (char)toupper((unsigned char)*s);
            new_word = 0;
        }
    }
}
/* Convert 0-999 into words, appended to *buf.
   Returns pointer to the NUL terminator of buf. */
static char *hundreds_to_words(int n, char *buf)
{
    if (n == 0)
        return buf;

    if (n >= 100)
    {
        buf += sprintf(buf, "%s hundred", ones[n / 100]);
        n %= 100;
        if (n)
            buf += sprintf(buf, " ");
    }

    if (n >= 20)
    {
        buf += sprintf(buf, "%s", tens[n / 10]);
        if (n % 10)
            buf += sprintf(buf, "-%s", ones[n % 10]);
    }
    else if (n > 0)
    {
        buf += sprintf(buf, "%s", ones[n]);
    }

    return buf;
}
char *number_to_words(long long n, char *out)
{
    if (n == 0)
    {
        strcpy(out, "zero");
        return out;
    }

    char *p = out;
    *p = '\0';

    int negative = 0;
    if (n < 0)
    {
        negative = 1;
        n = -n;
    }

    /* Split into groups of three digits (up to trillions) */
    int groups[5];
    int num_groups = 0;
    long long tmp = n;
    while (tmp > 0)
    {
        groups[num_groups++] = (int)(tmp % 1000);
        tmp /= 1000;
    }

    if (negative)
        p += sprintf(p, "negative ");

    int first = 1;
    for (int i = num_groups - 1; i >= 0; --i)
    {
        if (groups[i] == 0)
            continue;
        if (!first)
            p += sprintf(p, " ");
        p = hundreds_to_words(groups[i], p);
        if (thousands[i][0])
            p += sprintf(p, " %s", thousands[i]);
        first = 0;
    }

    return out;
}

int main()
{
    FILE *f = fopen("test.txt", "w");
    if (f == NULL)
    {
        perror("Failed to open file");
        return 1;
    }
    int x;
    printf("Enter a number: ");
    scanf("%d", &x);

    fprintf(f,
            "#include <stdio.h>\n#include <stdbool.h>\n#include <string.h>\n\n"
            "bool isEven(long long number, const char *str);\n\n"
            "int main() {\n"
            "  long long n;\n"
            "  char buf[32];\n"
            "  printf(\"Enter a number (0-%d): \");\n"
            "  scanf(\"%%lld\", &n);\n"
            "  snprintf(buf, sizeof(buf), \"%%lld\", n);\n"
            "  printf(\"%%lld is %%s\\n\", n, isEven(n, buf) ? \"even\" : \"odd\");\n"
            "  return 0;\n"
            "}\n",
            x);

    fprintf(f, "bool isEven(long long number, const char *str) {\n\n");

    for (int i = 0; i <= x; i++)
    {
        char lower[512];
        char title[512];
        char upper[512];

        number_to_words(i, lower);
        strcpy(title, lower);
        strcpy(upper, lower);
        to_title(title);
        to_upper(upper);

        fprintf(f, "if (number == %d || strcmp(str, \"%s\") == 0 || strcmp(str, \"%s\") == 0 || strcmp(str, \"%s\") == 0) ", i, lower, title, upper);
        fprintf(f, "return %s;\n", (i % 2 == 0) ? "true" : "false");
    }
    fprintf(f, "\nreturn false;\n}");

    fclose(f);

    system("copy test.txt IsEven.c");

    system("del test.txt");
    system("gcc IsEven.c -o IsEven.exe");
    system("IsEven.exe");

    return 0;
}