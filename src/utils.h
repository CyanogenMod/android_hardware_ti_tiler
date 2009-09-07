/*
 * utils.h
 *
 * Utility definitions for the Memory Interface for TI OMAP processors.
 *
 * Copyright (C) 2008-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

//#define __DEBUG__
//#define __DEBUG_ENTRY__
//#define __DEBUG_ASSERT__

/* ---------- Generic Macros Used in Macros ---------- */

/* statement begin */
#define S_ do
/* statement end */
#define _S while (0)
/* expression begin */
#define E_ (
/* expression end */
#define _E )

/* ---------- Generic Debug Print Macros ---------- */

/**
 * Use as: 
 *    P("val is %d", 5);
 *    ==> val is 5
 *    DP("val is %d", 15);
 *    ==> val is 5 at test.c:56:main()
 */
/* debug print (fmt must be a literal); adds new-line. */
#ifdef __DEBUG__
#define P(fmt, ...) S_ { fprintf(stdout, fmt "\n", ##__VA_ARGS__); fflush(stdout); } _S
#else
#define P(fmt, ...)
#endif

/* debug print with context information (fmt must be a literal) */
#define DP(fmt, ...) P(fmt " at %s(" __FILE__ ":%d)", ##__VA_ARGS__, __FUNCTION__, __LINE__)

/* ---------- Program Flow Debug Macros ---------- */

/**
 * Use as: 
 *    int get5() {
 *      IN;
 *      return R_I(5);
 *    }
 *    void check(int i) {
 *      IN;
 *      if (i == 5) { RET; return; }
 *      OUT;
 *    }
 *    void main() {
 *      IN;
 *      check(get5());
 *      check(2);
 *      OUT;
 *    }
 *    ==>
 *    in main(test.c:11)
 *    in get5(test.c:2)
 *    out(5) at get5(test.c:3)
 *    in check(test.c:6)
 *    out() at check(test.c:7)
 *    in check(test.c:6)
 *    out check(test.c:8)
 *    out main(test.c:14)
 */

#ifdef __DEBUG_ENTRY__
/* function entry */
#define IN P("in %s(" __FILE__ ":%d)", __FUNCTION__, __LINE__)
/* function exit */
#define OUT P("out %s(" __FILE__ ":%d)", __FUNCTION__, __LINE__)
/* function abort (return;)  Use as { RET; return; } */
#define RET DP("out() ")
/* generic function return */
#define R(val,type,fmt) E_ { type __val__ = (type) val; DP("out(" fmt ")", __val__); __val__; } _E
#else
#define IN
#define OUT
#define RET
#define R(val,type,fmt) (val)
#endif

/* integer return */
#define R_I(val) R(val,int,"%d")
/* pointer return */
#define R_P(val) R(val,void *,"%p")
/* long return */
#define R_UP(val) R(val,long,"0x%lx")

/* ---------- Assertion Debug Macros ---------- */

/**
 * Use as: 
 *     int i = 5;
 *     // int j = i * 5;
 *     int j = A_I(i,==,3) * 5;
 *     // if (i > 3) P("bad")
 *     if (NOT_I(i,<=,3)) P("bad")
 *     P("j is %d", j);
 *     ==> assert: i (=5) !== 3 at test.c:56:main()
 *     assert: i (=5) !<= 3 at test.c:58:main()
 *     j is 25
 *  
 */
/* generic assertion check, A returns the value of exp, CHK return void */
#ifdef __DEBUG_ASSERT__
#define A(exp,cmp,val,type,fmt) E_ { \
    type __exp__ = (type) (exp); type __val__ = (type) (val); \
    if (!(__exp__ cmp __val__)) DP("assert: %s (=" fmt ") !" #cmp " " fmt, #exp, __exp__, __val__); \
    __exp__; \
} _E
#define CHK(exp,cmp,val,type,fmt) S_ { \
    type __exp__ = (type) (exp); type __val__ = (type) (val); \
    if (!(__exp__ cmp __val__)) DP("assert: %s (=" fmt ") !" #cmp " " fmt, #exp, __exp__, __val__); \
} _S
#else
#define A(exp,cmp,val,type,fmt) (exp)
#define CHK(exp,cmp,val,type,fmt)
#endif

