#ifndef SHELL_H
#define SHELL_H

#include "stdint.h"
#include "shell_list.h"

extern void ShellWriteUart1(const unsigned char *buf, unsigned char len);
extern void ShellWriteCharUart1(unsigned char data);
#define shellWriteBuf  ShellWriteUart1     //注册串口输出字�?�接�?
#define shellWriteCharBuf  ShellWriteCharUart1     //注册串口输出字�?�接�?
#define SHELL_HEAP_SUPPORT 1
#define SHELL_HEAP_SIZE 1024                //shell占用静态空间大�?
#define SHELL_BUF_LEN  (255)                //shell临时打印缓冲长度
#define SHELL_CMD_MAX_LEN (128)             //shell最长的命令字符长度

#define SHELL_CMD_MAX_PARA_LEN   (15)   //每个参数最长长度，cmd本身也属于参数
#define SHELL_CMD_MAX_PARA_NUM   (10)   //每个命令的最大参数数量，cmd本身消耗一个数量


void SHELL_ModInit(void);
void shell_RegCmd(const char *name, Shell_function func);
uint8_t shell_handleFunc(const char c);

#endif