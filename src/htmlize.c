#include <stdio.h>

main()
{
    char s[10000];
    int line = 1;

    printf("<html><body>\n");
    while (1) {
        fgets(s, 10000, stdin);
        if (feof(stdin))
            break;
        printf("<a name=\"%d\">%s<br>", line, s);
        line++;
    }
    printf("</body></html>\n");
}

