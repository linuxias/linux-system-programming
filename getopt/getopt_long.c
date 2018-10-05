#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

static const char *optstring = "o:hv";

static const struct option long_options[] = {
    { "out", required_argument, NULL, 'o' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

int main(int argc, char *argv[])
{
    char optc;

    while ((optc = getopt_long(argc, argv, optstring, long_options, NULL)) != -1) {
        switch (optc) {
            case 'o':
                printf("out argument : %s\n", optarg);
                break;
            case 'h':
                printf("help");
                break;
            case 'v':
                printf("version");
                break;
            default:
                break;
        }
    }

    return 0;
}