/* typed assertions */
#define A_I(exp,cmp,val) A(exp,cmp,val,int,"%d")
#define A_L(exp,cmp,val) A(exp,cmp,val,long,"%ld")
#define A_P(exp,cmp,val) A(exp,cmp,val,void *,"%p")
#define CHK_I(exp,cmp,val) CHK(exp,cmp,val,int,"%d")
#define CHK_L(exp,cmp,val) CHK(exp,cmp,val,long,"%ld")
#define CHK_P(exp,cmp,val) CHK(exp,cmp,val,void *,"%p")

/* generic assertion check, returns true iff assertion fails */
#ifdef __DEBUG_ASSERT__
#define NOT(exp,cmp,val,type,fmt) E_ { \
    type __exp__ = (type) (exp); type __val__ = (type) (val); \
    if (!(__exp__ cmp __val__)) DP("assert: %s (=" fmt ") !" #cmp " " fmt, #exp, __exp__, __val__); \
    !(__exp__ cmp __val__); \
} _E
#else
#define NOT(exp,cmp,val,type,fmt) (!((exp) cmp (val)))
#endif

/* typed assertion checks */
#define NOT_I(exp,cmp,val) NOT(exp,cmp,val,int,"%d")
#define NOT_L(exp,cmp,val) NOT(exp,cmp,val,long,"%ld")
#define NOT_P(exp,cmp,val) NOT(exp,cmp,val,void *,"%p")

/* error propagation macros - these macros ensure evaluation of the expression
   even if there was a prior error */

/* new error is accumulated into error */
#define ERR_ADD(err, exp) S_ { int __error__ = A_I(exp,==,0); err = err ? err : __error__; } _S
/* new error overwrites old error */
#define ERR_OVW(err, exp) S_ { int __error__ = A_I(exp,==,0); err = __error__ ? __error__ : err; } _S

/* allocation macro */
#define NEW(type)    (type*)calloc(1, sizeof(type))
#define NEWN(type,n) (type*)calloc(n, sizeof(type))

/* free variable and set it to NULL */
#define FREE(var)    S_ { free(var); var = NULL; } _S

/* clear variable */
#define ZERO(var)    memset(&(var), 0, sizeof(var))

/* 

    DLIST macros facilitate double-linked lists in a generic fashion.  They
    require a list info structure (separate from the element structure, although
    the info structure can be a member of the element structure).  The info
    structure must contain (at least) the following 3 members:

    *next, *last: as pointers to the info structure.  These are the next and
                  previous elements in the list.
    *me: as a pointer to the element structure.  This is how we get to the
         element.

    The list element structure __MAY__ contain the info structure as a member,
    or a pointer to the info structure if it is desired to be kept separately.
    In such cases macros are provided to add the elements directly to the list,
    and automatically set up the info structure fields correctly.  You can also
    iterate through the elements of the list using a pointer to the elements
    itself, as the list structure can be obtained for each element.

    Otherwise, macros are provided to manipulate list info structures and
    link them in any shape or form into double linked lists.  This allows
    having NULL values as members of such lists.

   :NOTE: If you use a macro with a field argument, you must not have NULL
   elements because the field of any element must be/point to the list
   info structure */

