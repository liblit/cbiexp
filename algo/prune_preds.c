#include <stdio.h>
#include <assert.h>
#include <math.h>

main(int argc, char** argv)
{
    char str[3000];
    float in, cr, co;
    int s, f, os, of;

    assert(argc == 2);

    FILE* fp = fopen(argv[1], "r");
    assert(fp);
    while (1) {
        fscanf(fp, "%[^\n]s", str);
        fgetc(fp);
        if (feof(fp))
            break;
        sscanf(str, "%f In, %f Cr, %f Co, %d S, %d F, %d OS, %d OF,", &in, &cr, &co, &s, &f, &os, &of);
        if (in - 1.96 * sqrt(((cr * (1 - cr)) / (f + s)) + ((co * (1 - co))/(of + os))) > 0 && s + f >= 10) {
            printf(str);
            printf("\n");
        }
    }
    fclose(fp);
}
//printf("In = %f Cr = %f Co = %f S = %d F = %d OS = %d OF = %d\n", in, cr, co, s, f, os, of);

