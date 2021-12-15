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
#include <time.h>
#include <unistd.h>
#include "map.h"

#ifdef _MSC_VER
/* MSVC compatibility stuff */
# define PATH_MAX MAX_PATH
#else
# include <limits.h>
#endif

struct map g_map;

#define APPID "Chitubox Nova3d-Plugin"
#define THROW() goto error_handler
#define CATCH error_handler

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

/** Better version of strncat which ensures the string is terminated.
 * @param dst The destination buffer.
 * @param src The source string to copy. Must be zero terminated.
 * @param dst_len Length of \c dst in bytes.
 * @return Number of bytes copied on success excluding the terminating null byte, -1 if the string was truncated.
 */
static int strlcpy(char *dst, const char *src, size_t dst_len)
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

/** parses GCODE header parameter from Chitubox.
 * Syntax: ";<param>:<value>"
 */
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

/** Parse Chitubox GCode */
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
            if (strcmp(line, ";END_GCODE_BEGIN") == 0) {
                state = PARSE_ENDCODE;
            }
            break;
        case PARSE_ENDCODE:
            if (strcmp(line, ";END_GCODE_END") == 0) {
                state = PARSE_DONE;
            }
            break;
        case PARSE_DONE:
            break;
        }
    } while (!feof(f));

    fclose(f);

    /* delete file */
    unlink(filename);

    map_sort(&g_map);

    return 0;
}

/** returns parsed parameter as unsigned int.
 * @param name Parameter name as defined in Chitubox GCode header.
 * @return Parameter value, or 0 not parameter not found.
 */
static unsigned int param_uint(const char *name)
{
    const char *value = map_lookup(&g_map, name);
    if (value == NULL) return 0;

    return strtoul(value, NULL, 10);
}

/** returns parsed parameter as unsigned int.
 * @param name Parameter name as defined in Chitubox GCode header.
 * @return Parameter value, or 0 not parameter not found.
 */
static double param_float(const char *name)
{
    const char *value = map_lookup(&g_map, name);
    if (value == NULL) return 0;

    return strtod(value, NULL);
}

/** Create Nova3d slice.conf  */
int create_slice_conf(const char *dir)
{
    char filename[PATH_MAX];
    FILE *f;
    time_t now = time(NULL);
    const char *timestamp = ctime(&now);
    int ret;
    float ppm = 19.324f; /* pixel per mm ? */

    ret = snprintf(filename, sizeof(filename), "%s/slice.conf", dir);
    if (ret >= (int)sizeof(filename)) {
        fprintf(stderr, "error: filename too long.\n");
        return -1;
    }

    f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "error: could not open file '%s' for writing.\n", filename);
        return -1;
    }

    fprintf(f, "# Generated by %s %s", APPID, timestamp);
    fprintf(f, "# conf version 1.0\n\n");
    fprintf(f, "xppm                    = %5.3f\n", ppm);
    fprintf(f, "yppm                    = %5.3f\n", ppm);
    fprintf(f, "xres                    = %u\n", param_uint("resolutionX"));
    fprintf(f, "yres                    = %u\n", param_uint("resolutionY"));
    fprintf(f, "thickness               = %.3f\n", param_float("layerHeight"));
    fprintf(f, "layers_num              = %u\n", param_uint("totalLayer"));
    fprintf(f, "head_layers_num         = %u\n", param_uint("bottomLayerCount"));
    fprintf(f, "layers_expo_ms          = %u\n", param_uint("normalExposureTime") * 1000);
    fprintf(f, "head_layers_expo_ms     = %u\n", param_uint("bottomLayerExposureTime") * 1000);
    fprintf(f, "wait_before_expo_ms     = %u\n", param_uint("lightOffTime") * 1000);
    fprintf(f, "lift_distance           = %u\n", param_uint("normalLayerLiftHeight"));
    fprintf(f, "lift_up_speed           = %.1f\n", param_float("normalLayerLiftSpeed"));
    fprintf(f, "lift_down_speed         = %.1f\n", param_float("normalDropSpeed"));
    fprintf(f, "lift_when_finished      = %u\n", 80);
    fclose(f);

    return 0;
}

/* My version of a basename function. It removes file extension and directories.
 * E.g. "foo/bar.ext" -> "bar".
 * This function modifies the filename and returns a pointer to the basename inside filename.
 * Create a copy before calling this function.
 */
static char *my_basename(char *filename)
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

