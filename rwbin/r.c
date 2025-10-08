#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Record {
    int id;
    int course;
};

int main(int argc, char *argv[]) {
    struct Record r = {0};
    int id = 0, course =0;

    FILE *fp = fopen("1.bin", "rb");
    if (!fp) {
        perror("fopen");
        return 2;
    }

    size_t n = fread(&r, sizeof(r), 1, fp);
    if (n != 1) {
        perror("read");
        fclose(fp);
        return 3;
    }

    printf("id: %d, course: %d\n", id, course);

    fclose(fp);
    return 0;
}

