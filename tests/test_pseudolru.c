/*
 
Copyright (c) 2011, Willem-Hendrik Thiart
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of its contributors may not be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WILLEM-HENDRIK THIART BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"

#include "pseudolru.h"


static int __mem_compare(
    const void *e1,
    const void *e2
)
{
    return e1 - e2;
}

void TestPseudoLRU_init_is_empty(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    CuAssertTrue(tc, pseudolru_is_empty(me));
    CuAssertTrue(tc, 0 == pseudolru_count(me));
    pseudolru_free(me);
}

void TestPseudoLRU_init_means_peek_is_empty(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    CuAssertTrue(tc, NULL == pseudolru_peek(me));
    pseudolru_free(me);
}

void TestPseudoLRU_post_put_means_peek_exists(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 10, (void *) 10);
    CuAssertTrue(tc, NULL != pseudolru_peek(me));
    pseudolru_free(me);
}

void TestPseudoLRU_post_put_means_not_empty(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 10, (void *) 10);

    CuAssertTrue(tc, !pseudolru_is_empty(me));
    CuAssertTrue(tc, 1 == pseudolru_count(me));
    pseudolru_free(me);
}

void TestPseudoLRU_put_x_plus_others_then_get_x(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 5, (void *) 1);
    pseudolru_put(me, (void *) 1, (void *) 1);
    pseudolru_put(me, (void *) 3, (void *) 1);
    pseudolru_put(me, (void *) 10, (void *) 1);

    pseudolru_put(me, (void *) 15, (void *) 2);
    pseudolru_put(me, (void *) 2, (void *) 3);
    pseudolru_put(me, (void *) 4, (void *) 4);

    CuAssertTrue(tc, 7 == pseudolru_count(me));
    CuAssertTrue(tc, 3 == (unsigned long) pseudolru_get(me, (void *) 2));
    pseudolru_free(me);
}

void TestPseudoLRU_put_4_get_2(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 10, (void *) 1);
    pseudolru_put(me, (void *) 15, (void *) 2);
    pseudolru_put(me, (void *) 2, (void *) 3);
    pseudolru_put(me, (void *) 4, (void *) 4);

    CuAssertTrue(tc, 3 == (unsigned long) pseudolru_get(me, (void *) 2));
    CuAssertTrue(tc, 4 == (unsigned long) pseudolru_get(me, (void *) 4));

    pseudolru_free(me);
}

void TestPseudoLRU_get_puts_at_root(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 10, (void *) 1);
    pseudolru_put(me, (void *) 11, (void *) 4);
    CuAssertTrue(tc, 4 == (unsigned long) pseudolru_peek(me));

    pseudolru_get(me, (void *) 10);
    CuAssertTrue(tc, 1 == (unsigned long) pseudolru_peek(me));

    pseudolru_free(me);
}

void TestPseudoLRU_put_does_not_dupe(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 10, (void *) 1);
    pseudolru_put(me, (void *) 10, (void *) 4);
    CuAssertTrue(tc, 1 == pseudolru_count(me));
    pseudolru_free(me);
}

void TestPseudoLRU_put_dupe_doesnt_overwrite(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 10, (void *) 1);
    pseudolru_put(me, (void *) 10, (void *) 4);
    CuAssertTrue(tc, 1 == (unsigned long) pseudolru_get(me, (void *) 10));

    pseudolru_free(me);
}

void TestPseudoLRU_get_nonexistant_fails(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 10, (void *) 1);
    pseudolru_put(me, (void *) 15, (void *) 2);
    pseudolru_put(me, (void *) 2, (void *) 3);
    pseudolru_put(me, (void *) 4, (void *) 4);

    CuAssertTrue(tc, 0 == (unsigned long) pseudolru_get(me, (void *) 5));
    CuAssertTrue(tc, 0 == (unsigned long) pseudolru_get(me, (void *) 678));

    pseudolru_free(me);
}

void TestPseudoLRU_get_when_empty_fails(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    CuAssertTrue(tc, !pseudolru_get(me, (void *) 15));

    pseudolru_free(me);
}

void TestPseudoLRU_remove_when_empty_fails(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    CuAssertTrue(tc, !pseudolru_remove(me, (void *) 15));

    pseudolru_free(me);
}

void TestPseudoLRU_remove_successully_if_exists(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 10, (void *) 1);
    pseudolru_put(me, (void *) 15, (void *) 2);
    pseudolru_put(me, (void *) 2, (void *) 3);
    pseudolru_remove(me, (void *) 15);

    CuAssertTrue(tc, 2 == pseudolru_count(me));
    CuAssertTrue(tc, 3 == (unsigned long) pseudolru_get(me, (void *) 2));
    CuAssertTrue(tc, 1 == (unsigned long) pseudolru_get(me, (void *) 10));

    pseudolru_free(me);
}

void TestPseudoLRU_remove_returns_value(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 15, (void *) 2);
    CuAssertTrue(tc, 2 == (unsigned long) pseudolru_remove(me, (void *) 15));

    pseudolru_free(me);
}

void TestPseudoLRU_pop_lru_gets_unused_item(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 5, (void *) 128);
    pseudolru_put(me, (void *) 1, (void *) 6);
    pseudolru_put(me, (void *) 3, (void *) 12);
    pseudolru_put(me, (void *) 10, (void *) 9);
    pseudolru_put(me, (void *) 15, (void *) 2);
    pseudolru_put(me, (void *) 2, (void *) 3);
    pseudolru_put(me, (void *) 4, (void *) 4);

    unsigned long lru;

    lru = (unsigned long) pseudolru_pop_lru(me);
    CuAssertTrue(tc, 6 == pseudolru_count(me));
    CuAssertTrue(tc, 4 != lru);
    CuAssertTrue(tc, 3 != lru);
    CuAssertTrue(tc, 2 != lru);
    CuAssertTrue(tc, 9 != lru);
    CuAssertTrue(tc, 12 != lru);
    pseudolru_free(me);
}

void TestPseudoLRU_pop_lru_gets_unused_item2(
    CuTest * tc
)
{
    pseudolru_t *me;

    me = pseudolru_new(__mem_compare);

    pseudolru_put(me, (void *) 5, (void *) 128);
    pseudolru_put(me, (void *) 1, (void *) 6);
    pseudolru_put(me, (void *) 3, (void *) 12);
    pseudolru_put(me, (void *) 10, (void *) 9);
    pseudolru_put(me, (void *) 15, (void *) 2);
    pseudolru_put(me, (void *) 0, (void *) 7);
    pseudolru_put(me, (void *) 4, (void *) 4);

    pseudolru_get(me, (void *) 3);
    pseudolru_get(me, (void *) 0);
    pseudolru_get(me, (void *) 4);

    unsigned long lru;

    CuAssertTrue(tc, 7 == pseudolru_count(me));

    lru = (unsigned long) pseudolru_pop_lru(me);

    CuAssertTrue(tc, 6 == pseudolru_count(me));
    CuAssertTrue(tc, 12 != lru);
    CuAssertTrue(tc, 7 != lru);
    CuAssertTrue(tc, 4 != lru);
    CuAssertTrue(tc, 3 != lru);
    CuAssertTrue(tc, 9 != lru);
    pseudolru_free(me);
}
