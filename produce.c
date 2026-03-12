#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <curl/curl.h>

#define OPENAI_API_KEY "YOUR-KEY-HERE"

typedef struct
{
    char *data;
    size_t size;
} CurlResponse;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t total = size * nmemb;
    CurlResponse *r = (CurlResponse *)userp;
    r->data = realloc(r->data, r->size + total + 1);
    memcpy(r->data + r->size, contents, total);
    r->size += total;
    r->data[r->size] = '\0';
    return total;
}

static int ask_ai(long long n)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return -1;

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CurlResponse response = {.data = malloc(1), .size = 0};

    char body[256];
    snprintf(body, sizeof(body),
             "{\"model\":\"gpt-4o-mini\",\"messages\":"
             "[{\"role\":\"user\",\"content\":\"Is %lld even? Reply with only the word true or false.\"}],"
             "\"max_tokens\":5}",
             n);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Authorization: Bearer " OPENAI_API_KEY);

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        free(response.data);
        return -1;
    }

    int result = -1;
    char *content = strstr(response.data, "\"content\":");
    if (content)
    {
        content += 10;
        while (*content == ' ' || *content == '"')
            content++;
        if (_strnicmp(content, "true", 4) == 0)
            result = 1;
        else if (_strnicmp(content, "false", 5) == 0)
            result = 0;
    }

    free(response.data);
    return result;
}

typedef struct
{
    const char *exe;
    long long n;
    bool result;
    char raw[256];
} ThreadArg;

static DWORD WINAPI run_checker(LPVOID param)
{
    ThreadArg *a = (ThreadArg *)param;

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "%s.exe %lld", a->exe, a->n);

    FILE *pipe = _popen(cmd, "r");
    if (!pipe)
    {
        fprintf(stderr, "popen failed for %s\n", a->exe);
        return 1;
    }

    fgets(a->raw, sizeof(a->raw), pipe);
    _pclose(pipe);

    a->raw[strcspn(a->raw, "\r\n")] = '\0';
    a->result = (strcmp(a->raw, "true") == 0);
    return 0;
}

static const char *ones[] = {
    "", "one", "two", "three", "four", "five", "six", "seven", "eight",
    "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
    "sixteen", "seventeen", "eighteen", "nineteen"};

static const char *tens[] = {
    "", "", "twenty", "thirty", "forty", "fifty",
    "sixty", "seventy", "eighty", "ninety"};

static const char *thousands[] = {
    "", "thousand", "million", "billion", "trillion"};

static void to_upper(char *s)
{
    for (; *s; ++s)
        *s = (char)toupper((unsigned char)*s);
}

