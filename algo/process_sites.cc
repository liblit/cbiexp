#include <stdio.h>
#include <string.h>
#include <assert.h>

main(int argc, char** argv)
{
    assert(argc == 2);
    FILE* ifp = fopen(argv[1], "r");
    assert(ifp);

    FILE* ofpu = fopen("units.h", "w");
    assert(ofpu);
    fprintf(ofpu, "#ifndef UNITS_H\n#define UNITS_H\n\n");
    fprintf(ofpu, "#include \"def.h\"\n\n");
    fprintf(ofpu, "unit_t units[] = {\n");

    FILE* ofps = fopen("sites.h", "w");
    assert(ofps);
    fprintf(ofps, "#ifndef SITES_H\n#define SITES_H\n\n");
    fprintf(ofps, "char* sites[] = {\n");

    while (1) {
        char s[1000], *unit, *scheme, *t;

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

        int count = 0;
        while (1) {
            char p[1000];
            fscanf(ifp, "%[^\n]s", p);
            fgetc(ifp);  // eat '\n'
            if (strncmp(p, "</sites", 7) == 0)
                break;
            fprintf(ofps, "\t\"%s\",\n", p);
            count++;
        }

        fprintf(ofpu, "\t{ \"");
        if (strcmp(scheme, "scalar-pairs") == 0)
            fprintf(ofpu, "S");
        else if (strcmp(scheme, "branches") == 0)
            fprintf(ofpu, "B");
        else if (strcmp(scheme, "returns") == 0)
            fprintf(ofpu, "R");
        else
            assert(0);
        fprintf(ofpu, "%s\", %d },\n", unit, count);
    }

    fprintf(ofpu, "};\n\n#endif\n");
    fclose(ofpu);
    fprintf(ofps, "};\n\n#endif\n");
    fclose(ofps);

    return 0;
}

