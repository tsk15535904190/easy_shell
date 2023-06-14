/**
 * @name        :cuteshell
 * @brief       :shell底层链表实现
 * @date        :2022/01/11
 * @author      :bliss
 **/
#include "shell_list.h"

/*链表的初始化*/
void List_init(List *l)
{
    l->head.next = &l->tail;
    l->head.prev = NULL;
    l->tail.prev = &l->head;
    l->tail.next = NULL;
}
/*在链表头插入*/
void List_InsertHead(List *l,Node *n)
{
    n->next = l->head.next;
    n->prev = &l->head;
    l->head.next->prev = n;
    l->head.next = n;
}
/*从链表尾插入*/
void List_InsertTail(List *l,Node *n)
{
    n->next = &l->tail;
    n->prev = l->tail.prev;
    l->tail.prev->next = n;
    l->tail.prev = n;
}
/*从链表头删除一个节点*/
void List_DeleteHead(List *l)
{
    if(l->head.next == &l->tail)
    {
        return;
    }
    l->head.next->next->prev = &l->head;
    l->head.next = l->head.next->next;
}
/*从链表尾删除一个节点*/
void List_DeleteTail(List *l)
{
    if(l->head.next == &l->tail)
    {
        return;
    }
    l->tail.prev->prev->next = &l->tail;
    l->tail.prev = l->tail.prev->prev;

}
/*从链表中删除节点*/
void List_DeleteNode(Node *n)
{
    n->prev->next = n->next;
    n->next->prev = n->prev;
}
/*链表的遍历*/
void List_Foreach(List *l)
{
    if(l->head.next == &l->tail)
    {
        return;
    }
    Node *n;
    for (n = l->head.next; n != &l->tail; n = n->next)
    {
    }
}