/** Create Nova3d GCode */
int create_gcode(const char *tmpdir, const char *projectname)
{
    char filename[PATH_MAX];
    FILE *f;
    time_t now = time(NULL);
    const char *timestamp = ctime(&now);
    unsigned int i;
    unsigned int wait_time;
    unsigned int num_slices        = param_uint("totalLayer");
    unsigned int num_bottom_layers = param_uint("bottomLayerCount");
    unsigned int speed_up          = param_uint("normalLayerLiftSpeed");
    unsigned int speed_down        = param_uint("normalDropSpeed");
    float liftHeight               = param_float("normalLayerLiftHeight");
    float layerHeight              = param_float("layerHeight");
    int ret;

    /* create GCode filename */
    ret = snprintf(filename, sizeof(filename), "%s/%s.gcode", tmpdir, projectname);
    if (ret >= (int)sizeof(filename)) {
        fprintf(stderr, "error: filename too long.\n");
        return -1;
    }

    f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "error: could not open file '%s' for writing.\n", filename);
        return -1;
    }

    fprintf(f, "; Generated by %s %s", APPID, timestamp);
    fprintf(f, ";(****Build and Slicing Parameters****)\n");
    fprintf(f, ";(Pix per mm X            = 19.324 )\n");
    fprintf(f, ";(Pix per mm Y            = 19.324 )\n");
    fprintf(f, ";(X Resolution            = %u )\n", param_uint("resolutionX"));
    fprintf(f, ";(Y Resolution            = %u )\n", param_uint("resolutionY"));
    fprintf(f, ";(Layer Thickness         = %.3f mm )\n", param_float("layerHeight"));
    fprintf(f, ";(Layer Time              = %u ms )\n", param_uint("normalExposureTime") * 1000);
    fprintf(f, ";(Render Outlines         = False\n");
    fprintf(f, ";(Outline Width Inset     = 2\n");
    fprintf(f, ";(Outline Width Outset    = 0\n");
    fprintf(f, ";(Bottom Layers Time      = %u ms )\n", param_uint("bottomLayerExposureTime") * 1000);
    fprintf(f, ";(Number of Bottom Layers = %u )\n", param_uint("bottomLayerCount"));
    fprintf(f, ";(Blanking Layer Time     = %u ms )\n", param_uint("lightOffTime") * 1000);
    fprintf(f, ";(Build Direction         = Bottom_Up)\n");
    fprintf(f, ";(Lift Distance           = %u mm )\n", param_uint("liftHeight"));
    fprintf(f, ";(Slide/Tilt Value        = 0)\n");
    fprintf(f, ";(Anti Aliasing           = True)\n");
    fprintf(f, ";(Use Mainlift GCode Tab  = False)\n");
    fprintf(f, ";(Anti Aliasing Value     = 2 )\n");
    fprintf(f, ";(Z Lift Feed Rate        = %.1f mm/s )\n", param_float("normalLayerLiftSpeed"));
    fprintf(f, ";(Z Bottom Lift Feed Rate = %.1f mm/s )\n", param_float("bottomLayerLiftSpeed"));
    fprintf(f, ";(Z Lift Retract Rate     = 100 mm/s )\n");
    fprintf(f, ";(Flip X                  = True)\n");
    fprintf(f, ";(Flip Y                  = True)\n");
    fprintf(f, ";Number of Slices         = %u\n", num_slices);
    fprintf(f, ";(****Machine Configuration ******)\n");
    fprintf(f, ";(Platform X Size         = %.1fmm )\n", param_float("machineX"));
    fprintf(f, ";(Platform Y Size         = %.1fmm )\n", param_float("machineY"));
    fprintf(f, ";(Platform Z Size         = %.1fmm )\n", param_float("machineZ"));
    fprintf(f, ";(Max X Feedrate          = 200mm/s )\n");
    fprintf(f, ";(Max Y Feedrate          = 200mm/s )\n");
    fprintf(f, ";(Max Z Feedrate          = 200mm/s )\n");
    fprintf(f, ";(Machine Type            = UV_LCD)\n\n");

    /* gcode start code */
    fprintf(f, "G28\n");
    fprintf(f, "G21 ;Set units to be mm\n");
    fprintf(f, "G91 ;Relative Positioning\n");
    fprintf(f, "M17 ;Enable motors\n");
    fprintf(f, "<Slice> Blank\n");
    fprintf(f, "M106 S0\n");
    fprintf(f, "\n");

    /* slices */
    for (i = 0; i < num_slices; ++i) {
        if (i < num_bottom_layers) {
            wait_time = param_uint("bottomLayerExposureTime");
        } else {
            wait_time = param_uint("normalExposureTime");
        }
        wait_time *= 1000; /* ms */
        fprintf(f, ";<Slice> %u\n", i);             /**< image/slice index */
        fprintf(f, "M106 S255\n");                  /**< enable UV light */
        fprintf(f, ";<Delay> %u\n", wait_time);     /**< wait exposuretime [ms] */
        fprintf(f, "M106 S0\n");                    /**< disable UV light */
        fprintf(f, ";<Slice> Blank\n");             /**< show blank image */
        fprintf(f, "G1 Z%.3f F%u\n", liftHeight, speed_up);    /**< move up */
        fprintf(f, "G1 Z-%.3f F%u\n", liftHeight - layerHeight, speed_down); /**< move down */
        fprintf(f, ";<Delay> 6800\n\n");            /**< wait 6.8s (always, why?) */
    }

    /* gcode end code */
    fprintf(f, "M18 ;Disable Motors\n");
    fprintf(f, "M106 SO\n");
    fprintf(f, "G1 Z80\n");
    fprintf(f, ";<Completed>\n");

    fclose(f);

    return 0;
}

