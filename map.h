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

#ifndef __MAP_H__
#define __MAP_H__

#include <stdbool.h>

struct map_pair;
struct map {
    struct map_pair *elements;  /**< array of elements */
    unsigned int size_elements; /**< array size */
    unsigned int num_elements;  /**< number of used elements */
    bool sorted;                /**< sorted state */
};

int map_init(struct map *m, unsigned int init_size);
int map_clear(struct map *m);

int map_add(struct map *m, const char *key, const char *value);
void map_sort(struct map *m);
const char *map_lookup(struct map *m, const char *key);

#endif /* __MAP_H__ */
