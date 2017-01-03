#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include "buf.h"

#define IPRO_HEADER_SIZE 12

int
scan_stdin(const char *format)
{
    struct buf b;
    ssize_t buf_len;
    size_t size, i;
    char *s;
    uint32_t u;
    int32_t msg, len, syn, d;
    char buf[256];

    buf_init(&b);

    do {
        buf_len = read(STDIN_FILENO, buf, sizeof(buf));
        if (buf_len < 0) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "ipro_scanf: read error: %s", strerror(errno));
            goto err;
        }

        if (buf_len > 0)
            buf_add(&b, buf, buf_len);
    } while (buf_len);

    if (b.size < IPRO_HEADER_SIZE)
        goto err;

    size = b.size;
    b.size = 0;
    buf_get_int32(&b, &msg);
    buf_get_int32(&b, &len);
    buf_get_int32(&b, &syn);
    b.size = size;

    printf("(%"PRIu32", %"PRIu32", %"PRIu32") ", msg, len, syn);
    b.size = IPRO_HEADER_SIZE;

    if (format == NULL)
        goto out;

    for (i = 0; format[i] != '\0'; ++i) {
        if (i)
            printf(", ");
        switch (format[i]) {
            case 'u':
                if (buf_get_uint32(&b, &u) < 0)
                    goto scan_err;

                printf("%"PRIu32, u);
                break;

            case 'd':
                if (buf_get_int32(&b, &d) < 0)
                    goto scan_err;

                printf("%"PRId32, d);
                break;

            case 's':
                if (buf_get_uint32(&b, &u) < 0 || (s = buf_get(&b, u)) == NULL)
                    goto scan_err;

                s = aescape(s, u);
                printf("%s", s);
                free(s);
                break;

            case 'w':
                if (buf_get_w(&b, &u) < 0)
                    goto scan_err;

                printf("%"PRId32, u);
                break;

            case 'W':
                if (buf_get_w(&b, &u) < 0 || (s = buf_get(&b, u)) == NULL)
                    goto scan_err;

                s = aescape(s, u);
                printf("%s", s);
                free(s);
                break;

            default:
                fprintf(stderr, "ipro_scanf: '%c' Invalid format character\n",
                        format[i]);
                goto scan_err;
        }
    }

out:
    printf("\n");
    buf_release(&b);
    return EXIT_SUCCESS;

scan_err:
    printf("\n");
err:
    buf_release(&b);
    return EXIT_FAILURE;
}

void
print_usage()
{
    printf("Usage: ipro_scanf [ -h ] { format [udswW] }\n");
}

int
main(int argc, char **argv)
{
    int opt;

    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
        }
    }

    return scan_stdin(argv[optind]);
}
