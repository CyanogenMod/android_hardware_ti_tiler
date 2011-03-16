/*
 * testlib.h
 *
 * Unit test interface API.
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _TESTLIB_H_
#define _TESTLIB_H_

/* error type definitions */
#define TESTLIB_UNAVAILABLE -65378
#define TESTLIB_INVALID        -1

#define T(test) ++i; \
    if (!id || i == id) printf("TEST #% 3d - %s\n", i, #test); \
    if (i == id) { \
        printf("TEST_DESC - "); \
        fflush(stdout); \
        return __internal__TestLib_Report(test); \
    }

/* test run function that must be defined from the test app */

/**
 * Runs a specified test by id, or lists all test cases.  This
 * function uses the TESTS macros, and defines each T(test) to
 * run a test starting from id == 1, and then return the result.
 *
 * @author a0194118 (9/7/2009)
 *
 * @param id   Test case id, or 0 if only listing test cases
 *
 * @return Summary result: TEST_RESULT_OK, FAIL, INVALID or
 *         UNAVAILABLE.
 */
#define TESTS_ \
    int __internal__TestLib_DoList(int id) { int i = 0;

#define _TESTS \
    return TESTLIB_INVALID; }

#define DEFINE_TESTS(TESTS) TESTS_ TESTS _TESTS

/* internal function prototypes and defines */
extern int __internal__TestLib_Report(int res);
extern void __internal__TestLib_NullFn(void *ptr);

#define nullfn __internal__TestLib_NullFn

/**
 * Parses argument list, prints usage on error, lists test
 * cases, runs tests and reports results.
 *
 * @author a0194118 (9/12/2009)
 *
 * @param argc      Number of test arguments
 * @param argv      Test argument array
 * @param init_fn   Initialization function of void fn(void *).
 *                  This is called before the testing.
 * @param exit_fn   Deinit function of void fn(void *).  This is
 *                  done after the testing concludes.
 * @param ptr       Custom pointer that is passed into the
 *                  initialization functions.
 *
 * @return # of test cases failed, 0 on success, -1 if no tests
 *         were run because of an error or a list request.
 */
int TestLib_Run(int argc, char **argv, void(*init_fn)(void *),
                void(*exit_fn)(void *), void *ptr);

#endif
