#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

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
	const char *short_opts = ":chnqB:";

	struct option long_opts[] = {
		{"count", no_argument, NULL, 'c'},
		{"help", no_argument, NULL, 'h'},
		{"line-number", no_argument, NULL, 'n'},
		{"quite", no_argument, NULL, 'q'},
		{"before-context", required_argument, NULL, 'B'},
		{NULL, 0, NULL, 0}};

	// flags
	bool ignore_case = false;
	bool count = false;
	bool line_number = false;
	bool quite = false;

	int max_count = -1;
	int ret = 0;

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
		case 'h':
			printf(USAGE);
			return 0;
		case 'n':
			line_number = true;
			break;
		case 'q':
			quite = true;
			break;
		case 'B':
			/* TODO*/
		}
	}

	nargs = argc - optind;
	if (nargs != 1)
	{
		printf("Invalid Input. Run -h to see usage \n");
		return 1;
		// mu_die("expected two positional arguments, but found %d", nargs);
	}

	printf("count: %s\n", count ? "true" : "false");
	printf("line_number: %s\n", max_count ? "true" : "false");
	printf("quite: \"%s\"\n", quite ? "true" : "false");

	file* FILE;

	fh = open(argv, "r");

	if (fh == NULL) {
		printf("Please input a file \n");
        ret 1;
    }

	char* line;
	size_t bufferLength = 0;
	size_t lineLength = 0;

	// file reading loop
	while (1) {
		lineLength = getLine(&line, &bufferLength, FILE);

		if (lineLength == -1) {
			break;
		}
	}


}
