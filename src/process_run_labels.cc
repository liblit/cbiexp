#include <cstdio>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <cstdlib>

char* sruns_file = NULL;
char* fruns_file = NULL;
char* label_path_fmt = NULL;
int num_runs = 0;

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-n")) {
	    i++;
	    num_runs = atoi(argv[i]);
	    continue;
	}
	if (!strcmp(argv[i], "-l")) {
	    i++;
	    label_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-s")) {
	    i++;
	    sruns_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-f")) {
	    i++;
	    fruns_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: process-run-labels -n <num-runs> -l <label-path-fmt> -s <sruns-file> -f <fruns-file>\n"
		 "Reads  label file of each run\n"
		 "writes <sruns-file> and <fruns-file>");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (!label_path_fmt || !sruns_file || !fruns_file) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

int main(int argc, char** argv)
{

    process_cmdline(argc, argv);

    FILE* sfp = fopen(sruns_file, "w"); assert(sfp);
    FILE* ffp = fopen(fruns_file, "w"); assert(ffp);

    for (int i = 0; i < num_runs; i++) {
	char filename[100];
	char s[100];
	sprintf(filename, label_path_fmt, i);
	FILE* fp = fopen(filename, "r");
	if (!fp) {
	    fprintf(stderr, "cannot read %s: %s\n", filename, strerror(errno));
	    return 1;
	}
	fscanf(fp, "%s", s);
	fclose(fp);

	if (!strcmp(s, "success"))
	    fprintf(sfp, "%d\n", i);
	else if (!strcmp(s, "failure"))
	    fprintf(ffp, "%d\n", i);
	else if (strcmp(s, "ignore")) {
	    fprintf(stderr, "malformed label in %s: %s\n", filename, s);
	    return 1;
	}
    }

    fclose(sfp);
    fclose(ffp);
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
