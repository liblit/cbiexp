#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "def.h"

main()
{
    FILE* sites_ifp = fopen(I_SITES_FILE, "r"); assert(sites_ifp);
    FILE* sites_ofp = fopen(O_SITES_FILE, "w"); assert(sites_ofp);
    FILE* units_ofp = fopen(O_UNITS_FILE, "w"); assert(units_ofp);
    int num_units = 0, num_sites = 0;

    fprintf(units_ofp, "#ifndef UNITS_H\n#define UNITS_H\n\n");
    fprintf(units_ofp, "#include \"def.h\"\n\n");
    fprintf(units_ofp, "unit_t units[] = {\n");

    while (1) {
        char s[3000], *unit, *scheme, *t;

        fgets(s, 3000, sites_ifp);
        if (feof(sites_ifp))
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
            char p[3000];
            fgets(p, 3000, sites_ifp);
            if (strncmp(p, "</sites", 7) == 0)
                break;
            fprintf(sites_ofp, "%s", p);
            count++;
        }

        fprintf(units_ofp, "\t{ \"");
        if      (strcmp(scheme, "scalar-pairs") == 0)
            fprintf(units_ofp, "S");
        else if (strcmp(scheme, "branches") == 0)
            fprintf(units_ofp, "B");
        else if (strcmp(scheme, "returns") == 0)
            fprintf(units_ofp, "R");
        else if (strcmp(scheme, "g-object-unref") == 0)
            fprintf(units_ofp, "U");
        else
            assert(0);
        fprintf(units_ofp, "%s\", %d },\n", unit, count);

        num_units++;
        num_sites += count;
    }

    fprintf(units_ofp, "};\n\n");
    fprintf(units_ofp, "#define NUM_UNITS %d\n", num_units);
    fprintf(units_ofp, "#define NUM_SITES %d\n", num_sites);
    fprintf(units_ofp, "\n#endif\n");

    fclose(units_ofp);
    fclose(sites_ofp);

    return 0;
}

