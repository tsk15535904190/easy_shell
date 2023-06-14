#include "shell_port.h"
#include "stdio.h"
void ShellWriteUart1(const unsigned char *buf, unsigned char len)
{
    printf("%.*s", len, buf);
}
void ShellWriteCharUart1(unsigned char data)
{
    printf("%.*s", 1, &data);
}