#include <stdio.h>

int main(void) {
    const char *path = "output.txt";
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    const char *msg = "hello, file\n";
    size_t len = 12; // strlen("hello, file\n")
    size_t nw = fwrite(msg, 1, len, fp);
    if (nw != len) {
        perror("fwrite");
        fclose(fp);
        return 1;
    }

    if (fclose(fp) != 0) {
        perror("fclose");
        return 1;
    }
    return 0;
}

