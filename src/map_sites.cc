// Usage: map_sites < <sites_file>
// Creates files SITES_TXT_FILE and UNITS_HDR_FILE

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "def.h"

inline char scheme_code(char* s)
{
    if (!strcmp(s, "scalar-pairs"))
        return 'S';
    if (!strcmp(s, "branches"))
        return 'B';
    if (!strcmp(s, "returns"))
        return 'R';
    assert(0);
}

char* get_scheme_and_unit(char* x)
{
    char *u, *t, *s;

    u = strchr(x, '\"');
    t = strchr(u + 1, '\"');
    *t = '\0';
    s = strchr(t + 1, '\"');
    s++;
    t = strchr(s, '\"');
    *t = '\0';

    *u = scheme_code(s);
    return u;
}

char* del_cfg(char* s)
{
    char *x;
    x = strchr(s, '\t'); x++;
    x = strchr(x, '\t'); x++;
    x = strchr(x, '\t'); x++;
    char* y;
    y = strchr(x , '\t'); y++;
    while (*x++ = *y++)
        ;
    return s;
} 

char* print_s_site(char* s)
{
    char *t = del_cfg(s);
    char* x;
    x = strchr(t , '\t'); x++;
    x = strchr(x , '\t'); x++;
    x = strchr(x , '\t'); x++;
    x = strchr(x , '\t'); 
    *x = ' '; x++; *x = '$'; x++; *x = ' '; x++;

    char* y;
    y = strchr(x, '\t'); y++;
    y = strchr(y, '\t'); y++;

    while (*y != '\t') *x++ = *y++;
    *x = '\n'; x++; *x = '\0';
    return t;
} 

char* print_site(char site_kind, char* s)
{
    switch (site_kind) {
    case 'S': return print_s_site(s);
    case 'B': return del_cfg(s);
    case 'R': return del_cfg(s);
    default: assert(0);
    }
}

main()
{
    FILE* sites_fp = fopen(SITES_TXT_FILE, "w"); assert(sites_fp);
    FILE* units_fp = fopen(UNITS_HDR_FILE, "w"); assert(units_fp);
    int num_units = 0, num_b_preds = 0, num_r_preds = 0, num_s_preds = 0;

    fprintf(units_fp, "#ifndef UNITS_H\n#define UNITS_H\n\n");
    fprintf(units_fp, "const struct { char* s; int c; } units[] = {\n");

    while (1) {
        char s[3000];
        fgets(s, 3000, stdin);
        if (feof(stdin))
            break;
        assert(!strncmp(s, "<sites", 6));
        char* t = get_scheme_and_unit(s);
        int count = 0;
        while (1) {
            char p[3000];
            fgets(p, 3000, stdin);
            if (!strncmp(p, "</sites", 7))
                break;
            fprintf(sites_fp, "%s", print_site(t[0], p));
            count++;
        }
        fprintf(units_fp, "\t{ \"%s\", %d },\n", t, count);
        num_units++;
        switch (t[0]) {
        case 'S': num_s_preds += 6 * count; break;
        case 'R': num_r_preds += 6 * count; break;
        case 'B': num_b_preds += 2 * count; break;
        default: assert(0);
        }
    }

    fprintf(units_fp, "};\n\n");
    fprintf(units_fp, "#define NUM_UNITS   %d\n", num_units);
    fprintf(units_fp, "#define NUM_B_PREDS %d\n", num_b_preds);
    fprintf(units_fp, "#define NUM_R_PREDS %d\n", num_r_preds);
    fprintf(units_fp, "#define NUM_S_PREDS %d\n", num_s_preds);
    fprintf(units_fp, "\n#endif\n");

    fclose(sites_fp);
    fclose(units_fp);

    return 0;
}
