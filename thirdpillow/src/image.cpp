#include "image.h"

image::image() {
    this->width = 0;
    this->height = 0;
}

image::image(int width, int height) {
    this->width = width;
    this->height = height;
    this->data = new color[width * height];
}

int image::get_width() {
    return this->width;
}

int image::get_height() {
    return this->height;
}

color image::get_color(int x, int y) {
    int index = putils::get_index_2d(x, y, this->width);
    return this->data[index];
}

void image::set_color(int x, int y, color value) {
    int index = putils::get_index_2d(x, y, this->width);
    this->data[index] = value;
}

//http://stackoverflow.com/questions/2693631/read-ppm-file-and-store-it-in-an-array-coded-with-c
void image::from_ppm_raw(char* filename) {
    printf("loading image from %s\n", filename);
    typedef struct {
        unsigned char red, green, blue;
    } PPMPixel;

    typedef struct {
        int x, y;
        PPMPixel* data;
    } PPMImage;

    char buff[16];
    PPMImage* img;
    FILE* fp;
    int c, rgb_comp_color;
    fp = fopen(filename, "rb");
    if (!fp) {
        printf("unable to load ppm from %s\n", filename);
        exit(1);
    }
    if (!fgets(buff, sizeof(buff), fp)) {
        printf("image format error: %s\n", filename);
        exit(1);
    }
    if (buff[0] != 'P' || buff[1] != '6') {
        printf("image format must be P6: %s\n", filename);
        exit(1);
    }
    img = (PPMImage*)malloc(sizeof(PPMImage));
    if (!img) {
        printf("unable to allocate memory for PPMImage: %s\n", filename);
    }
    c = getc(fp);
    while (c == '#') {
        while (getc(fp) != '\n');
        c = getc(fp);
    }
    ungetc(c, fp);
    if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
        printf("invalid image size error: %s\n", filename);
        exit(1);
    }
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
        printf("invalid rgb component: %s\n", filename);
        exit(1);
    }
    if (rgb_comp_color != 255) { //RGB_COMPONENT_COLOR = 255
        printf("%s does not have 8-bit components\n", filename);
        exit(1);
    }
    while (fgetc(fp) != '\n');
    img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));
    if (!img) {
        printf("unable to allocate memory for PPMPixel: %s\n", filename);
        exit(1);
    }
    if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
        printf("error loading image data: %s\n", filename);
    }
    this->width = img->x;
    this->height = img->y;
    this->data = new color[this->width * this->height];
    for (int i = 0; i < this->width; i++) {
        for (int j = 0; j < this->height; j++) {
            int index = putils::get_index_2d(i, j, this->width);
            unsigned char c_r = (unsigned char)img->data[index].red;
            unsigned char c_g = (unsigned char)img->data[index].green;
            unsigned char c_b = (unsigned char)img->data[index].blue;
            color* pixel_c = new color(c_r, c_g, c_b, 255);
            this->data[index] = *pixel_c;
        }
    }
    free(img->data);
    free(img);
}

image::~image() {
}