/*
  Usage:
 
  DLIST macros are designed for preallocating the list info structures, e.g. in
  an array.  This is why most macros take a list info structure, and not a
  pointer to a list info structure.
 
  Basic linked list consists of element structure and list info structure
 
    struct elem {
        int data;
    } *elA, *elB;
    struct list {
        struct elem *me;
        struct list *last, *next;
    } head, *inA, *inB, *in;
    
    DLIST_INIT(head);              // initialization -> ()
    DLIST_IS_EMPTY(head) == TRUE;  // emptiness check
 
    // add element at beginning of list -> (1)
    elA = NEW(struct elem);
    elA->data = 1;
    inA = NEW(struct list);
    DLIST_ADD_AFTER(head, elA, *inA);
    
    // add before an element -> (2, 1)
    elB = NEW(struct elem);
    elB->data = 2;
    inB = NEW(struct list);
    DLIST_ADD_BEFORE(*inA, elB, *inB);

    // move an element to another position or another list -> (1, 2)
    DLIST_MOVE_BEFORE(head, *inB);
 
    // works even if the position is the same -> (1, 2)
    DLIST_MOVE_BEFORE(head, *inB);

    // get first and last elements
    DLIST_FIRST(head) == elA;
    DLIST_LAST(head) == elB;
 
    // loop through elements
    DLIST_LOOP(head, in) {
        P("%d", in->me->data);
    }
 
    // remove element -> (2)
    DLIST_REMOVE(*inA);
    FREE(elA);
    FREE(inA);
 
    // delete list
    DLIST_SAFE_LOOP(head, in, inA) {
        DLIST_REMOVE(*in);
        FREE(in->me);
        FREE(in);
    }

  You can combine the element and list info structures to create an easy list,
  but you still need to specify both element and info structure while adding
  elements.
 
    struct elem {
        int data;
        struct elem *me, *last, *next;
    } head, *el, *elA, *elB;
 
    DLIST_INIT(head);              // initialization -> ()
    DLIST_IS_EMPTY(head) == TRUE;  // emptiness check
 
    // add element at beginning of list -> (1)
    elA = NEW(struct elem);
    elA->data = 1;
    DLIST_ADD_AFTER(head, elA, *elA);
    
    // add before an element -> (2, 1)
    elB = NEW(struct elem);
    elB->data = 2;
    DLIST_ADD_BEFORE(*elA, elB, *elB);

    // move an element to another position or another list -> (1, 2)
    DLIST_MOVE_BEFORE(head, *elB);
 
    // works even if the position is the same -> (1, 2)
    DLIST_MOVE_BEFORE(head, *elB);

    // get first and last elements
    DLIST_FIRST(head) == elA;
    DLIST_LAST(head) == elB;
 
    // loop through elements
    DLIST_LOOP(head, el) {
        P("%d", el->data);
    }
 
    // remove element -> (2)
    DLIST_REMOVE(*elA);
    FREE(elA);
 
    // delete list
    DLIST_SAFE_LOOP(head, el, elA) {
        DLIST_REMOVE(*el);
        FREE(el);
    }

  A better way to get to the list structure from the element structure is to
  enclose a pointer the list structure in the element structure.  This allows
  getting to the next/previous element from the element itself.
 
    struct elem;
    struct list {
        struct elem *me;
        struct list *last, *next;
    } head, *inA, *inB, *in;
    struct elem {
        int data;
        struct list *list_data;
    } *elA, *elB, *el;
 
    // or
 
    struct elem {
        int data;
        struct list {
            struct elem *me;
            struct list *last, *next;
        } *list_data;
    } *elA, *elB, *el;
    struct list head, *inA, *inB, *in;
 
    DLIST_INIT(head);              // initialization -> ()
    DLIST_IS_EMPTY(head) == TRUE;  // emptiness check
 
    // add element at beginning of list -> (1)
    elA = NEW(struct elem);
    elA->data = 1;
    inA = NEW(struct list);
    DLIST_PADD_AFTER(head, elA, inA, list_data);
    
    // add before an element -> (2, 1)
    elB = NEW(struct elem);
    elB->data = 2;
    inB = NEW(struct list);
    DLIST_PADD_BEFORE(*elA, elB, inB, list_data);

    // move an element to another position or another list -> (1, 2)
    DLIST_MOVE_BEFORE(head, *inB);
 
    // works even if the position is the same -> (1, 2)
    DLIST_MOVE_BEFORE(head, *inB);

    // get first and last elements
    DLIST_FIRST(head) == elA;
    DLIST_LAST(head) == elB;
 
    // loop through elements
    DLIST_LOOP(head, in) {
        P("%d", in->me->data);
    }
    DLIST_PLOOP(head, el, list_data) {
        P("%d", el->data);
    }
 
    // remove element
    DLIST_REMOVE(*inA);
    FREE(inA);
    FREE(elA);

    // delete list
    DLIST_SAFE_PLOOP(head, el, elA, list_data) {
        DLIST_REMOVE(*el->list_data);
        FREE(el->list_data);
        FREE(el);
    }

  Lastly, you can include the list data in the element structure itself.
 
    struct elem {
        int data;
        struct list {
            struct list *last, *next;
            struct elem *me;
        } list_data;
    } *elA, *elB, *el;
    struct list head, *in;
    
    DLIST_INIT(head);              // initialization -> ()
    DLIST_IS_EMPTY(head) == TRUE;  // emptiness check
 
    // add element at beginning of list -> (1)
    elA = NEW(struct elem);
    elA->data = 1;
    DLIST_MADD_AFTER(head, elA, list_data);
    
    // add before an element -> (2, 1)
    elB = NEW(struct elem);
    elB->data = 2;
    DLIST_PADD_BEFORE(elA->list_data, elB, list_data);

    // move an element to another position or another list -> (1, 2)
    DLIST_MOVE_BEFORE(head, elB->list_data);
 
    // works even if the position is the same -> (1, 2)
    DLIST_MOVE_BEFORE(head, elB->list_data);

    // get first and last elements
    DLIST_FIRST(head) == elA;
    DLIST_LAST(head) == elB;
 
    // loop through elements
    DLIST_LOOP(head, in) {
        P("%d", in->me->data);
    }
    DLIST_MLOOP(head, el, list_data) {
        P("%d", el->data);
    }
  
    // remove element
    DLIST_REMOVE(elA->list_data);
    FREE(elA);

    // delete list
    DLIST_SAFE_MLOOP(head, el, elA, list_data) {
        DLIST_REMOVE(el->list_data);
        FREE(el);
    }

 */

