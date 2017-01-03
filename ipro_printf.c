#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "buf.h"

void
print_usage()
{
    printf("Usage: ipro_printf [ -h ] [ -m msg ] [ -s syn ]\n"
           "        { format [udswW] } { args }\n");
}

int
main(int argc, char **argv)
{
    const char *format, *v;
    char *buf;
    int opt;
    size_t i, format_len;
    int32_t d;
    uint32_t msg, syn, u;
    struct buf h, b;

    msg = 0;
    syn = 0;

    while ((opt = getopt(argc, argv, "hm:s:")) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
                return EXIT_SUCCESS;

            case 'm':
                sscanf(optarg, "%"SCNu32, &msg);
                break;

            case 's':
                sscanf(optarg, "%"SCNu32, &syn);
                break;

            default:
                fprintf(stderr, "ipro_printf: bad format\n");
                return EXIT_FAILURE;
        }
    }

    buf_init(&b);

    if (argc - optind < 1)
        goto out;

    if ((format_len = strlen(format = argv[optind])) != argc - optind - 1) {
        fprintf(stderr, "ipro_printf: missing operand\n");
        return EXIT_FAILURE;
    }

    /* body */
    for (i = 0; i < format_len; ++i) {
        v = argv[optind + 1 + i];
        switch (format[i]) {
            case 'u':
                sscanf(v, "%"SCNu32, &u);
                buf_add(&b, &u, sizeof(u));
                break;

            case 'd':
                sscanf(v, "%"SCNd32, &d);
                buf_add(&b, &d, sizeof(d));
                break;

            case 's':
                u = aunescape(&buf, v);
                buf_add(&b, &u, sizeof(u));
                buf_add(&b, buf, u);
                free(buf);
                break;

            case 'w':
                sscanf(v, "%"SCNu32, &u);
                buf_add_w(&b, u);
                break;

            case 'W':
                u = (uint32_t)strlen(v);
                buf_add_w(&b, u);
                buf_add(&b, v, u);
                break;

            default:
                fprintf(stderr, "ipro_prinitf: '%c' Invalid format character\n",
                        format[i]);
                buf_release(&b);
                return EXIT_FAILURE;
        }
    }

out:
    /* header */
    buf_init(&h);
    buf_add(&h, &msg, sizeof(msg));
    u = (uint32_t)b.size;
    buf_add(&h, &u, sizeof(u));
    buf_add(&h, &syn, sizeof(syn));

    write(STDOUT_FILENO, h.buf, h.size);
    buf_release(&h);

    if (b.size != 0)
        write(STDOUT_FILENO, b.buf, b.size);
    buf_release(&b);

    return EXIT_SUCCESS;
}
