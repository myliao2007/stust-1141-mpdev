#include <stdio.h>

int main(int argc, char **argv) {
    const char *path = (argc > 1) ? argv[1] : "input.txt";
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    unsigned char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (fwrite(buf, 1, n, stdout) != n) {
            perror("fwrite");
            fclose(fp);
            return 1;
        }
    }
    if (ferror(fp)) {
        perror("fread");
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

