#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include "circArray.h"

#define USAGE                                               \
	"Usage: grep_stub [-m] [-h] [-i] PATTERN FILE\n"        \
	"\n"                                                    \
	"Print lines in FILE that match PATTERN.\n"             \
	"\n"                                                    \
	"optional arguments\n"                                  \
	"   -h, --help\n"                                       \
	"       Show usage statement and exit.\n"               \
	"\n"                                                    \
	"   -i, --ignore-case\n"                                \
	"       Perform a case-insensitve match for PATTERN.\n" \
	"\n"                                                    \
	"   -m, --max-count NUM\n"                              \
	"       Stop reading FILE after NUM matching lines.\n"

int main(int argc, char *argv[])
{
	int opt, nargs;
	const char *short_opts = ":cvhnqB:";

	struct option long_opts[] = {
		{"count", no_argument, NULL, 'c'},
		{"reverse-count", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{"line-number", no_argument, NULL, 'n'},
		{"quite", no_argument, NULL, 'q'},
		{"before-context", required_argument, NULL, 'B'},
		{NULL, 0, NULL, 0}};

	// flags
	bool ignore_case = false;
	bool count = false;
	bool reverseCount = false;
	bool lineNumber = false;
	bool quite = false;
	bool context = false;

	int contextLines = 0;

	while (1)
	{
		int opt = getopt_long(argc, argv, short_opts, long_opts, NULL);

		if (opt == -1)
			break;

		switch (opt)
		{
		case 'c':
			count = true;
			break;
		case 'v':
			reverseCount = true;
			break;
		case 'h':
			printf(USAGE);
			return 0;
		case 'n':
			lineNumber = true;
			break;
		case 'q':
			quite = true;
			break;
		case 'B':
			context = true;
			contextLines = atoi(optarg);
			break;
		}
	}
	nargs = argc - optind;
	if (nargs != 2)
	{
		printf("Invalid Input. Run -h to see usage \n");
		return 1;
		// mu_die("expected two positional arguments, but found %d", nargs);
	}

	// general needed info
	char *fileName = argv[optind + 1];
	char *target = argv[optind];
	bool exitStatus = 1;
	char *line;
	size_t bufferLength = 0;
	size_t lineLength = 0;

	// path to filename in directory
	char path[100];
	strcpy(path, "./");
	strcat(path, fileName);
	FILE *fh;
	fh = fopen(path, "r");

	if (fh == NULL)
	{
		printf("Please input a file \n");
		return 1;
	}

	// flag specific data
	int matchedLines = 0;
	int matchedReversedLines = 0;
	int lineCount = 0;
	circArray circArr = createCircArray(contextLines);

	// file reading loop
	while (1)
	{
		lineLength = getline(&line, &bufferLength, fh);
		lineCount++;
		if (lineLength == -1)
		{
			break;
		}

		char *p = strstr(line, target);
		if (p)
		{
			if (quite) return 1;
			if (context && (!count && !reverseCount)) printLastLines(&circArr, lineNumber, lineCount);
			matchedLines++;
			if (!count && !reverseCount)
			{
				if (lineNumber)
					printf("%d:", lineCount);
				printf("%s", line);
			}
		}
		else
		{
			matchedReversedLines++;
		}
		if (context) insert(&circArr, line);
	}

	if (count)
		printf("%d\n", matchedLines);
	if (reverseCount)
		printf("%d\n", matchedReversedLines);

	fclose(fh);
	deleteCircArray(&circArr);

	return exitStatus;
}