static void to_title(char *s)
{
    int new_word = 1;
    for (; *s; ++s)
    {
        if (*s == ' ' || *s == '-')
            new_word = 1;
        else if (new_word)
        {
            *s = (char)toupper((unsigned char)*s);
            new_word = 0;
        }
    }
}

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
        buf += sprintf(buf, "%s", ones[n]);
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

    /* Generate IsEven.c */
    fprintf(f,
            "#include <stdio.h>\n#include <stdbool.h>\n#include <string.h>\n#include <stdlib.h>\n\n"
            "bool isEven(long long number, const char *str);\n\n"
            "int main(int argc, char *argv[]) {\n"
            "  if (argc < 2) { printf(\"usage: %%s <number>\\n\", argv[0]); return 1; }\n"
            "  long long n = atoll(argv[1]);\n"
            "  char buf[32];\n"
            "  snprintf(buf, sizeof(buf), \"%%lld\", n);\n"
            "  printf(\"%%s\\n\", isEven(n, buf) ? \"true\" : \"false\");\n"
            "  return 0;\n"
            "}\n");
    fprintf(f, "bool isEven(long long number, const char *str) {\n\n");
    for (int i = 0; i <= x; i++)
    {
        char lower[512], title[512], upper[512];
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
    system("copy test.txt IsEven.c > nul");
    system("del test.txt");
    system("gcc IsEven.c -o IsEven.exe");

    /* Generate IsOdd.c */
    f = fopen("test.txt", "w");
    if (f == NULL)
    {
        perror("Failed to open file");
        return 1;
    }
    fprintf(f,
            "#include <stdio.h>\n#include <stdbool.h>\n#include <string.h>\n#include <stdlib.h>\n\n"
            "bool isOdd(long long number, const char *str);\n\n"
            "int main(int argc, char *argv[]) {\n"
            "  if (argc < 2) { printf(\"usage: %%s <number>\\n\", argv[0]); return 1; }\n"
            "  long long n = atoll(argv[1]);\n"
            "  char buf[32];\n"
            "  snprintf(buf, sizeof(buf), \"%%lld\", n);\n"
            "  printf(\"%%s\\n\", isOdd(n, buf) ? \"true\" : \"false\");\n"
            "  return 0;\n"
            "}\n");
    fprintf(f, "bool isOdd(long long number, const char *str) {\n\n");
    for (int i = 0; i <= x; i++)
    {
        char lower[512], title[512], upper[512];
        number_to_words(i, lower);
        strcpy(title, lower);
        strcpy(upper, lower);
        to_title(title);
        to_upper(upper);
        fprintf(f, "if (number == %d || strcmp(str, \"%s\") == 0 || strcmp(str, \"%s\") == 0 || strcmp(str, \"%s\") == 0) ", i, lower, title, upper);
        fprintf(f, "return %s;\n", (i % 2 == 1) ? "true" : "false");
    }
    fprintf(f, "\nreturn false;\n}");
    fclose(f);
    system("copy test.txt IsOdd.c > nul");
    system("del test.txt");
    system("gcc IsOdd.c -o IsOdd.exe");

    /* Run both checkers in parallel */
    long long n = (long long)x;

    ThreadArg even_arg = {.exe = "IsEven", .n = n};
    ThreadArg odd_arg = {.exe = "IsOdd", .n = n};

    printf("\n[*] Spawning threads...\n");

    HANDLE threads[2];
    threads[0] = CreateThread(NULL, 0, run_checker, &even_arg, 0, NULL);
    threads[1] = CreateThread(NULL, 0, run_checker, &odd_arg, 0, NULL);

    if (!threads[0] || !threads[1])
    {
        fprintf(stderr, "Failed to create threads.\n");
        return 1;
    }

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);
    CloseHandle(threads[0]);
    CloseHandle(threads[1]);

    printf("\n[IsEven.exe] says: %s\n", even_arg.raw);
    printf("[IsOdd.exe]  says: %s\n", odd_arg.raw);

    /* Ask AI */
    printf("[OpenAI]     says: ");
    int ai_result = ask_ai(n);
    if (ai_result == 1)
        printf("true\n");
    else if (ai_result == 0)
        printf("false\n");
    else
        printf("error\n");

    /* Final verdict — all three must agree */
    printf("\n");
    if (even_arg.result && !odd_arg.result && ai_result == 1)
    {
        printf("FINAL VERDICT: %d ", (int)n);
        printf("is EVEN. (All three agree.)\n");
    }
    else if (!even_arg.result && odd_arg.result && ai_result == 0)
    {
        printf("FINAL VERDICT: %d ", (int)n);
        printf("is ODD. (All three agree.)\n");
    }
    else if (ai_result == -1)
    {
        printf("FINAL VERDICT: AI check failed.");
        printf(" Falling back to thread consensus.\n");
        if (even_arg.result && !odd_arg.result)
            printf("VERDICT: %d is EVEN.\n", (int)n);
        else if (!even_arg.result && odd_arg.result)
            printf("VERDICT: %d is ODD.\n", (int)n);
        else
            printf("VERDICT: complete chaos.\n");
    }
    else
    {
        printf("FINAL VERDICT: sources DISAGREE.");
        printf(" Reality may be broken.\n");
    }

    return 0;
}