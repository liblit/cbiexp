#include <stdio.h>
#include <string.h>
#include <assert.h>

main(int argc, char** argv)
{
    char s[1000], *unit, *scheme, *t;
    char p[1000];

    assert(argc == 2);
    FILE* ifp = fopen(argv[1], "r");
    assert(ifp);

    FILE* ofpu = fopen("units.h", "w");
    assert(ofpu);
    fprintf(ofpu, "#ifndef UNITS_H\n#define UNITS_H\n\n");
    fprintf(ofpu, "char* units[] = {\n");

    FILE* ofps = fopen("sites.h", "w");
    assert(ofps);
    fprintf(ofps, "#ifndef SITES_H\n#define SITES_H\n\n");
    fprintf(ofps, "char* sites[] = {\n");

    while (1) {
        fscanf(ifp, "%[^\n]s", s);
        fgetc(ifp);  // eat '\n'
        if (feof(ifp))
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

        if (strcmp(scheme, "scalar-pairs") == 0)
            fprintf(ofpu, "\t\"S");
        else if (strcmp(scheme, "branches") == 0)
            fprintf(ofpu, "\t\"B");
        else if (strcmp(scheme, "returns") == 0)
            fprintf(ofpu, "\t\"R");
        else
            assert(0);
        fprintf(ofpu, "%s\",\n", unit);

        while (1) {
            fscanf(ifp, "%[^\n]s", p);
            fgetc(ifp);  // eat '\n'
            if (strncmp(p, "</sites", 7) == 0)
                break;
            fprintf(ofps, "\t\"%s\",\n", p);
        }
    }

    fprintf(ofpu, "};\n\n#endif");
    fclose(ofpu);
    fprintf(ofps, "};\n\n#endif");
    fclose(ofps);

    return 0;
}

