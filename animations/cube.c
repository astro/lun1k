
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "libs/armmath.h"

float *zbuf;

typedef struct tri {
	float sx[3];
	float sy[3];
	float nx[3];
	float ny[3];
	float b[3];
	int xmin;
	int xmax;
	int ymin;
	int ymax;
} tri;

/* Stolen from: https://github.com/halcy/rasterizer/blob/master/models.h */
void rasterize_triangle(float vertices[3][3], int colors[3][3]) {
    // Status variables.
    tri t;
    float d1;
    float d2;
    float d3;
    float r;
    float g;
    float b;
    int x;
    int y;
    float z;
    int i;
    float l;

    // The actual rasterizer.

		
    // Backface cull
    if(
        (vertices[1][0] - vertices[0][0]) *
        (vertices[2][1] - vertices[0][1]) -
        (vertices[2][0] - vertices[0][0]) *
        (vertices[1][1] - vertices[0][1])
        < 0
        ) {
        printf("culled\n");
        return;
    }

    // Lines of the form: d = nx * ( x - sx ) + ny * ( y - sy )
    for( i = 0; i < 3; i++ ) {
        t.sx[i] = vertices[i][0];
        t.sy[i] = vertices[i][1];
    }

    // Normals
    for( i = 0; i < 3; i++ ) {
        t.nx[i] = -(t.sy[(i+1)%3] - t.sy[i]);
        t.ny[i] =  (t.sx[(i+1)%3] - t.sx[i]);
        l = sqrt( t.nx[i] * t.nx[i] + t.ny[i] * t.ny[i] );
        t.nx[i] /= l;
        t.ny[i] /= l;
    }

    // For barycentric coordinates
    for( i = 0; i < 3; i++ ) {
        t.b[i] = t.nx[i] * ( t.sx[(i+2)%3] - t.sx[i] ) + t.ny[i] * ( t.sy[(i+2)%3] - t.sy[i] );
    }

    // Bounding rectangles.
    t.xmin = floor( fmin( fmin( t.sx[0], t.sx[1] ), t.sx[2] ) );
    t.ymin = floor( fmin( fmin( t.sy[0], t.sy[1] ), t.sy[2] ) );
    t.xmax = ceil( fmax( fmax( t.sx[0], t.sx[1] ), t.sx[2] ) );
    t.ymax = ceil( fmax( fmax( t.sy[0], t.sy[1] ), t.sy[2] ) );

    // Clip and possibly reject.
    t.xmin = fmax(0, t.xmin);
    t.xmax = fmin(LED_WIDTH, t.xmax);
    t.ymin = fmax(0, t.ymin);
    t.ymax = fmin(LED_HEIGHT, t.ymax);

    if(t.ymin > t.ymax || t.xmin > t.xmax) {
        return;
    }
    // Draw pixels inside, if need be
    for( y = t.ymin; y < t.ymax; y++ ) {
        for( x = t.xmin; x < t.xmax; x++ ) {
            d1 = t.nx[0] * ( x - t.sx[0] ) + t.ny[0] * ( y - t.sy[0] );
            if( d1 >= 0 ) {
                d2 = t.nx[1] * ( x - t.sx[1] ) + t.ny[1] * ( y - t.sy[1] );
                if( d2 >= 0 ) {
                    d3 = t.nx[2] * ( x - t.sx[2] ) + t.ny[2] * ( y - t.sy[2] );
                    if( d3 >= 0 ) {
                        d1 /= t.b[0];
                        d2 /= t.b[1];
                        d3 /= t.b[2];

                        // Z test
                        z =
                            1.0f / vertices[0][2] * d2 +
                            1.0f / vertices[1][2] * d3 +
                            1.0f / vertices[2][2] * d1;

                        if( z > zbuf[y*LED_WIDTH+x] ) {
                            zbuf[y*LED_WIDTH+x] = z;

                            r =
                                colors[0][0] * d2 +
                                colors[1][0] * d3 +
                                colors[2][0] * d1;
                            g =
                                colors[0][1] * d2  +
                                colors[1][1] * d3  +
                                colors[2][1] * d1;
                            b =
                                colors[0][2] * d2 +
                                colors[1][2] * d3 +
                                colors[2][2] * d1;

                            setLedXY(x, y, r,g,b);
                        }
                    }
                }
            }
        }
    }
}

