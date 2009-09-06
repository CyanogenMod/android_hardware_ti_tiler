#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __DEBUG__
#define __DEBUG_ASSERT__
#define __DEBUG_ENTRY__

#include "../src/utils.h"

#define F() in = head.next;
#define N(a) A_I(in,!=,&head); A_I(in->me->data,==,a); in = in->next;
#define L() A_I(in,==,&head);

int all_zero(int *p, int len)
{
    IN;
    int ix = 0;
    for (ix = 0; ix < len; ix ++)
    {
        if (p[ix])
        {
            P("[%d]=%d\n", ix, p[ix]);
            return R_I(1);
        }
    }
    return R_I(0);
}

void test_new() {
    IN;
    int *p;
    p = NEW(int);
    A_I(all_zero(p, 1),==,0);
    FREE(p);
    A_I(p,==,NULL);
    p = NEWN(int,8000);
    A_I(all_zero(p, 8000),==,0);
    FREE(p);
    OUT;
}

void test_list() {
    IN;

    struct elem {
        int data;
    } *elA, *elB;
    struct list {
        struct elem *me;
        struct list *last, *next;
    } head, *inA, *inB, *in, *in_safe;
    
    /* initialization */
    DLIST_INIT(head);    
    A_I(DLIST_IS_EMPTY(head),!=,0);

    /* add element at beginning of list */
    elA = NEW(struct elem);
    elA->data = 1;
    inA = NEW(struct list);
    DLIST_ADD_AFTER(head, elA, *inA);
    F()N(1)L();

    /* add element after an element */
    elB = NEW(struct elem);
    elB->data = 2;
    inB = NEW(struct list);
    DLIST_ADD_AFTER(*inA, elB, *inB);
    F()N(1)N(2)L();

    /* add element at the end of the list */
    elB = NEW(struct elem);
    inB = NEW(struct list);
    (DLIST_ADD_BEFORE(head, elB, *inB))->data = 3;
    F()N(1)N(2)N(3)L();

    /* move an element to another position or another list */
    DLIST_MOVE_AFTER(head, *inB);
    F()N(3)N(1)N(2)L();

    DLIST_MOVE_BEFORE(head, *inB);
    F()N(1)N(2)N(3)L();

    /* works even if the position is the same */
    DLIST_MOVE_BEFORE(head, *inB);
    F()N(1)N(2)N(3)L();

    A_I(DLIST_FIRST(head)->data,==,1);
    A_I(DLIST_LAST(head)->data,==,3);

    DLIST_LOOP(head, in) {
        P("%d", in->me->data);
    }
    P(".");

    /* remove elements */
    DLIST_SAFE_LOOP(head, in, in_safe) {
        if (in->me->data > 1)
        {
            DLIST_REMOVE(*in);
            FREE(in->me);
            FREE(in);
        }
    }
    F()N(1)L();

    /* delete list */
    DLIST_SAFE_LOOP(head, in, in_safe) {
        DLIST_REMOVE(*in);
        FREE(in->me);
        FREE(in);
    }
    F()L();

    OUT;
}

void test_ezlist() {
    IN;

    struct elem {
        int data;
        struct elem *me, *last, *next;
    } *elA, *elB, head, *el, *el_safe, *in;
    
    /* initialization */
    DLIST_INIT(head);    
    A_I(DLIST_IS_EMPTY(head),!=,0);

    /* add element at beginning of list */
    elA = NEW(struct elem);
    elA->data = 1;
    DLIST_ADD_AFTER(head, elA, *elA);
    F()N(1)L();

    /* add element after an element */
    elB = NEW(struct elem);
    elB->data = 2;
    DLIST_ADD_AFTER(*elA, elB, *elB);
    F()N(1)N(2)L();

    /* add element at the end of the list */
    elB = NEW(struct elem);
    (DLIST_ADD_BEFORE(head, elB, *elB))->data = 3;
    F()N(1)N(2)N(3)L();

    /* move an element to another position or another list */
    DLIST_MOVE_AFTER(head, *elB);
    F()N(3)N(1)N(2)L();

    DLIST_MOVE_BEFORE(head, *elB);
    F()N(1)N(2)N(3)L();

    /* works even if the position is the same */
    DLIST_MOVE_BEFORE(head, *elB);
    F()N(1)N(2)N(3)L();

    A_I(DLIST_FIRST(head)->data,==,1);
    A_I(DLIST_LAST(head)->data,==,3);

    DLIST_LOOP(head, el) {
        P("%d", el->data);
    }
    P(".");

    /* remove elements */
    DLIST_SAFE_RLOOP(head, el, el_safe) {
        if (el->me->data == 1)
        {
            DLIST_REMOVE(*el);
            FREE(el);
        }
    }
    F()N(2)N(3)L();

    /* delete list */
    DLIST_SAFE_LOOP(head, el, el_safe) {
        DLIST_REMOVE(*el);
        FREE(el);
    }
    F()L();

    OUT;
}

