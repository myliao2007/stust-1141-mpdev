#include <stdio.h>

int main(int argc, char **argv) {
    const char *path = (argc > 1) ? argv[1] : "input.txt";
    int ret = 0;
    FILE *fp = fopen(path, "rb");
    FILE *fp_o = fopen("output", "wb");
    if (!fp || !fp_o) {
        ret = 1;
        goto out;
    }

    unsigned char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (fwrite(buf, 1, n, fp_o) != n) {
            perror("fwrite");
            ret = 1;
            goto out;
        }
    }
    if (ferror(fp)) {
        perror("fread");
        ret = 1;
        goto out;
    }

out:
    if(fp) fclose(fp);
    if(fp_o) fclose(fp_o);
    return ret;
}

