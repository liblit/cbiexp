#include <stdio.h>
#include <string.h>
#include <assert.h>

//#define SITES_FILE "/moa/sc3/cbi/rhythmbox-get-uri-fix/share/rhythmbox.sites"
//#define SITES_FILE "/moa/sc4/cbi/data11/share/mossbad.sites"
//#define SITES_FILE "/moa/sc2/cbi/rhythmbox/share/rhythmbox.sites"
//#define SITES_FILE "/moa/sc2/cbi/data10/share/mossbad.sites"
#define SITES_FILE "/moa/sc4/cbi/data11/share/mossbad.sites"

main()
{
    FILE* ifp = fopen(SITES_FILE, "r");
    assert(ifp);

    FILE* ofpu = fopen("units.h", "w");
    assert(ofpu);
    fprintf(ofpu, "#ifndef UNITS_H\n#define UNITS_H\n\n");
    fprintf(ofpu, "#include \"def.h\"\n\n");
    fprintf(ofpu, "unit_t units[] = {\n");

    FILE* ofps = fopen("sites.txt", "w");
    assert(ofps);

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
            fprintf(ofps, "%s\n", p);
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
    fclose(ofps);

    return 0;
}

