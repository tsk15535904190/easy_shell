#include "shell.h"
#include "string.h"
#include "stdarg.h"

typedef enum para_type
{
    E_INT = 0x00,
    E_HEX = 0x01,
    E_CHAR = 0x02,
    E_STRING = 0x03,
} para_type_t;

static const char WelcomeText[] = "\r\n"
                                  "|APP shell Build: "__DATE__
                                  " "__TIME__
                                  ">>\r\n";

static List shellList;

#if SHELL_HEAP_SUPPORT
static uint8_t shell_heap[SHELL_HEAP_SIZE];
uint16_t shell_heapIndex = 0;
static void *shell_malloc(uint16_t size)
{
    uint8_t *p = NULL;
    if ((shell_heapIndex + size) > SHELL_HEAP_SIZE) // 堆空间不足
    {
        return p;
    }
    p = &(shell_heap[shell_heapIndex]);
    shell_heapIndex += size;
    return p;
}
#endif

uint32_t SHELL_Printf(const char *format, ...)
{
    uint8_t printBuf[SHELL_BUF_LEN] = {0};
    uint32_t printLen = 0;
    va_list args;

    va_start(args, format);
    printLen = vsnprintf(printBuf, SHELL_BUF_LEN - 1, format, args);
    va_end(args);
    shellWriteBuf(printBuf, printLen);
    return printLen;
}

void SHELL_PrintfBuf(const char *buf)
{
    uint16_t len = strlen(buf);

    if (len)
    {
        SHELL_Printf(buf, len);
        SHELL_Printf("\r\n", 2);
    }
}

void SHELL_ModInit(void)
{
    List_init(&shellList);
    memset(shell_heap, 0x00, (SHELL_HEAP_SIZE * sizeof(uint8_t)));
    SHELL_PrintfBuf(WelcomeText);
}

void shell_RegCmd(const char *name, Shell_function func)
{
    ShellNode *shell_node = NULL;
    for (shell_node = ((ShellNode *)shellList.head.next); shell_node != ((ShellNode *)&shellList.tail); shell_node = ((ShellNode *)((Node *)shell_node)->next))
    {
        if (0 == strcmp((const char *)name, shell_node->name))
        {
            SHELL_PrintfBuf(name);
            SHELL_PrintfBuf("command has been registered!");
            return;
        }
    }
    shell_node = NULL;
#if SHELL_HEAP_SUPPORT
    shell_node = (ShellNode *)shell_malloc(sizeof(ShellNode));
#else
    shell_node = malloc(sizeof(Node));
#endif
    if (NULL == shell_node) // 空间申请失败，空间不足
    {
        SHELL_PrintfBuf("heap memory not enough! reg fail!");
        return;
    }
    shell_node->handleFunc = func;
    shell_node->name = name;
    List_InsertTail(&shellList, (Node *)shell_node);
}

