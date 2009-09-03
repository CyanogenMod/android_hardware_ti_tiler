
/*
 * Copyright (C) Texas Instruments - http://www.ti.com/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
/* ====================================================================
*             Texas Instruments OMAP(TM) Platform Software
* (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied.
* ==================================================================== */

/** OMX_TI_Common.h
 *  The LCML header file contains the definitions used by both the
 *  application and the component to access common items.
 */

#ifndef __OMX_TI_COMMON_H__
#define __OMX_TI_COMMON_H__

/* OMX_TI_SEVERITYTYPE enumeration is used to indicate severity level of errors returned by TI OpenMax components. 
   Critcal	Requires reboot/reset DSP
   Severe	Have to unload components and free memory and try again
   Major	Can be handled without unloading the component
   Minor	Essentially informational 
*/
typedef enum OMX_TI_SEVERITYTYPE {
    OMX_TI_ErrorCritical=1,
    OMX_TI_ErrorSevere,
    OMX_TI_ErrorMajor,
    OMX_TI_ErrorMinor
} OMX_TI_SEVERITYTYPE;

/* Round integer (i) up to a power-of 2 (n2n) */
#define ROUNDUP2N(i, n2n) (((i) + (n2n) - 1) & ~ ((n2n) - 1))
/* Round integer (i) up to (bits) power-of 2 */
#define ROUNDUP2NB(i, bits) ROUNDUP2N(i, 1 << (bits))
/* Count how many (n)-s does it take to reach or surpass integer (i) */
#define COUNTUPN(i, n) (((i) + (n) - 1) / (n))
/* Count how many (bits) power-of 2-s does it take to reach or surpass integer (i) */
#define COUNTUP2NB(i, bits) (((i) + (1 << (bits)) - 1) >> (bits))



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
/* File EOF */

