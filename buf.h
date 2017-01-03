#ifndef BUF_H
#define BUF_H

struct buf {
    char *buf;
    size_t alloc, size;
};

void
buf_init(struct buf *b);

void
buf_grow(struct buf *b, size_t count);

void
buf_add(struct buf *b, const void *buf, size_t count);

void
buf_add_byte(struct buf *b, char byte);

void
buf_add_w(struct buf *b, uint32_t u);

void *
buf_get(struct buf *b, size_t count);

int
buf_get_copy(struct buf *b, void *buf, size_t count);

int
buf_get_w(struct buf *b, uint32_t *pr);

void
buf_release(struct buf *b);

#define BUF_GET_TYPE_DECLARE(type) \
int \
buf_get_##type(struct buf *b, type##_t *pr);

BUF_GET_TYPE_DECLARE(int8)
BUF_GET_TYPE_DECLARE(uint8)
BUF_GET_TYPE_DECLARE(int16)
BUF_GET_TYPE_DECLARE(uint16)
BUF_GET_TYPE_DECLARE(int32)
BUF_GET_TYPE_DECLARE(uint32)
BUF_GET_TYPE_DECLARE(int64)
BUF_GET_TYPE_DECLARE(uint64)

char *
aescape(const char *buf, size_t count);

size_t
aunescape(char **buf, const char *s);

#endif
