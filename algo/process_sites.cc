#include <stdio.h>
#include <string.h>
#include <assert.h>

main(int argc, char** argv)
{
    char s[1000], *unit, *scheme, *t;
    char p[1000];

    assert(argc == 2);
    FILE* fp = fopen(argv[1], "r");
    assert(fp);

    while (1) {
        fscanf(fp, "%[^\n]s", s);
        fgetc(fp);  // eat '\n'
        if (feof(fp))
            break;
        assert(strncmp(s, "<sites", 6) == 0);
        unit = strchr(s, '\"'); 
        unit++;
        t = strchr(unit, '\"');
        *t = '\0';
        t++;
        scheme = strchr(t, '\"');
        scheme++;
        t = strchr(scheme, '\"');
        *t = '\0';
        int count = 0;
        while (1) {
            fscanf(fp, "%[^\n]s", p);
            fgetc(fp);  // eat '\n'
            if (strncmp(p, "</sites", 7) == 0)
                break;
            if (strcmp(scheme, "scalar-pairs") == 0)
                printf("S");
            else if (strcmp(scheme, "branches") == 0)
                printf("B");
            else if (strcmp(scheme, "returns") == 0)
                printf("R");
            else
                assert(0);
            printf("%s", unit);
            printf("%05d", count);
            printf("@ %s\n", p);
            count++;
            assert(count < 99999);
        }
    }
    return 0;
}

