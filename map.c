/*****************************************************************************
 * Nova3d-Plugin
 * Copyright (C) 2021  Gerhard Gappmeier <gappy1502@gmx.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.EOF
 *****************************************************************************/

#include "map.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

struct map_pair {
    char *key;
    char *value;
};

static int map_pair_init(struct map_pair *el, const char *name, const char *value)
{
    el->key = strdup(name);
    if (el->key == NULL) return -1;

    el->value = strdup(value);
    if (el->value == NULL) {
        free(el->key);
        el->key = NULL;
        return -1;
    }

    return 0;
}

static void map_pair_clear(struct map_pair *el)
{
    if (el->key) free(el->key);
    if (el->value) free(el->value);

    el->key = NULL;
    el->value = NULL;
}

/* Comparison function for qsort and bsearch.
 * Compares the pair's key which should be used as sorting criteria.
 */
static int map_pair_compare(const void *a, const void *b)
{
    const struct map_pair *pair_a = a;
    const struct map_pair *pair_b = b;

    return strcmp(pair_a->key, pair_b->key);
}

int map_init(struct map *m, unsigned int init_size)
{
    m->elements = malloc(init_size * sizeof(struct map_pair));
    if (m->elements == NULL) return -1;
    m->size_elements = init_size;
    m->num_elements = 0;

    return 0;
}

int map_clear(struct map *m)
{
    unsigned int i;

    for (i = 0; i < m->num_elements; ++i) {
        map_pair_clear(&m->elements[i]);
    }
    free(m->elements);
    m->elements = NULL;
    m->num_elements = 0;
    m->size_elements = 0;
    m->sorted = false;

    return 0;
}

int map_add(struct map *m, const char *key, const char *value)
{
    int ret;
    if (m->num_elements >= m->size_elements) {
        /* we could do a realloc here, but we don't need this if init_size is correct */
        return -1; /* let it fail just in case */
    }

    ret = map_pair_init(&m->elements[m->num_elements], key, value);
    if (ret < 0) return ret;

    m->num_elements++;
    m->sorted = false;

    return 0;
}

void map_sort(struct map *m)
{
    qsort(m->elements, m->num_elements, sizeof(*m->elements), map_pair_compare);
    m->sorted = true;
}

/**
 * Returns the value for the given key.
 *
 * @param m map to use
 * @param key Key to search for.
 *
 * @return the according value or NULL if not found.
 */
const char *map_lookup(struct map *m, const char *key)
{
    const struct map_pair search_key = { (char*)key, NULL };
    const struct map_pair *result;

    if (!m->sorted) {
        fprintf(stderr, "error: you need to sort the array before using lookup\n");
        return NULL;
    }

    result = bsearch(&search_key, m->elements, m->num_elements, sizeof(*m->elements), map_pair_compare);
    if (result == NULL) return NULL;

    return result->value;
}

