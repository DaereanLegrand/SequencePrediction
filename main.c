#include <limits.h>
#include <stdio.h>
#include <string.h>

#define MAX_SEQ_LEN 100

typedef struct {
	int value;
	int paired_with;
} Cell;

void
print_table(Cell table[MAX_SEQ_LEN][MAX_SEQ_LEN], int n, char *seq)
{
	printf("\t");
	for (int i = 0; i < n; i++)
		printf("| %c\t|", seq[i]);
	printf("\n");
	for (int i = 0; i < n; i++) {
		printf("| %c\t|", seq[i]);
		for (int j = 0; j < n; j++) {
			if (table[i][j].value != INT_MAX)
				printf("| %d,%d\t|", table[i][j].value, table[i][j].paired_with);
			else
				printf("|  \t|");
		}
		printf("\n");
	}
}

void
export_pairings(Cell table[MAX_SEQ_LEN][MAX_SEQ_LEN], int n, char *seq, const char *filename)
{
	FILE *fp = fopen(filename, "w");
	fprintf(fp, "%s\n", seq);
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (table[i][j].paired_with == 3)
				fprintf(fp, "%d %d\n", i, j);
		}
	}
	fclose(fp);
}

int
freeEnergy1(char a, char b)
{
	int sum = (a + b);
	switch (sum) {
	case 'C' + 'G':
		return -5;
	case 'A' + 'U':
		return -4;
	case 'G' + 'U':
		return -1;
	default:
		return 0;
	}
}

int
freeEnergy2(char a, char b)
{
	int sum = (a + b);
	return (sum == ('A' + 'U') || sum == ('C' + 'G')) ? -1 : 0;
}

void
initialize(Cell table[MAX_SEQ_LEN][MAX_SEQ_LEN], int n)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			table[i][j].value = INT_MAX;
			table[i][j].paired_with = -1;
		}
		table[i][i].value = 0;
		if (i > 0)
			table[i][i - 1].value = 0;
	}
}

void
fill_energy_table(Cell table[MAX_SEQ_LEN][MAX_SEQ_LEN], int n, char *seq, int (*energy_func)(char, char))
{
	for (int d = 1; d < n; d++) {
		for (int i = 0; i + d < n; i++) {
			int j = i + d, min_val = INT_MAX, min_choice = -1;

			if (table[i + 1][j].value < min_val) {
				min_val = table[i + 1][j].value;
				min_choice = 1;
			}
			if (table[i][j - 1].value < min_val) {
				min_val = table[i][j - 1].value;
				min_choice = 2;
			}
			int val3 = table[i + 1][j - 1].value + energy_func(seq[i], seq[j]);
			if (val3 < min_val) {
				min_val = val3;
				min_choice = 3;
			}
			for (int k = i; k < j; k++) {
				int val4 = table[i][k].value + table[k + 1][j].value;
				if (val4 < min_val) {
					min_val = val4;
					min_choice = 4;
				}
			}
			table[i][j].value = min_val;
			table[i][j].paired_with = min_choice;
		}
	}
}

int
main()
{
	char *sequences[] = {"GGAAAUCC", "ACUCGAUUCCGAG"};
	int (*energy_functions[])(char, char) = {freeEnergy1, freeEnergy2};

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			char *seq = sequences[i];
			int n = strlen(seq);
			Cell energy[MAX_SEQ_LEN][MAX_SEQ_LEN];

			printf("Sequence: %s with Energy Function %d\n", seq, j + 1);
			initialize(energy, n);
			fill_energy_table(energy, n, seq, energy_functions[j]);

			print_table(energy, n, seq);
			char filename[25];
			sprintf(filename, "rna_pairs_%d_%d.txt", i + 1, j + 1);
			export_pairings(energy, n, seq, filename);
			printf("Pairings exported to %s\n", filename);
		}
	}
	return 0;
}
