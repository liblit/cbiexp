#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

typedef std::vector<const char *> Fields;

char* sites_src_file = NULL;
char* units_src_file = NULL;
FILE* sfp = NULL;
FILE* ufp = NULL;

static void get_fields(Fields &fields, char *row)
{
    char *tab;
    while ((tab = strchr(row, '\t'))) {
	*tab = '\0';
	fields.push_back(row);
	row = tab + 1;
    }

    tab = strchr(row, '\n');
    *tab = '\0';
    fields.push_back(row);
}

char get_scheme_code(char* scheme_str)
{
    if (!strcmp(scheme_str, "scalar-pairs"  )) return 'S';
    if (!strcmp(scheme_str, "branches"      )) return 'B';
    if (!strcmp(scheme_str, "returns"       )) return 'R';
    if (!strcmp(scheme_str, "g-object-unref")) return 'G';
    assert(0);
}

char* get_scheme_code_and_signature(char* x)
{
    char *u, *t, *s;

    u = strchr(x, '\"');
    assert(u);
    t = strchr(u + 1, '\"');
    assert(t);
    *t = '\0';
    s = strchr(t + 1, '\"');
    assert(s);
    s++;
    t = strchr(s, '\"');
    assert(t);
    *t = '\0';

    *u = get_scheme_code(s);
    return u;
}

bool print_s_site(const Fields &fields)
{
    size_t left, right;

    switch (fields.size()) {
    case 9:
	left = 4;
	right = 7;
	break;
    case 6:
	left = 4;
	right = 5;
	break;
    case 5:
	left = 3;
	right = 4;
	break;
    default:
	return false;
    }

    fprintf(sfp, "\'S\', { \"%s\", \"%s\" } ", fields[left], fields[right]);
    return true;
}

bool print_1_site(const Fields &fields, char scheme_code)
{
    size_t operand;

    switch (fields.size()) {
    case 5:
	operand = 4;
	break;
    case 4:
	operand = 3;
	break;
    default:
	return false;
    }

    fprintf(sfp, "\'%c\', { \"%s\" } ", scheme_code, fields[operand]);
    return true;
}

void print_site(char scheme_code, char* s)
{
    Fields fields;
    get_fields(fields, s);

    assert(fields.size() >= 3);
    const char * const file = fields[0];
    const char * const line = fields[1];
    const char * const func = fields[2];
    fprintf(sfp, "\t{ \"%s\", %s, \"%s\", ", file, line, func);

    bool ok;
    switch (scheme_code) {
    case 'S': ok = print_s_site(fields); break;
    case 'B':
    case 'R':
    case 'G': ok = print_1_site(fields, scheme_code); break;
    default: assert(0);
    }

    if (!ok) {
	fprintf(stderr, "unexpected field count (%u) for %c site\n", fields.size(), scheme_code);
	exit(1);
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

    int num_sites = 0, num_units = 0, num_b_preds = 0, num_r_preds = 0, num_s_preds = 0, num_g_preds = 0;

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
        char* t = get_scheme_code_and_signature(s);
        int count = 0;
        while (1) {
            char p[3000];
            fgets(p, 3000, stdin);
            if (!strncmp(p, "</sites", 7))
                break;
            print_site(t[0], p);
            count++;
        }
        fprintf(ufp, "\t{ '%c', \"%s\", %d },\n", t[0], t + 1, count);
        num_sites += count;
        num_units++;

        switch (t[0]) {
        case 'S': num_s_preds += 6 * count; break;
        case 'R': num_r_preds += 6 * count; break;
        case 'B': num_b_preds += 2 * count; break;
        case 'G': num_g_preds += 4 * count; break;
        default: assert(0);
        }
    }

    fputs("};\n\n", sfp);
    fprintf(sfp, "const int num_sites = %d;\n", num_sites);

    fputs("};\n\n", ufp);
    fprintf(ufp, "const int num_units = %d;\n", num_units);
    fprintf(ufp, "const int NumBPreds = %d;\n", num_b_preds);
    fprintf(ufp, "const int NumRPreds = %d;\n", num_r_preds);
    fprintf(ufp, "const int NumSPreds = %d;\n", num_s_preds);
    fprintf(ufp, "const int NumGPreds = %d;\n", num_g_preds);

    fclose(sfp);
    fclose(ufp);
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
