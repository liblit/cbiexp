#include <cstdio>

main()
{
    int i;
    float f;

    while (1) {
	scanf("%d %f", &i, &f);
	if (feof(stdin))
	    break;
	printf("%s\n", preds[i]);
    }
}
