#include <stdio.h>
#include <assert.h>

#define B 10
#define R 10
#define S 10

main()
{
    FILE* pfp = fopen("preds.txt", "r");
    assert(pfp);
    FILE* Bfp = fopen("B.html", "r");
    assert(Bfp);
    FILE* Rfp = fopen("R.html", "r");
    assert(Rfp);
    FILE* Sfp = fopen("S.html", "r");
    assert(Sfp);

    int i, u, c, p;
    char s[500000];

    printf("struct { int u, c, p; char* s; } tab[] = {\n");

    for (i = 0; i < B; i++) {
        fscanf(pfp, "%d %d %d", &u, &c, &p);
        fscanf(Bfp, "%[^\n]s", s);
        fgetc(Bfp);
        printf("\t{ %d, %d, %d, \"%s\" },\n", u, c, p, s);
    }
    for (i = 0; i < R; i++) {
        fscanf(pfp, "%d %d %d", &u, &c, &p);
        fscanf(Rfp, "%[^\n]s", s);
        fgetc(Rfp);
        printf("\t{ %d, %d, %d, \"%s\" },\n", u, c, p, s);
    }
    for (i = 0; i < S; i++) {
        fscanf(pfp, "%d %d %d", &u, &c, &p);
        fscanf(Sfp, "%[^\n]s", s);
        fgetc(Sfp);
        printf("\t{ %d, %d, %d, \"%s\" },\n", u, c, p, s);
    }
    printf("};\n");
}

