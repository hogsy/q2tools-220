//
// llwolib.c: library for loading triangles from a Lightwave triangle file
//

#include <stdio.h>
#include "cmdlib.h"
#include "mathlib.h"
#include "4data.h"

#define MAXPOINTS 2000
#define MAXPOLYS  2000

static struct {
    float p[3];
} pnt[MAXPOINTS];
static struct {
    short pl[3];
} ply[MAXPOLYS];

static char trashcan[10000];
char buffer[1000];
static long counter;
static union {
    char c[4];
    float f;
    long l;
} buffer4;
static union {
    char c[2];
    short i;
} buffer2;

static FILE *lwo;
static short numpoints, numpolys;

static long lflip(long x) {
    union {
        char b[4];
        long l;
    } in, out;

    in.l     = x;
    out.b[0] = in.b[3];
    out.b[1] = in.b[2];
    out.b[2] = in.b[1];
    out.b[3] = in.b[0];

    return out.l;
}

static float fflip(float x) {
    union {
        char b[4];
        float l;
    } in, out;

    in.l     = x;
    out.b[0] = in.b[3];
    out.b[1] = in.b[2];
    out.b[2] = in.b[1];
    out.b[3] = in.b[0];

    return out.l;
}

static int sflip(short x) {
    union {
        char b[2];
        short i;
    } in, out;

    in.i     = x;
    out.b[0] = in.b[1];
    out.b[1] = in.b[0];

    return out.i;
}

static void skipchunk() {
    long chunksize;

    fread(buffer4.c, 4, 1, lwo);
    counter -= 8;
    chunksize = lflip(buffer4.l);
    fread(trashcan, chunksize, 1, lwo);
    counter -= chunksize;
};

static void getpoints() {
    long chunksize;
    short i, j;

    fread(buffer4.c, 4, 1, lwo);
    counter -= 8;
    chunksize = lflip(buffer4.l);
    counter -= chunksize;
    numpoints = chunksize / 12;
    if (numpoints > MAXPOINTS) {
        fprintf(stderr, "reader: Too many points!!!");
        exit(0);
    }
    for (i = 0; i < numpoints; i++) {
        for (j = 0; j < 3; j++) {
            fread(buffer4.c, 4, 1, lwo);
            pnt[i].p[j] = fflip(buffer4.f);
        }
    }
}

static void getpolys() {
    short temp, i, j;
    short polypoints;
    long chunksize;

    fread(buffer4.c, 4, 1, lwo);
    counter -= 8;
    chunksize = lflip(buffer4.l);
    counter -= chunksize;
    numpolys = 0;
    for (i = 0; i < chunksize;) {
        fread(buffer2.c, 2, 1, lwo);
        polypoints = sflip(buffer2.i);
        if (polypoints != 3) {
            fprintf(stderr, "reader: Not a triangle!!!");
            exit(0);
        }
        i += 10;
        for (j = 0; j < 3; j++) {
            fread(buffer2.c, 2, 1, lwo);
            temp                = sflip(buffer2.i);
            ply[numpolys].pl[j] = temp;
        }
        fread(buffer2.c, 2, 1, lwo);
        numpolys++;
        if (numpolys > MAXPOLYS) {
            fprintf(stderr, "reader: Too many polygons!!!\n");
            exit(0);
        }
    }
}

void LoadLWOTriangleList(char *filename, triangle_t **pptri, int32_t *numtriangles) {
    int32_t i, j;
    triangle_t *ptri;

    if ((lwo = fopen(filename, "rb")) == 0) {
        fprintf(stderr, "reader: could not open file '%s'\n", filename);
        exit(0);
    }

    fread(buffer4.c, 4, 1, lwo);
    fread(buffer4.c, 4, 1, lwo);
    counter = lflip(buffer4.l) - 4;
    fread(buffer4.c, 4, 1, lwo);
    while (counter > 0) {
        fread(buffer4.c, 4, 1, lwo);
        if (!strncmp(buffer4.c, "PNTS", 4)) {
            getpoints();
        } else {
            if (!strncmp(buffer4.c, "POLS", 4)) {
                getpolys();
            } else {
                skipchunk();
            }
        }
    }
    fclose(lwo);

    ptri   = malloc(MAXTRIANGLES * sizeof(triangle_t));
    *pptri = ptri;

    for (i = 0; i < numpolys; i++) {
        for (j = 0; j < 3; j++) {
            ptri[i].verts[j][0] = pnt[ply[i].pl[j]].p[0];
            ptri[i].verts[j][1] = pnt[ply[i].pl[j]].p[2];
            ptri[i].verts[j][2] = pnt[ply[i].pl[j]].p[1];
        }
    }

    *numtriangles = numpolys;
}
