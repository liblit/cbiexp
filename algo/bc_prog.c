#include <stdio.h>
#include <assert.h>

#define R 15

main()
{
    FILE* pfp = fopen("P", "r");
    assert(pfp);
    FILE* Rfp = fopen("R", "r");
    assert(Rfp);

    int i, site, p;
    char s[500000];

    printf("struct { int site, p; char* s; } tab[] = {\n");

    for (i = 0; i < R; i++) {
        fscanf(pfp, "%d %d", &site, &p);
        fscanf(Rfp, "%[^\n]s", s);
        fgetc(Rfp);
        printf("\t{ %d, %d, \"%s\" },\n", site, p, s);
    }
    printf("};\n");
}

