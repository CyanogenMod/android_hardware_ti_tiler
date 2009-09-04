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
#define P(fmt, ...) S_ { fprintf(stdout, fmt "\n", __VA_ARGS__); fflush(stdout); } _S
#else
#define P(fmt, ...)
#endif

/* debug print with context information (fmt must be a literal) */
#define DP(fmt, ...) P(fmt " at (" __FILE__ ":%d:%s())", __VA_ARGS__, __LINE__, __FUNCTION__)

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
 *    in test.c:main()
 *    in test.c:get5()
 *    out(5) at test.c:3:get5()
 *    in test.c:check()
 *    out() at test.c:7:check()
 *    in test.c:check()
 *    out test.c:check()
 *    out test.c:14:main()
 */

#ifdef __DEBUG_ENTRY__
/* function entry */
#define IN P("in " __FILE__ ":%s()", __FUNCTION__)
/* function exit */
#define OUT P("out " __FILE__ ":%s()", __FUNCTION__)
/* function abort (return;)  Use as { RET; return; } */
#define RET DP("out() ")
/* generic function return */
#define R(val,type,fmt) E_ { type i = (type) val; DP("out(" fmt ")", i); i; } _E
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
#define R_UP(val) R(val,long,"%lx")

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
/* generic assertion check, returns the value of exp */
#ifdef __DEBUG_ASSERT__
#define A(exp,cmp,val,type,fmt) E_ { \
    type i = (type) (exp); type v = (type) (val); \
    if (!(i cmp v)) DP("assert: " #exp " (=" fmt ") !" #cmp " " fmt, i, v); \
    i; \
} _E
#else
#define A(exp,cmp,val,type,fmt) (exp)
#endif

/* typed assertions */
#define A_I(exp,cmp,val) A(exp,cmp,val,int,"%d")
#define A_L(exp,cmp,val) A(exp,cmp,val,long,"%ld")
#define A_P(exp,cmp,val) A(exp,cmp,val,void *,"%p")


/* generic assertion check, returns true iff assertion fails */
#ifdef __DEBUG_ASSERT__
#define NOT(exp,cmp,val,type,fmt) E_ { \
    type i = (type) (exp); type v = (type) (val); \
    if (!(i cmp v)) DP("assert: " #exp " (=" fmt ") !" #cmp " " fmt, i, v); \
    !(i cmp v); \
} _E
#else
#define NOT(exp,cmp,val,type,fmt) (!((exp) cmp (val)))
#endif

/* typed assertion checks */
#define NOT_I(exp,cmp,val) NOT(exp,cmp,val,int,"%d")
#define NOT_L(exp,cmp,val) NOT(exp,cmp,val,long,"%ld")
#define NOT_P(exp,cmp,val) NOT(exp,cmp,val,void *,"%p")



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

