#include <stdio.h>
#include <assert.h>

/************** EDIT THE FOLLOWING PARAMETERS *******************/

#define SITES_2_FILE "sites2_data9.txt"
#define SITES_3_FILE "sites3_data9.txt"
#define NUM_2_SITES 2146
#define NUM_3_SITES 7896

/****************************************************************/

#define MAX_CHARS 500

FILE *fp, *out_fp;
char func[300], line[300], site[300], u[MAX_CHARS], v[MAX_CHARS];

void print_3_pred(char c)
{
    fprintf(out_fp, "\t\"%s\t%s\t%s\t%s %c %s\",\n",
        func, line, site, u, c, v);
}

void print_2_pred(char c)
{
    fprintf(out_fp, "\t\"%s\t%s\t%s\t%s is %c\",\n", 
        func, line, site, u, c);
}

void read_2_line()
{
	int c, i;
        char dummy[500];

        fscanf(fp, "%[^\t]s", dummy);  // read file name
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", line);   // read line number
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", func);   // read function name
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", site);   // read site index
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\n]s", u);      // read operand
        for (i = 0; i < MAX_CHARS; i++)
            if (u[i] == '\"')
                u[i] = 'Q';
        c = fgetc(fp); assert(c == '\n');
}

void read_3_line()
{
	int c, i;
        char dummy[500];

        fscanf(fp, "%[^\t]s", dummy);    // read file name
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", line);     // read line number
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", func);     // read function name
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", site);     // read site index
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", u);        // read left operand
        c = fgetc(fp); assert(c == '\t');
        for (i = 0; i < MAX_CHARS; i++)
            if (u[i] == '\"')
                u[i] = 'Q';
        fscanf(fp, "%[^\n]s", v);        // read right operand
        c = fgetc(fp); assert(c == '\n');
        for (i = 0; i < MAX_CHARS; i++)
            if (v[i] == '\"')
                v[i] = 'Q';
}

main()
{
    int i;

    out_fp = fopen("preds.h", "w");
    if (!out_fp) {
        printf("Couldn't open file: preds.h\n");
        return 1;
    }
    fprintf(out_fp, "#ifndef PREDS_H\n#define PREDS_H\n\n");
    fprintf(out_fp, "#define NUM_3_SITES %d\n"  , NUM_3_SITES);
    fprintf(out_fp, "#define NUM_2_SITES %d\n\n", NUM_2_SITES);
    fprintf(out_fp, "char* preds[] = {\n");
 
    fp = fopen(SITES_3_FILE, "r");
    if (!fp) {
        printf("Couldn't open file: %s\n", SITES_3_FILE);
        return 1;
    }
    for (i = 0; i < NUM_3_SITES; i++) {
        char func[300], line[300], site[300], u[500], v[500];
        read_3_line ();
        print_3_pred('<');
        print_3_pred('=');
        print_3_pred('>');
    }
    fclose(fp);

    fp = fopen(SITES_2_FILE, "r");
    if (!fp) {
        printf("Couldn't open file: %s\n", SITES_2_FILE);
        return 1;
    }
    for (i = 0; i < NUM_2_SITES; i++) {
        read_2_line ();
        print_2_pred('0');
        print_2_pred('1');
    }
    fclose(fp);

    fprintf(out_fp, "};\n\n#endif\n");

    return 0;
}