float projMat[4][4];

void matInit(void) {
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            projMat[i][j] = (i == j) ? 1 : 0;
}

void matMul(float a[4][4], float b[4][4]) {
    float c[4][4];
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            c[i][j] = 0;
            for(int k = 0; k < 4; k++)
                c[i][j] += a[i][k] * b[j][k];
        }
    }
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            a[i][j] = c[i][j];
        }
    }
}

void printMat(float a[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            printf("\t%4.2f", a[i][j]);
        }
        printf("\n");
    }
}

void matTranslate(float x, float y, float z) {
    float trans[4][4] = {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { x, y, z, 1 }
    };
    printf("matTranslate %.4f %.4f %.4f\n", x, y, z);
    printMat(trans);
    matMul(projMat, trans);
    printf("=\n");
    printMat(projMat);
}

void matRotateY(float a) {
    float rot[4][4] = {
        {  cos(a), 0, sin(a), 0 },
        {  0,      1, 0,      0 },
        { -sin(a), 0, cos(a), 0 },
        {  0,      0, 0,      1 }
    };
    printf("matRotateY %.4f\n", a);
    printMat(rot);
    matMul(projMat, rot);
    printf("=\n");
    printMat(projMat);
}

void draw_triangle(float vertices[3][3], int colors[3][3]) {
    /* apply camera matrix */
    /* perspective projection */
    float v[3][3];
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            v[i][j] = 0;
            for(int k = 0; k < 4; k++) {
                v[i][j] += vertices[i][j] * projMat[j][k];
            }
        }
        v[i][0] = (v[i][0] - LED_WIDTH / 2) / v[i][2] + LED_WIDTH / 2;
        v[i][1] = (v[i][1] - LED_HEIGHT / 2) / v[i][2] + LED_HEIGHT / 2;
        v[i][2] = v[i][2];
        printf("v[%i]: %.2f x %.2f x %.2f\n", i, v[i][0], v[i][1], v[i][2]);
    }
    rasterize_triangle(v, colors);
}

void draw_quad(float x1, float y1, float z1,
               float x2, float y2, float z2,
               float x3, float y3, float z3,
               float x4, float y4, float z4,
               uint8_t r, uint8_t g, uint8_t b) {
    float v1[3][3] = {
        { x1, y1, z1 },
        { x2, y2, z2 },
        { x3, y3, z3 }
    };
    int c[3][3] = {
        { r, g, b },
        { r, g, b },
        { r, g, b }
    };
    draw_triangle(v1, c);
    float v2[3][3] = {
        { x1, y1, z1 },
        { x3, y3, z3 },
        { x4, y4, z4 }
    };
    draw_triangle(v2, c);
}

static void init(void) {
    zbuf = malloc(LED_WIDTH * LED_HEIGHT * sizeof(*zbuf));
}

static void deinit(void) {
    free(zbuf);
}

static uint8_t tick(void) {
    /* clear */
    for(int i = 0; i < LED_WIDTH * LED_HEIGHT; i++)
        zbuf[i] = 0;
    for(int y = 0; y < LED_HEIGHT; y++)
        for(int x = 0; x < LED_WIDTH; x++)
            setLedXY(x, y, 0, 0, 0);

    matInit();
    matTranslate(0, 0, 1);
    matRotateY(M_PI * getSysTick() / 10000);

    /* Front side */
    draw_quad(-10, -10, -10,
              10, -10, -10,
              10, 10, -10,
              -10, 10, -10,
              255, 255, 255);
    /* Back side */
    draw_quad(10, -10, -10,
              -10, -10, 10,
              -10, 10, 10,
              10, 10, 10,
              255, 255, 0);
    /* Top side */
    draw_quad(-10, -10, -10,
              -10, -10, 10,
              10, -10, 10,
              10, -10, -10,
              255, 0, 255);
    /* Bottom side */
    draw_quad(-10, 10, -10,
              10, 10, -10,
              10, 10, 10,
              -10, 10, 10,
              0, 255, 255);

    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
        registerAnimation("Cube", init,tick,deinit, 0, 1000);
}