/* -- internal (generic direction) macros -- */
#define DLIST_move__(base, info, next, last) ((info).last = &base)->next = ((info).next = (base).next)->last = &(info)
#define DLIST_padd__(base, pInfo, pElem, pField, next, last) ((pInfo)->last = &base)->next = ((pInfo)->next = (base).next)->last = pElem->pField = pInfo
#define DLIST_loop__(head, pInfo, next)                           for (pInfo=(head).next; pInfo != &(head); pInfo = (pInfo)->next)
#define DLIST_ploop__(head, pElem, pField, next)                  for (pElem=(head).next->me; pElem; pElem = (pElem)->pField->next->me)
#define DLIST_mloop__(head, pElem, mField, next)                  for (pElem=(head).next->me; pElem; pElem = (pElem)->mField.next->me)
#define DLIST_safe_loop__(head, pInfo, pInfo_safe, next)          for (pInfo=(head).next; pInfo != &(head); pInfo = pInfo_safe) if ((pInfo_safe = (pInfo)->next) || 1)
#define DLIST_safe_ploop__(head, pElem, pElem_safe, pField, next) for (pElem=(head).next->me; pElem; pElem = pElem_safe) if ((pElem_safe = (pElem)->pField->next->me) || 1)
#define DLIST_safe_mloop__(head, pElem, pElem_safe, mField, next) for (pElem=(head).next->me; pElem; pElem = pElem_safe) if ((pElem_safe = (pElem)->mField.next->me) || 1)

#define DLIST_IS_EMPTY(head) ((head).next == &(head))

/* Adds the element (referred to by the info structure) before/or after another
   element (or list header) (base). */

#define DLIST_ADD_AFTER(base, pElem, info)     (DLIST_move__(base, info, next, last))->me = pElem
#define DLIST_ADD_BEFORE(base, pElem, info)    (DLIST_move__(base, info, last, next))->me = pElem

/* Adds the element (referred to by pElem pointer) along with its info
   structure (referred to by pInfo pointer) before/or after an element or
   list header (base).  It also sets up the list structure header to point to
   the element as well as the element's field to point back to the list info
   structure. */
#define DLIST_PADD_BEFORE(base, pElem, pInfo, pField) (DLIST_padd__(base, pInfo, pElem, pField, last, next))->me = pElem
#define DLIST_PADD_AFTER(base, pElem, pInfo, pField)  (DLIST_padd__(base, pInfo, pElem, pField, next, last))->me = pElem

/* Adds the element (referred to by pElem pointer) before/or after an element or
   list header (base).  It also sets up the list structure header (which is a
   member of the element's structure) to point to the element. */
