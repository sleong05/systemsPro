#include "mu.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <fcntl.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define USAGE                                                                                                                                                       \
    "Usage: fedit [-l] [-r] [-x] [-c] [-k] FILE\n"                                                                                                                  \
    "\n"                                                                                                                                                            \
    "The fedit program performs one of the following operations on a file.\n"                                                                                       \
    "\n"                                                                                                                                                            \
    "   -l, --rotate-left NROTL\n"                                                                                                                                  \
    "       Rotate the file NROTL bytes left (with wrap around).\n"                                                                                                 \
    "\n"                                                                                                                                                            \
    "   -r, --rotate-right NROTR\n"                                                                                                                                 \
    "       Rotate the file NROTR bytes right (with wrap around).\n"                                                                                                \
    "\n"                                                                                                                                                            \
    "   -x, --expand\n"                                                                                                                                             \
    "       Expand the file by NEXPAND. The value of these bytes is given by the --value option.\n"                                                                 \
    "\n"                                                                                                                                                            \
    "   -c, --contract NCONTRACT\n"                                                                                                                                 \
    "      Contract (shrink) the file by NCONTRACT bytes. If NCONTRACT > FSIZE, shrink the file to 0 bytes.\n"                                                      \
    "\n"                                                                                                                                                            \
    "   -k, --keep NKEEP\n"                                                                                                                                         \
    "      Keep this many bytes (starting from the offset provided by --skip) (and remove all others). In the case of EOF, fewer than NKEEP bytes may be kept.\n"   \
    "optional arguments\n"                                                                                                                                          \
    "\n"                                                                                                                                                            \
    "   -s, --skip NSKIP\n"                                                                                                                                         \
    "      Optionally used with --keep. Skip NKSIP bytes before keeping. Default : 0 It is an error to use this option if the user does not also specify-- keep.\n" \
    "\n"                                                                                                                                                            \
    "   -v, --value CHAR\n"                                                                                                                                         \
    "      The character value that is used when expanding the file. Default: A.\n"                                                                                 \
    "\n"                                                                                                                                                            \
    "   -m, --repeat\n"                                                                                                                                             \
    "      Repeat skip/keep operations as many times until the end of the file is reached.  It is an error to use this option if the user does not also specify-- keep.\n"

enum action
{
    NOACTION,
    rLeft,
    rRight,
    keep,
    expand,
    contract
};

static void rotate_left(char *path, int n, int size);
static void keepBytes(char *path, int start, int bytesToKeep, int size);
static void rotate_right(char *path, int n, int size);

int main(int argc, char *argv[])
{
    int opt, nargs;
    const char *short_opts = ":hl:r:x:c:v:k:s:";
    struct option long_opts[] = {
        {"help", no_argument, NULL, 'h'},
        {"rotate-left", required_argument, NULL, 'l'},
        {"rotate-right", required_argument, NULL, 'r'},
        {"expand", required_argument, NULL, 'x'},
        {"contract", required_argument, NULL, 'c'},
        {"value", required_argument, NULL, 'v'},
        {"keep", required_argument, NULL, 'k'},
        {"skip", required_argument, NULL, 's'},
        {NULL, 0, NULL, 0}};

    // flags
    enum action operation = NOACTION;
    int mainArg;

    char expansionChar = 'A';
    int offSet = 0;
    // parsing args
    while (1)
    {
        opt = getopt_long(argc, argv, short_opts, long_opts, NULL);
        if (opt == -1)
            break;
        // ensure one main op is selected
        if (operation != NOACTION) {
            switch (opt) {
                case 'l':
                case 'r':
                case 'x':
                case 'c':
                case 'k':
                    fprintf(stderr, "Only one main flag may be selected\n");
                    return 1;
            }
        }
        switch (opt)
        {
        case 'h':
            printf(USAGE);
            return 0;
        case 'l':
            operation = rLeft;
            mainArg = atoi(optarg);
            break;
        case 'r':
            operation = rRight;
            mainArg = atoi(optarg);
            break;
        case 'x':
            operation = expand;
            mainArg = atoi(optarg);
            break;
        case 'c':
            operation = contract;
            mainArg = atoi(optarg);
            break;
        case 'k':
            operation = keep;
            mainArg = atoi(optarg);
            break;
        case 'v':
            expansionChar = optarg[0];
            break;
        case 's':
            offSet = atoi(optarg);
            break;
        case '?': /* ... unknown option */
        case ':': /* ... option missing required argument */
        default:
            /* ... unexpected; here for completeness */
        }
    }

    nargs = argc - optind;
    if (nargs != 1)
    {
        mu_die("expected one positional arguments, but found %d", nargs);
        return 1;
    }

    // general needed info
    char *fileName = argv[optind];

    // path to filename in directory
    char path[100];
    strcpy(path, "./");
    strcat(path, fileName);

    // get size of file
    struct stat st;
    stat(path, &st);
    int size = st.st_size;
    int dsize;

    switch (operation)
    {
    case rLeft:
        rotate_left(path, mainArg, size);
        break;
    case rRight:
        rotate_right(path, mainArg, size);
        break;
    case keep:
        keepBytes(path, offSet, mainArg, size);
        break;
    case expand:
    {
        dsize = size + mainArg;
        truncate(path, dsize);

        int fd = open(path, O_RDWR);
        char *buf = malloc(mainArg);

        lseek(fd, size, SEEK_SET);
        memset(buf, expansionChar, mainArg);

        write(fd, buf, mainArg);

        free(buf);
        close(fd);
        break;
    }
    case contract:
        dsize = (size > mainArg) ? (size - mainArg) : 0;
        truncate(path, dsize);
        break;

    default:
        break;
    }
    return 0;
}

static void keepBytes(char *path, int start, int bytesToKeep, int size) {
    if (start >= size) return;

    int fd = open(path, O_RDWR);

    int end = MIN(bytesToKeep+ start, size);

    int length = end - start;
    char *textSegment = malloc(length);
    pread(fd, textSegment, length, start );

    write(fd, textSegment, length);
    ftruncate(fd, length);

    close(fd);
    free(textSegment);
}

static void rotate_left(char *path, int n, int size)
{
    int fd = open(path, O_RDWR);

    n %= size;

    char *prefix = malloc(n);
    char *buf = malloc(size);

    pread(fd, prefix, n, 0);
    pread(fd, buf, size - n, n);

    memcpy(buf + (size - n), prefix, n);

    lseek(fd, 0, SEEK_SET);
    write(fd, buf, size);

    close(fd);
    free(prefix);
    free(buf);
}

static void rotate_right(char *path, int n, int size)
{
    int fd = open(path, O_RDWR);

    n %= size;

    char *postfix = malloc(size);
    char *buf = malloc(size-n);

    pread(fd, postfix, n, size-n);
    pread(fd, buf, size-n, 0);

    memcpy(postfix + n, buf, size-n);

    lseek(fd, 0, SEEK_SET);
    write(fd, postfix, size);

    close(fd);
    free(postfix);
    free(buf);
}