#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
	FILE *fp_in;
	FILE *fp_out;

	if (argc < 3)
	{
		printf("Usage: txt2bin <input txt file> <output txt file>\n");
		return(-1);
	}

	fp_in = fopen(argv[1], "rt");
	if (fp_in == NULL) {
		printf("Unable to open %s.\n", argv[1]);
		return -1;
	}

	fp_out = fopen(argv[2], "wb");
	if (fp_out == NULL) {
		printf("Unable to open output file %s\n.", argv[2]);
	}

	int ret;
	do {
		int value = 0;
		ret = fscanf(fp_in, "%d\n", &value);
		char byte = (char)value;
		fwrite(&byte, sizeof(byte), 1, fp_out);
	} while (ret != EOF);

	fclose(fp_out);
	fclose(fp_in);

	printf("Done.\n");
	return 0;
}
