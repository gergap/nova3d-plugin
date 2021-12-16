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

#include "stringutils.h"

#include <string.h>

/* like Perl's chomp() function */
void chomp(char *str)
{
    size_t len = strlen(str);
    if (str[len - 1] == '\n') {
        str[len - 1] = 0;
    }
}

/** Better version of strncat which ensures the string is terminated.
 * @param dst The destination buffer.
 * @param src The source string to copy. Must be zero terminated.
 * @param dst_len Length of \c dst in bytes.
 * @return Number of bytes copied on success excluding the terminating null byte, -1 if the string
 * was truncated.
 */
int strlcpy(char *dst, const char *src, size_t dst_len)
{
    size_t len = strlen(src);
    if (len > (dst_len - 1)) {
        memcpy(dst, src, dst_len - 1);
        dst[dst_len - 1] = 0; /* terminate string */
        return -1;
    }
    memcpy(dst, src, len + 1);

    return len;
}

/* My version of POSIX basename function. It removes file extension and directories.
 * Normal basename is not available on Windows.
 * E.g. "foo/bar.ext" -> "bar".
 * This function modifies the filename and returns a pointer to the basename inside filename.
 * Create a copy before calling this function.
 */
char *my_basename(char *filename)
{
    char *ext = strrchr(filename, '.');
    if (ext == NULL) return NULL;
    *ext = 0; /* cut off extension */

    /* find last slash */
    ext = strrchr(filename, '/');
    if (ext == NULL) return filename; /* there is no separator */
    ext++;

    return ext;
}

