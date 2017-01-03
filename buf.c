#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include "buf.h"

#ifdef MAX
#undef MAX
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BUF_GET_TYPE(type) \
int \
buf_get_##type(struct buf *b, type##_t *pr) \
{ \
    return buf_get_copy(b, pr, sizeof(*pr)); \
}

BUF_GET_TYPE(int8)
BUF_GET_TYPE(uint8)
BUF_GET_TYPE(int16)
BUF_GET_TYPE(uint16)
BUF_GET_TYPE(int32)
BUF_GET_TYPE(uint32)
BUF_GET_TYPE(int64)
BUF_GET_TYPE(uint64)

static void *
xmalloc(size_t size)
{
    void *ptr;
    if ((ptr = malloc(size)) == NULL)
        exit(255);
    return ptr;   
}

void
buf_init(struct buf *b)
{
	b->buf = NULL;
	b->alloc = b->size = 0;
}

void
buf_grow(struct buf *b, size_t count)
{
	if (b->size + count <= b->alloc)
		return;

	b->alloc = MAX(b->size + count, b->alloc*3/2);

	if ((b->buf = realloc(b->buf, b->alloc)) == NULL)
		exit(-1);
}

void
buf_add(struct buf *b, const void *buf, size_t count)
{
	buf_grow(b, count);
	if (buf == NULL)
		memset(b->buf + b->size, 0, count);
	else
		memcpy(b->buf + b->size, buf, count);
	b->size += count;
}

void
buf_add_byte(struct buf *b, char byte)
{
	buf_grow(b, 1);
	b->buf[b->size++] = byte;
}

void
buf_add_w(struct buf *b, uint32_t u)
{
    size_t i;

	for (i = 28; i >= 7; i -= 7) {
    	if (u >= (1 << i))
				buf_add_byte(b, (u >> i) | 0x80);
	}
	buf_add_byte(b, u & 0x7f);
}

void *
buf_get(struct buf *b, size_t count)
{
	void *p;

	if (b->size + count > b->alloc)
		return NULL;

	p = b->buf + b->size;

	b->size += count;
	
	return p;
}

int
buf_get_copy(struct buf *b, void *buf, size_t count)
{
	void *p;

	if ((p = buf_get(b, count)) == NULL)
		return -1;

	memcpy(buf, p, count);

	return 0;
}

int
buf_get_w(struct buf *b, uint32_t *pr)
{
    size_t i;
    uint32_t r;
    uint8_t c;

    r = 0;

    for (i = 0; i < 5; ++i) {
        if (buf_get_uint8(b, &c) == -1)
            return -1;
        r <<= 7;
        r |= c & 0x7f;
        if ((c & 0x80) == 0)
            break;
    }

    if (pr != NULL)
        *pr = r;

    return 0;
}

void
buf_release(struct buf *b)
{
	free(b->buf);
}

size_t
escape(char *s, size_t n, const char *buf, size_t count)
{
	size_t i, len;
	char c;

    len = 0;

    for (i = 0; i < count; ++i) {
        c = buf[i];

        if (isprint(c) && c != '\\' && c != ' ') {
            if (len < n)
    			s[len] = c;
            len += 1;
		} else {
            if (len < n)
			    snprintf(s + len, n - len, "\\x%.2x", c);
            len += 3;
        }
    }

    if (n)
        s[n - 1] = '\0';

    return len;
}

char *
aescape(const char *buf, size_t count)
{
    size_t n;
    char *s;

    n = escape(NULL, 0, buf, count);

    s = xmalloc(n + 1);

    escape(s, n + 1, buf, count);

    return s;
}

ssize_t
unescape(char *buf, size_t count, const char *s)
{
    const char *p;
    char cp[4];
    char c;
    size_t i;

    i = 0;
    p = s;
    cp[3] = '\0';

    while (*p != '\0') {
        c = *p;
        if (c != '\\') {
            p += 1;
        } else {
            if (*(p + 1) == '\0' || *(p + 2) == '\0' || *(p + 3) == '\0')
                break;
            
            cp[0] = *(p + 2);
            cp[1] = *(p + 3);
            
            c = strtoul(cp, NULL, 16);

            p += 4;
        }
        if (i < count)
            buf[i] = c;
        ++i;
    }

    return i;
}

size_t
aunescape(char **buf, const char *s)
{
    ssize_t count = unescape(NULL, 0, s);
    *buf = xmalloc(count);
    return unescape(*buf, count, s);
}