#define DLIST_MADD_BEFORE(base, pElem, mField) (DLIST_move__(base, pElem->mField, last, next))->me = pElem
#define DLIST_MADD_AFTER(base, pElem, mField)  (DLIST_move__(base, pElem->mField, next, last))->me = pElem

/* Removes the element (referred to by the info structure) from its current
   list.  This requires that the element is a part of a list.

   :NOTE: the info structure will still think that it belongs to the list it
   used to belong to. However, the old list will not contain this element any
   longer. You want to discard the info/element after this call.  Otherwise,
   you can use one of the MOVE macros to also add the item to another list,
   or another place in the same list. */
#define DLIST_REMOVE(info) ((info).last->next = (info).next)->last = (info).last

/* Initializes the list header (to an empty list) */
#define DLIST_INIT(head) do { (head).me = NULL; (head).next = (head).last = &(head); } while (0)

/* These functions move an element (referred to by the info structure) before
   or after another element (or the list head).
   :NOTE: This logic also works for moving an element after/before itself. */
#define DLIST_MOVE_AFTER(base, info)    do { DLIST_REMOVE(info); DLIST_move__(base, info, next, last); } while (0)
#define DLIST_MOVE_BEFORE(base, info)   do { DLIST_REMOVE(info); DLIST_move__(base, info, last, next); } while (0)

/* Loops behave syntactically as a for() statement.  They traverse the loop
   variable from the 1st to the last element (or in the opposite direction in
   case of RLOOP). There are 3 flavors of loops depending on the type of the 
   loop variable.

   DLIST_LOOP's loop variable is a pointer to the list info structure.  You can
   get to the element by using the 'me' member.  Nonetheless, this loop
   construct allows having NULL elements in the list. 

   DLIST_MLOOP's loop variable is a pointer to a list element.  mField is the
   field of the element containing the list info structure.  Naturally, this
   list cannot have NULL elements.

   DLIST_PLOOP's loop variable is also a pointer to a list element.  Use this
   construct if the element contains a pointer to the list info structure
   instead of embedding it directly into the element structure.

*/ 
#define DLIST_LOOP(head, pInfo)           DLIST_loop__(head, pInfo, next)
#define DLIST_MLOOP(head, pElem, mField)  DLIST_mloop__(head, pElem, mField, next)
#define DLIST_PLOOP(head, pElem, pField)  DLIST_ploop__(head, pElem, pField, next)
#define DLIST_RLOOP(head, pInfo)          DLIST_loop__(head, pInfo, last)
#define DLIST_RMLOOP(head, pElem, mField) DLIST_mloop__(head, pElem, mField, last)
#define DLIST_RPLOOP(head, pElem, pField) DLIST_ploop__(head, pElem, pField, last)

/* Safe loops are like ordinary loops, but they allow removal of the current
   element from the list. They require an extra loop variable that holds the
   value of the next element in case the current element is moved/removed. */ 
#define DLIST_SAFE_LOOP(head, pInfo, pInfo_safe)            DLIST_safe_loop__(head, pInfo, pInfo_safe, next)
#define DLIST_SAFE_MLOOP(head, pElem, pElem_safe, mField)  DLIST_safe_mloop__(head, pElem, pElem_safe, mField, next)
#define DLIST_SAFE_PLOOP(head, pElem, pElem_safe, pField)  DLIST_safe_ploop__(head, pElem, pElem_safe, pField, next)
#define DLIST_SAFE_RLOOP(head, pInfo, pInfo_safe)           DLIST_safe_loop__(head, pInfo, pInfo_safe, last)
#define DLIST_SAFE_RMLOOP(head, pElem, pElem_safe, mField) DLIST_safe_mloop__(head, pElem, pElem_safe, mField, last)
#define DLIST_SAFE_RPLOOP(head, pElem, pElem_safe, pField) DLIST_safe_ploop__(head, pElem, pElem_safe, pField, last)

/* returns the first element of a list */
#define DLIST_FIRST(head) (head).next->me
/* returns the last element of a list */
#define DLIST_LAST(head) (head).last->me

#endif

