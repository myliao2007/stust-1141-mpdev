#include <stdio.h>
#include <string.h>

struct Record {
    char id[64];
    char course[64];
};

int main(void) {
    struct Record r = {0};

    // 填入資料
    strncpy(r.course, "LinuxProgramming", sizeof(r.course) - 1);
    r.course[sizeof(r.course) - 1] = '\0';
    strncpy(r.id, "STUST-0001", sizeof(r.id) - 1);
    r.id[sizeof(r.id) - 1] = '\0';

    // 以二進位寫入
    FILE *fp = fopen("1.bin", "wb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    size_t n = fwrite(&r, sizeof(r), 1, fp);
    if (n != 1) {
        perror("fwrite");
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

