#include <stdio.h>
#include <string.h>
#include <assert.h>

FILE *i_obs_fp, *i_tru_fp, *o_obs_fp, *o_tru_fp;
char str[300];

void find_and_print()
{
    char t[5000000];
    while (1) {
        fscanf(i_obs_fp, "%[^\n]s", t);
        fscanf(i_tru_fp, "%[^\n]s", t);
        if (feof(i_obs_fp) || feof(i_tru_fp))
            assert(0);
        fgetc(i_obs_fp); 
        fgetc(i_tru_fp); 
        if (strcmp(t, str) == 0)
            break;
        fscanf(i_obs_fp, "%[^\n]s", t); fgetc(i_obs_fp); 
        fscanf(i_obs_fp, "%[^\n]s", t); fgetc(i_obs_fp); 
        fscanf(i_tru_fp, "%[^\n]s", t); fgetc(i_tru_fp); 
        fscanf(i_tru_fp, "%[^\n]s", t); fgetc(i_tru_fp); 
    }
    fprintf(o_obs_fp, "%s\n", str);
    fprintf(o_tru_fp, "%s\n", str);

    fscanf (i_obs_fp, "%[^\n]s", t); fgetc(i_obs_fp); 
    fprintf(o_obs_fp, "%s\n", t);
    fscanf (i_obs_fp, "%[^\n]s", t); fgetc(i_obs_fp); 
    fprintf(o_obs_fp, "%s\n", t);

    fscanf (i_tru_fp, "%[^\n]s", t); fgetc(i_tru_fp); 
    fprintf(o_tru_fp, "%s\n", t);
    fscanf (i_tru_fp, "%[^\n]s", t); fgetc(i_tru_fp); 
    fprintf(o_tru_fp, "%s\n", t);
}

main()
{
    FILE* pfp = fopen("preds.txt", "r");
    assert(pfp);

    o_obs_fp = fopen("newobs.txt", "w");
    o_tru_fp = fopen("newtru.txt", "w");
    assert(o_obs_fp && o_tru_fp);

    while (1) {
        int s, p;
        fscanf(pfp, "%d %d", &s, &p);
        if (feof(pfp))
            break;
        sprintf(str, "%d %d", s, p);
        i_obs_fp = fopen("obs.txt", "r");
        i_tru_fp = fopen("tru.txt", "r");
        assert(i_obs_fp && i_tru_fp);
        find_and_print();
        fclose(i_obs_fp);
        fclose(i_tru_fp);
    }
    fclose(pfp);
    fclose(o_obs_fp);
    fclose(o_tru_fp);
}
        	
