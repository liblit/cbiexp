#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>

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
    while ((*x++ = *y++))
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

char* compact_sites_file = NULL;
char* units_src_file = NULL;

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-u")) {
	    i++;
	    units_src_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-cs")) {
	    i++;
	    compact_sites_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: map-sites -cs <compact-sites-file> -u <units-src-file> < <verbose-sites-file>\n"
		 "Reads  <verbose-sites-file>\n"
		 "Writes <compact-sites-file> and <units-src-file>");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (!compact_sites_file || !units_src_file) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    FILE* cfp = fopen(compact_sites_file, "w"); assert(cfp);
    FILE* ufp = fopen(units_src_file, "w"); assert(ufp);
    int num_units = 0, num_b_preds = 0, num_r_preds = 0, num_s_preds = 0;

    fputs("#include <units.h>\n\n"
	  "const struct Unit units[] = {\n",
	  ufp);

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
	    fputs(print_site(t[0], p), cfp);
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

    fputs("};\n\n", ufp);
    fprintf(ufp, "const int NumUnits  = %d;\n", num_units);
    fprintf(ufp, "const int NumBPreds = %d;\n", num_b_preds);
    fprintf(ufp, "const int NumRPreds = %d;\n", num_r_preds);
    fprintf(ufp, "const int NumSPreds = %d;\n", num_s_preds);

    fclose(cfp);
    fclose(ufp);

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
