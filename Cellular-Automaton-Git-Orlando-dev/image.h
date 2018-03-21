#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED



typedef struct Image {
    unsigned width;
    unsigned height;
    char filename[32];
    unsigned char *data;
} Image;

typedef struct Color {
    unsigned char R, G, B, A;
} Color;


void destroyImage(Image img);

void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height);

void grid2PNG(Grid g, const char *filename);

Image initImage(unsigned width, unsigned height, const char* filename);

void saveImage(struct Image img);

void writePixel(Image *img, unsigned x, unsigned y, Color c);

Color packColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A);

Color hueGradient(double min, double max, double target, char type);

#endif // IMAGE_H_INCLUDED
