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

struct map_el {
    char *key;
    char *value;
};

struct map {
    struct map_el **elements;
    unsigned int num_elements;
};

int map_init(struct map *m, unsigned int init_size);
int map_cleanup(struct map *m);

int map_add(struct map *m, const char *key, const char *value);
const char *map_lookup(struct map *m, const char *key, const char *value);

#endif /* __MAP_H__ */

