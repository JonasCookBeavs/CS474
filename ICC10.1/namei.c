#include <stdio.h>
#include <string.h>

/*
 * Accepts an absolute path. Prints `/` followed by each path element in
 * turn.
 */
void loop_path(char *path)
{
    char *token = strtok(path, "/");
    printf("/\n");
    while(token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, "/");
    }
}

#define TEST_PATH_COUNT 4

int main(void)
{
    char paths[TEST_PATH_COUNT][32] = {
        "/",
        "/foo",
        "/foo/bar",
        "/foo/bar/../baz"
    };

    for (int i = 0; i < TEST_PATH_COUNT; i++) {
        printf("%d: %s:\n", i, paths[i]);
        loop_path(paths[i]);
        putchar('\n');
    }
}