#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

char* sites_src_file = NULL;
char* units_src_file = NULL;
FILE* sfp = NULL;
FILE* ufp = NULL;

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

char* get_scheme_and_signature(char* x)
{
    char *u, *t, *s;

    u = strchr(x, '\"');
    assert(u);
    t = strchr(u + 1, '\\');
    assert(t);
    *t = '\0';
    s = strchr(t + 2, '\"');
    assert(s);
    s++;
    t = strchr(s, '\\');
    assert(t);
    *t = '\0';

    *u = scheme_code(s);
    return u;
}

void print_s_site(char* s)
{
    char *x, *y;

    x = strchr(s , '\t'); assert(x); *x = '\0'; x++;
    x = strchr(x , '\t'); assert(x); x++;
    x = strchr(x , '\t'); assert(x); x++;

    y = strchr(x , '\t'); *y = '\0'; y++;

    fprintf(sfp, "{ \"%s\", \"%s\" } ", s, x);
}

void print_b_site(char* s)
{
    char* x = strchr(s, '\n'); assert(x); *x = '\0';
    fprintf(sfp, "{ \"%s\" } ", s);
}

void print_r_site(char* s)
{
    char* x = strchr(s, '\n'); assert(x); *x = '\0';
    fprintf(sfp, "{ \"%s\" } ", s);
}

void print_site(char site_kind, char* s)
{
    char *x, *y, *z;

    // s points to file name

    x = strchr(s, '\t'); assert(x); *x = '\0'; x++;

    // x points to line number

    y = strchr(x, '\t'); assert(y); *y = '\0'; y++;

    // y points to func name

    z = strchr(y, '\t'); assert(z); *z = '\0'; z++;

    // z points to CFG node

    fprintf(sfp, "\t{ \"%s\", %s, \"%s\", ", s, x, y);

    // ignore CFG node
    z = strchr(z, '\t'); assert(z); z++;

    // z points to scheme-specific predicate name

    switch (site_kind) {
    case 'S': print_s_site(z); break;
    case 'B': print_b_site(z); break;
    case 'R': print_r_site(z); break;
    default: assert(0);
    }

    fprintf(sfp, "},\n");
}

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-ss")) {
	    i++;
	    sites_src_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-us")) {
	    i++;
	    units_src_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: map-sites <options> < <sites-txt-file>\n"
                 "(w) -ss <sites-src-file>\n"
                 "(w) -us <units-src-file>\n"
            );
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (!sites_src_file || !units_src_file) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    int num_units = 0, num_b_preds = 0, num_r_preds = 0, num_s_preds = 0;

    sfp = fopen(sites_src_file, "w"); assert(sfp);
    ufp = fopen(units_src_file, "w"); assert(ufp);

    fputs("#include <sites.h>\n\n"
          "const struct site_t sites[] = {\n",
          sfp);

    fputs("#include <units.h>\n\n"
          "const struct unit_t units[] = {\n",
          ufp);

    while (1) {
        char s[3000];
        fgets(s, 3000, stdin);
        if (feof(stdin))
            break;
        assert(!strncmp(s, "<sites", 6));
        char* t = get_scheme_and_signature(s);
        int count = 0;
        while (1) {
            char p[3000];
            fgets(p, 3000, stdin);
            if (!strncmp(p, "</sites", 7))
                break;
            print_site(t[0], p);
            count++;
        }
        fprintf(ufp, "\t{ \"%s\", %d },\n", t, count);
        num_units++;
        switch (t[0]) {
        case 'S': num_s_preds += 6 * count; break;
        case 'R': num_r_preds += 6 * count; break;
        case 'B': num_b_preds += 2 * count; break;
        default: assert(0);
        }
    }

    fputs("};\n\n", sfp);
    fputs("};\n\n", ufp);
    fprintf(ufp, "const int NumUnits  = %d;\n", num_units);
    fprintf(ufp, "const int NumBPreds = %d;\n", num_b_preds);
    fprintf(ufp, "const int NumRPreds = %d;\n", num_r_preds);
    fprintf(ufp, "const int NumSPreds = %d;\n", num_s_preds);

    fclose(sfp);
    fclose(ufp);
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
