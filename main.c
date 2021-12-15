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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

#ifdef _MSC_VER
/* MSVC compatibility stuff */
# define PATH_MAX MAX_PATH
#else
# include <limits.h>
#endif

struct map g_map;

void usage(const char *appname)
{
    printf("Usage: %s <input-dir> <output-filename>\n", appname);
}

/* like Perl's chomp() function */
static void chomp(char *str)
{
    size_t len = strlen(str);
    if (str[len - 1] == '\n') {
        str[len - 1] = 0;
    }
}

static int parse_param(char *line)
{
    const char *name;
    const char *value;
    /* ignore comment char */
    line++;

    name = strtok(line, ":");
    if (name == NULL) return -1;

    value = strtok(NULL, ":");
    if (value == NULL) return -1;

    return map_add(&g_map, name, value);
}

int parse_gcode(const char *filename)
{
    char *ret;
    FILE *f;
    char line[256];
    enum parsing_state {
        PARSE_HEADER = 0,
        PARSE_STARTCODE,
        PARSE_SLICE,
        PARSE_ENDCODE,
        PARSE_DONE
    };
    enum parsing_state state = PARSE_HEADER;

    f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "error: could not open '%s'.\n", filename);
        exit(EXIT_FAILURE);
    }

    do {
        ret = fgets(line, sizeof(line), f);
        if (ret == NULL) break;
        chomp(line);

        switch (state) {
        case PARSE_HEADER:
            parse_param(line);
            if (strcmp(line, ";START_GCODE_BEGIN") == 0) {
                state = PARSE_STARTCODE;
            }
            break;
        case PARSE_STARTCODE:
            if (strcmp(line, ";START_GCODE_END") == 0) {
                state = PARSE_SLICE;
            }
            break;
        case PARSE_SLICE:
            break;
        case PARSE_ENDCODE:
            break;

        case PARSE_DONE:
            break;
        }


    } while(!feof(f));

    fclose(f);

    return 0;
}

int main(int argc, char *argv[])
{
    const char *idir;
    const char *ofilename;
    char ifilename[PATH_MAX];
    int ret;

    if (argc != 3) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    idir = argv[1];
    ofilename = argv[2];
    ret = snprintf(ifilename, sizeof(ifilename), "%s/run.gcode", idir);
    if (ret >= sizeof(ifilename)) {
        fprintf(stderr, "error: filename too long.\n");
        exit(EXIT_FAILURE);
    }

    ret = parse_gcode(ifilename);

    return 0;
}


