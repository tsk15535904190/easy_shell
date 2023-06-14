#include "stdio.h"
#include <string.h>
#include "stdint.h"
#include "shell.h"
#include "conio.h"
uint8_t test1(int a, char b, char *c)
{
    printf("a int :%d  a hex :0x%02x\r\n", a, a);
    printf("b:%c\r\n", b);
    printf("c:%s\r\n", c);
}
uint8_t test2(int a, int b, int c)
{
    printf("a:%d\r\n", a);
    printf("b:%d\r\n", b);
    printf("c:%d\r\n", c);
}
uint8_t test3(int a)
{
    printf("a int :%d  a hex :0x%02x\r\n", a, a);
}

int main(void)
{
    char c;
    SHELL_ModInit();
    shell_RegCmd("test1", test1);
    shell_RegCmd("test2", test2);
    shell_RegCmd("test3", test3);

    while (c = getch())
    {
        if (c == 27)
        {
            break;
        }
        else
        {
            shell_handleFunc(c);
        }
    }
    return 0;
}