void test_plist() {
    IN;

    struct elem;
    struct list {
        struct elem *me;
        struct list *last, *next;
    } head, *inA, *inB, *in;
    struct elem {
        int data;
        struct list *list_data;
    } *elA, *elB, *el, *el_safe;
    
    /* initialization */
    DLIST_INIT(head);    
    A_I(DLIST_IS_EMPTY(head),!=,0);

    /* add element at beginning of list */
    elA = NEW(struct elem);
    elA->data = 1;
    inA = NEW(struct list);
    DLIST_PADD_AFTER(head, elA, inA, list_data);
    F()N(1)L();

    /* add element after an element */
    elB = NEW(struct elem);
    elB->data = 2;
    inB = NEW(struct list);
    DLIST_PADD_AFTER(*inA, elB, inB, list_data);
    F()N(1)N(2)L();

    /* add element at the end of the list */
    elB = NEW(struct elem);
    inB = NEW(struct list);
    (DLIST_PADD_BEFORE(head, elB, inB, list_data))->data = 3;
    F()N(1)N(2)N(3)L();

    /* move an element to another position or another list */
    DLIST_MOVE_AFTER(head, *inB);
    F()N(3)N(1)N(2)L();

    DLIST_MOVE_BEFORE(head, *inB);
    F()N(1)N(2)N(3)L();

    /* works even if the position is the same */
    DLIST_MOVE_BEFORE(head, *inB);
    F()N(1)N(2)N(3)L();

    A_I(DLIST_FIRST(head)->data,==,1);
    A_I(DLIST_LAST(head)->data,==,3);

    DLIST_LOOP(head, in) {
        P("%d", in->me->data);
    }
    P(".");
    DLIST_PLOOP(head, el, list_data) {
        P("%d", el->data);
    }
    P(".");

    /* remove elements */
    DLIST_SAFE_PLOOP(head, el, el_safe, list_data) {
        if (el->data == 2)
        {
            DLIST_REMOVE(*el->list_data);
            FREE(el->list_data);
            FREE(el);
        }
    }
    F()N(1)N(3)L();

    /* delete list */
    DLIST_SAFE_PLOOP(head, el, el_safe, list_data) {
        DLIST_REMOVE(*el->list_data);
        FREE(el->list_data);
        FREE(el);
    }
    F()L();

    OUT;
}

void test_mlist() {
    IN;

    struct elem {
        int data;
        struct list {
            struct list *last, *next;
            struct elem *me;
        } list_data;
    } *elA, *elB, *el, *el_safe;
    struct list head, *in;
    
    /* initialization */
    DLIST_INIT(head);
    A_I(DLIST_IS_EMPTY(head),!=,0);

    /* add element at beginning of list */
    elA = NEW(struct elem);
    elA->data = 1;
    DLIST_MADD_AFTER(head, elA, list_data);
    F()N(1)L();

    /* add element after an element */
    elB = NEW(struct elem);
    elB->data = 2;
    DLIST_MADD_AFTER(elA->list_data, elB, list_data);
    F()N(1)N(2)L();

    /* add element at the end of the list */
    elB = NEW(struct elem);
    (DLIST_MADD_BEFORE(head, elB, list_data))->data = 3;
    F()N(1)N(2)N(3)L();

    /* move an element to another position or another list */
    DLIST_MOVE_AFTER(head, elB->list_data);
    F()N(3)N(1)N(2)L();

    DLIST_MOVE_BEFORE(head, elB->list_data);
    F()N(1)N(2)N(3)L();

    /* works even if the position is the same */
    DLIST_MOVE_BEFORE(head, elB->list_data);
    F()N(1)N(2)N(3)L();

    A_I(DLIST_FIRST(head)->data,==,1);
    A_I(DLIST_LAST(head)->data,==,3);

    DLIST_LOOP(head, in) {
        P("%d", in->me->data);
    }
    P(".");
    DLIST_MLOOP(head, el, list_data) {
        P("%d", el->data);
    }
    P(".");

    /* remove elements */
    DLIST_SAFE_MLOOP(head, el, el_safe, list_data) {
        if (el->data != 2)
        {
            DLIST_REMOVE(el->list_data);
            FREE(el);
        }
    }
    F()N(2)L();

    /* delete list */
    DLIST_SAFE_MLOOP(head, el, el_safe, list_data) {
        DLIST_REMOVE(el->list_data);
        FREE(el);
    }
    F()L();

    OUT;
}

int main(int argc, char **argv)
{
    IN;
    test_new();
    test_list();
    test_ezlist();
    test_plist();
    test_mlist();
    OUT;
}

