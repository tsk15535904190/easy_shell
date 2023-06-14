#ifndef SHELL_LIST_H
#define SHELL_LIST_H

#include "stdio.h"
#include "stdint.h"

typedef uint8_t(*Shell_function)();

typedef struct _Node
{
    struct _Node *prev;
    struct _Node *next;
}Node;

typedef struct _SHELLNODE
{
    Node node;
    const char *name;
    Shell_function handleFunc;
}ShellNode;

typedef struct _List
{
    Node head;
    Node tail;
}List;

/*�����ĳ�ʼ��*/
void List_init(List *l);
/*������ͷ����*/
void List_InsertHead(List *l,Node *n);
/*������β����*/
void List_InsertTail(List *l,Node *n);
/*������ͷɾ��һ??�ڵ�*/
void List_DeleteHead(List *l);
/*������βɾ��һ??�ڵ�*/
void List_DeleteTail(List *l);
/*��������ɾ���ڵ�*/
void List_DeleteNode(Node *n);
/*�����ı�??*/
void List_Foreach(List *l);
#endif