static int cmd_item_analyze_int(const char *str)
{
    int32_t value = 0, symbol = 1;
    size_t len = strlen((char *)str);
    int32_t pos = 0;

    if (len < 1)
    {
        return value;
    }
    if (str[0] == '-')
    {
        symbol = -1;
        pos = 1;
    }
    for (; pos < len; pos++)
    {
        if (('0' <= str[pos]) && (str[pos] <= '9'))
        {
            value *= 10;
            value += str[pos] - '0';
        }
        else
        {
            break;
        }
    }
    return symbol * value;
}
uint32_t cmd_item_analyze_hex(const uint8_t *str)
{
    uint32_t value = 0;
    size_t len = strlen((char *)str);
    int32_t pos = 0;

    if (len < 3)
    {
        return value;
    }

    for (pos = 2; pos < len; pos++)
    {
        if (('0' <= str[pos]) && (str[pos] <= '9'))
        {
            value *= 16;
            value += str[pos] - '0';
        }
        else if (('a' <= str[pos]) && (str[pos] <= 'f'))
        {
            value *= 16;
            value += str[pos] - 'a' + 10;
        }
        else if (('A' <= str[pos]) && (str[pos] <= 'F'))
        {
            value *= 16;
            value += str[pos] - 'A' + 10;
        }
        else
        {
            return value;
        }
    }
    return value;
}
para_type_t cmd_get_para_type(const char *str)
{
    size_t len = strlen((char *)str);
    uint8_t index = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            index++;
        }
        else
        {
            break;
        }
    }
    if (index == len) // 字符串都在0~9 代表是数字
    {
        return E_INT;
    }
    else if (1 == len) // 长度为1
    {
        if ((str[0] >= 'A') && (str[0] <= 'Z') || (str[0] >= 'a') && (str[0] <= 'z')) // 长度为1 表示为char
        {
            return E_CHAR;
        }
    }
    else
    {
        if (('0' == str[0]) && (('x' == str[1]) || ('X' == str[1])))
        {
            uint8_t flag;
            for (int i = 2; i < len; i++)
            {
                char a = str[i];
                if ((a >= 'A') && (a <= 'F') || (a >= 'a') && (a <= 'f') || (a >= '0') && (a <= '9'))
                {
                    flag++;
                }
            }
            if (len == flag + 2)
                return E_HEX;
        }
        return E_STRING;
    }
}
void cmd_analyze(const char *str) // str must start and end with space
{
    uint8_t index = 1;
    char c = str[index];
    char last_c = str[0];
    char cmd_item[SHELL_CMD_MAX_PARA_NUM][SHELL_CMD_MAX_PARA_LEN] = {0};

    para_type_t cmd_para_type[SHELL_CMD_MAX_PARA_NUM] = {0};
    #ifdef _WIN64
    uint64_t cmd_para[SHELL_CMD_MAX_PARA_NUM];
    #else
    uint32_t cmd_para[SHELL_CMD_MAX_PARA_NUM];
    #endif
    uint8_t item_num_index = 0;
    uint8_t item_str_index = 0;
    while (c)
    {
        if (' ' != c && ' ' == last_c) //
        {
            cmd_item[item_num_index][item_str_index] = c;
            item_str_index++;
        }
        else if (' ' == c && ' ' != last_c)
        {
            item_num_index++;
            item_str_index = 0;
        }
        else
        {
            if (' ' != c)
            {
                cmd_item[item_num_index][item_str_index] = c;
                item_str_index++;
            }
        }

        index++;
        last_c = c;
        c = str[index];
    }
    for (int i = 1; i < item_num_index; i++)
    {
        cmd_para_type[i] = cmd_get_para_type(cmd_item[i]);
        switch (cmd_para_type[i])
        {
        case E_INT:
            cmd_para[i] = cmd_item_analyze_int(cmd_item[i]);
            break;
        case E_CHAR:
            cmd_para[i] = cmd_item[i][0];
            break;
        case E_HEX:
            cmd_para[i] = cmd_item_analyze_hex(cmd_item[i]);
            break;
        case E_STRING:
            cmd_para[i] = cmd_item[i];
            break;
        default:
            break;
        }
    }

    ShellNode *shell_node = NULL;

    for (shell_node = ((ShellNode *)shellList.head.next); shell_node != ((ShellNode *)&shellList.tail); shell_node = ((ShellNode *)((Node *)shell_node)->next))
    {
        if (0 == strcmp(cmd_item[0], shell_node->name))
        {
            switch (item_num_index - 1) // 函数名也算是一个参数，因此在此减去一
            {
            case 1:
                shell_node->handleFunc(cmd_para[1]);
                break;
            case 2:
                shell_node->handleFunc(cmd_para[1], cmd_para[2]);
                break;
            case 3:
                shell_node->handleFunc(cmd_para[1], cmd_para[2], cmd_para[3]);
                break;
            case 4:
                shell_node->handleFunc(cmd_para[1], cmd_para[2], cmd_para[3],cmd_para[4]);
                break;
            case 5:
                shell_node->handleFunc(cmd_para[1], cmd_para[2], cmd_para[3],cmd_para[4], cmd_para[5]);
                break;
            case 6:
                shell_node->handleFunc(cmd_para[1], cmd_para[2], cmd_para[3],cmd_para[4], cmd_para[5], cmd_para[6]);
                break;
            case 7:
                shell_node->handleFunc(cmd_para[1], cmd_para[2], cmd_para[3],cmd_para[4], cmd_para[5], cmd_para[6],cmd_para[7]);
                break;
            case 8:
                shell_node->handleFunc(cmd_para[1], cmd_para[2], cmd_para[3],cmd_para[4], cmd_para[5], cmd_para[6],cmd_para[7], cmd_para[8]);
                break;
            case 9:
                shell_node->handleFunc(cmd_para[1], cmd_para[2], cmd_para[3],cmd_para[4], cmd_para[5], cmd_para[6],cmd_para[7], cmd_para[8], cmd_para[9]);
                break;
            default:
                break;
            }
            return;
        }
        else
        {
            continue;
        }
    }
    SHELL_Printf("unknow cmd!\r\n");
}

uint8_t shell_handleFunc(const char c)
{
    static char input_string[SHELL_CMD_MAX_LEN];
    static uint8_t input_index = 1;
    if (('\n' == c) || ('\r' == c))
    {
        input_string[input_index] = ' ';
        input_string[0] = ' ';
        SHELL_Printf("=>:%s\r\n", input_string);
        cmd_analyze(input_string);
        memset(input_string, 0, SHELL_CMD_MAX_LEN);
        input_index = 1;
    }
    else
    {
        input_string[input_index++] = c;
    }
}