int rename_png_files(const char *tmpdir, const char *projectname)
{
    unsigned int num_slices = param_uint("totalLayer");
    unsigned int i;
    char filename1[PATH_MAX];
    char filename2[PATH_MAX];
    char fmt[32];
    int digits;
    int ret;

    if (num_slices < 10) {
        digits = 1;
    } else if (num_slices < 100) {
        digits = 2;
    } else if (num_slices < 1000) {
        digits = 3;
    } else if (num_slices < 10000) {
        digits = 4;
    } else {
        digits = 5;
    }

    /* create format string with correct padding */
    snprintf(fmt, sizeof(fmt), "%%s/%%s%%0%uu.png", digits);

    for (i = 0; i < num_slices; ++i) {
        snprintf(filename1, sizeof(filename1), "%s/%u.png", tmpdir, i + 1);
        snprintf(filename2, sizeof(filename2), fmt, tmpdir, projectname, i);

        ret = rename(filename1, filename2);
        if (ret != 0) {
            fprintf(stderr, "error: failed to rename file '%s' -> '%s'.\n", filename1, filename2);
            return ret;
        }
    }

    return 0;
}

/**
 * Creates the Nova3d files like created by NovaMaker 1.4.4.
 *
 * @param tmpdir The Chitubox temporary directory. This temp directory contains
 *   the input files. We also create our files in this dir. It will get zipped
 *   by Chitubox after conversion.
 * @param ofilename The output filename that was chosen by the user. This contains
 *   the file extension (.CWS) that we configured in plugin.json.
 *
 * @return Zero on success.
 */
int generate_novamaker_files(const char *tmpdir, const char *ofilename)
{
    int ret = 0;
    char basename[PATH_MAX];
    const char *projectname;

    /* extract project name from ofilename */
    ret = strlcpy(basename, ofilename, sizeof(basename));
    if (ret < 0) {
        fprintf(stderr, "error: filename too long.\n");
        return -1;
    }
    projectname = my_basename(basename);

    ret = create_slice_conf(tmpdir);
    if (ret != 0) return ret;

    ret = create_gcode(tmpdir, projectname);
    if (ret != 0) return ret;

    ret = rename_png_files(tmpdir, projectname);
    if (ret != 0) return ret;

    return ret;
}

int main(int argc, char *argv[])
{
    const char *tmpdir;
    const char *ofilename;
    char ifilename[PATH_MAX];
    int ret;

    if (argc != 3) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    tmpdir = argv[1];
    ofilename = argv[2];
    ret = snprintf(ifilename, sizeof(ifilename), "%s/run.gcode", tmpdir);
    if (ret >= (int)sizeof(ifilename)) {
        fprintf(stderr, "error: filename too long.\n");
        exit(EXIT_FAILURE);
    }

    map_init(&g_map, 50);
    ret = parse_gcode(ifilename);
    if (ret != 0) {
        fprintf(stderr, "error: failed to parse GCODE.\n");
        THROW();
    }

    ret = generate_novamaker_files(tmpdir, ofilename);
    if (ret != 0) {
        fprintf(stderr, "error: failed to generate NovaMaker files.\n");
        THROW();
    }

CATCH:
    map_clear(&g_map);

    if (ret != 0) exit(EXIT_FAILURE);

    return 0;
}


