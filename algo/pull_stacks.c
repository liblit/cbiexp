#include <stdio.h>
#include <string.h>
#include <assert.h>

main(int argc, char** argv)
{
    char s[20000];

    assert(argc == 3);
    FILE* ifp = fopen(argv[1], "r");
    assert(ifp);
    FILE* ofp = fopen(argv[2], "w");
    assert(ofp);

    while (1) {
        fscanf(ifp, "%[^\n]s", s);
        fgetc(ifp);  // eat '\n'
        if (feof(ifp))
            break;
        if (strcmp(s, "<report id=\"main-stack-trace\">") == 0) {
            fprintf(ofp, "%s\n", s);
            while (1) {
                int i = fscanf(ifp, "%[^\n]s", s);
                fgetc(ifp);
                if (feof(ifp))
                    break;
                if (i)
                    fprintf(ofp, "%s\n", s);
                else
                    fprintf(ofp, "\n");
            }
        }
    }

    fclose(ifp);
    fclose(ofp);
    return 0;
}

