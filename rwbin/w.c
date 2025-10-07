#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Record {
    int id;
    int course;
};

int main(int argc, char *argv[]) {
    struct Record r = {0};

    if(argc < 2) { return 1; }

    r.id = atoi(argv[1]);
    r.course = 1141;

    fILE *fp = fopen("1.bin", "wb");
    if (!fp) {
        perror("fopen");
        return 2;
    }

    size_t n = fwrite(&r, sizeof(r), 1, fp);
    if (n != 1) {
        perror("fwrite");
        fclose(fp);
        return 3;
    }

    fclose(fp);
    return 0;
}

