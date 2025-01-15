#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PBMImage* createPBM(int width, int height) {
    PBMImage *img = (PBMImage*) malloc(sizeof(PBMImage));
    if (!img) return NULL;

    img->width = width;
    img->height = height;

    img->pixels = (int**) malloc(height * sizeof(int*));
    if (!img->pixels) {
        free(img);
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        img->pixels[i] = (int*) malloc(width * sizeof(int));
        if (!img->pixels[i]) {
            // desaloca tudo caso falhe
            for (int j = 0; j < i; j++) {
                free(img->pixels[j]);
            }
            free(img->pixels);
            free(img);
            return NULL;
        }
        // inicializa com 0
        memset(img->pixels[i], 0, width * sizeof(int));
    }

    return img;
}

void freePBM(PBMImage *img) {
    if (!img) return;
    for (int i = 0; i < img->height; i++) {
        free(img->pixels[i]);
    }
    free(img->pixels);
    free(img);
}

bool writePBM(const PBMImage *img, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return false;

    // Cabeçalho
    fprintf(fp, "P1\n");
    fprintf(fp, "%d %d\n", img->width, img->height);

    // Pixels
    // Aqui podemos escrever cada linha com espaços ou tudo junto
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            fprintf(fp, "%d ", img->pixels[y][x]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    return true;
}

static bool skipComments(FILE *fp) {
    // Pula comentários (#...) e espaços
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '#') {
            // linha de comentário, pula até \n
            while ((c = fgetc(fp)) != EOF && c != '\n') {}
        }
        else {
            ungetc(c, fp);
            break;
        }
    }
    return (c != EOF);
}

PBMImage* readPBM(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    // Ler "P1"
    char magic[3];
    if (!fgets(magic, sizeof(magic), fp)) {
        fclose(fp);
        return NULL;
    }

    if (strncmp(magic, "P1", 2) != 0) {
        fclose(fp);
        return NULL;
    }

    // Pula comentários e lê largura e altura
    if (!skipComments(fp)) {
        fclose(fp);
        return NULL;
    }

    int width, height;
    if (fscanf(fp, "%d %d", &width, &height) != 2) {
        fclose(fp);
        return NULL;
    }

    // valida
    if (width <= 0 || height <= 0) {
        fclose(fp);
        return NULL;
    }

    // Aloca
    PBMImage *img = createPBM(width, height);
    if (!img) {
        fclose(fp);
        return NULL;
    }

    // Lê pixels (0 ou 1)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int v;
            if (fscanf(fp, "%d", &v) != 1) {
                freePBM(img);
                fclose(fp);
                return NULL;
            }
            if (v != 0 && v != 1) {
                freePBM(img);
                fclose(fp);
                return NULL;
            }
            img->pixels[y][x] = v;
        }
    }

    fclose(fp);
    return img;
}

void setPixel(PBMImage *img, int x, int y, int value) {
    if (!img) return;
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) return;
    img->pixels[y][x] = value;
}

int getPixel(const PBMImage *img, int x, int y) {
    if (!img) return 0;
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) return 0;
    return img->pixels[y][x];
}
