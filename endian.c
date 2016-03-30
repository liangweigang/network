//reference redis src: endianconv.c
#include <stdint.h>

uint16_t intrev16(uint16_t v);
uint32_t intrev32(uint32_t v);
uint64_t intrev64(uint64_t v);

// Toggle 16 bit
void endian16(void* p)
{
    unsigned char t, *x = p;
    t = x[0];
    x[0] = x[1];
    x[1] = t;
}

void endian32(void* p)
{
    unsigned char t, *x = p;
    t = x[0];
    x[0] = x[3];
    x[3] = t;
    t = x[1];
    x[1] = x[2];
    x[2] = t;
}

void endian64(void* p)
{
    unsigned char t, *x = p;
    t = x[0];
    x[0] = x[7];
    x[7] = t;
    t = x[1];
    x[1] = x[6];
    x[6] = t;
    t = x[2];
    x[2] = x[5];
    x[5] = t;
    t = x[3];
    x[3] = x[4];
    x[4] = t;
}

uint16_t intrev16(uint16_t v)
{
    endian16(&v);
    return v;
}

uint32_t intrev32(uint32_t v)
{
    endian32(&v);
    return v;
}

uint64_t intrev64(uint64_t v)
{
    endian64(&v);
    return v;
}

#ifdef TEST_DEBUG
int main(int argc, char const *argv[])
{
    char buf[32];

    sprintf(buf,"helloworld");
    endian16(buf);
    printf("%s\n", buf);

    sprintf(buf,"helloworld");
    endian32(buf);
    printf("%s\n", buf);

    sprintf(buf,"helloworld");
    endian64(buf);
    printf("%s\n", buf);

    return 0;
}
#endif
