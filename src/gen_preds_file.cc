#include <cstdio>

int main(int, char*[])
{
    int i;
    float f;

    while (1) {
	scanf("%d %f", &i, &f);
	if (feof(stdin))
	    break;
	printf("%s\n", preds[i]);
    }

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
