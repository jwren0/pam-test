#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint32_t Color;

// The file to output an image to
#define OUTFILE "out.pam"

// The image height and width
#define IMAGE_HEIGHT 128
#define IMAGE_WIDTH  128

// Converting into appropriate format
#define RGBA(r, g, b, a) \
      ((r) << 0) \
    | ((g) << 8) \
    | ((b) << 16) \
    | ((a) << 24)

#define RGB(r, g, b) RGBA((r), (g), (b), 255)

// Get channels
#define GET_RED(color)   ((color) >>  0) & 0xff
#define GET_GREEN(color) ((color) >>  8) & 0xff
#define GET_BLUE(color)  ((color) >> 16) & 0xff
#define GET_ALPHA(color) ((color) >> 24) & 0xff

// Clear alpha channel
#define CLEAR_ALPHA(color) color & ~(0xff << 24)

// Colors
#define COLOR_RED   RGB(255, 0, 0)
#define COLOR_GREEN RGB(0, 255, 0)
#define COLOR_BLUE  RGB(0, 0, 255)
#define COLOR_BLACK RGB(0, 0, 0)
#define COLOR_WHITE RGB(255, 255, 255)

// The pixels
Color image[IMAGE_HEIGHT][IMAGE_WIDTH];

// Sold color
void image_solid(Color color) {
    for (size_t i = 0; i < IMAGE_HEIGHT; i++) {
        for (size_t j = 0; j < IMAGE_WIDTH; j++) {
            image[i][j] = color;
        }
    }
}

// Fade towards the top
void image_fade_top(Color color) {
    double fade = 0;
    color = CLEAR_ALPHA(color);

    for (size_t i = 0; i < IMAGE_HEIGHT; i++) {
        for (size_t j = 0; j < IMAGE_WIDTH; j++) {
            fade = ((double) i)/IMAGE_HEIGHT;
            image[i][j] = color | ((Color) (255*fade)) << 24;
        }
    }
}

// Fade towards the bottom
void image_fade_bottom(Color color) {
    double fade = 0;
    color = CLEAR_ALPHA(color);

    for (size_t i = 0; i < IMAGE_HEIGHT; i++) {
        for (size_t j = 0; j < IMAGE_WIDTH; j++) {
            fade = 1 - ((double) i)/IMAGE_HEIGHT;
            image[i][j] = color | ((Color) (255*fade)) << 24;
        }
    }
}

// Fade from one color to another
void image_fade_between(Color a, Color b) {
    int ar = GET_RED(a);
    int ag = GET_GREEN(a);
    int ab = GET_BLUE(a);
    int aa = GET_ALPHA(a);

    int dr = ar - ((int) GET_RED(b));
    int dg = ag - ((int) GET_GREEN(b));
    int db = ab - ((int) GET_BLUE(b));
    int da = aa - ((int) GET_ALPHA(b));

    double fade;

    for (size_t i = 0; i < IMAGE_HEIGHT; i++) {
        for (size_t j = 0; j < IMAGE_WIDTH; j++) {
            fade = ((double) i/IMAGE_HEIGHT);

            image[i][j] = RGBA(
                (Color) ar - (int) (fade * dr),
                (Color) ag - (int) (fade * dg),
                (Color) ab - (int) (fade * db),
                (Color) aa - (int) (fade * da)
            );
        }
    }
}

// Save the image in PAM format
void save_pam(const char *path) {
    FILE *fp = fopen(path, "w");

    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Write the PAM header
    fprintf(fp,
        "P7\n"
        "WIDTH %u\n"
        "HEIGHT %u\n"
        "DEPTH 4\n"
        "MAXVAL 255\n"
        "TUPLTYPE RGB_ALPHA\n"
        "ENDHDR\n",
        IMAGE_WIDTH, IMAGE_HEIGHT
    );

    // Write the pixels
    for (size_t i = 0; i < IMAGE_HEIGHT; i++) {
        for (size_t j = 0; j < IMAGE_WIDTH; j++) {
            fwrite(&(image[i][j]), sizeof(Color), 1, fp);
        }
    }

    // Close the file
    if (fclose(fp) != 0) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    // Set the image to a solid color
    //image_solid(COLOR_RED);

    // Use a fade effect
    //image_fade_top(COLOR_GREEN);
    //image_fade_bottom(COLOR_BLUE);
    //image_fade_between(COLOR_GREEN, COLOR_BLUE);
    image_fade_between(
        RGBA(120, 255, 255, 30),
        RGBA(230, 120, 255, 255)
    );

    // Save the image
    save_pam(OUTFILE);
}
