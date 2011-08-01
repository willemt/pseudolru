/*
 * =====================================================================================
 *
 *       Filename:  pseudo_lru.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/11 22:27:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Willem Thiart
 *        Company:  
 *
 * =====================================================================================
 */


typedef struct
{
    void *root;
    int (
    *cmp
    )   (
    const void *,
    const void *
    );
    int count;
} pseudolru_t;

pseudolru_t *pseudolru_initalloc(
    int (*cmp) (const void *,
                const void *)
);

void pseudolru_free(
    pseudolru_t * st
);

int pseudolru_is_empty(
    pseudolru_t * st
);

void *pseudolru_get(
    pseudolru_t * st,
    const void *key
);

void *pseudolru_remove(
    pseudolru_t * st,
    const void *key
);

void *pseudolru_pop_lru(
    pseudolru_t * dt
);

int pseudolru_count(
    pseudolru_t * st
);

void *pseudolru_peek(
    pseudolru_t * st
);

void pseudolru_put(
    pseudolru_t * st,
    void *key,
    void *value
);
