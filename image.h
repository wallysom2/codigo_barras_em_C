#ifndef IMAGE_H
#define IMAGE_H

#include <stdbool.h>

/* Estrutura para guardar uma imagem PBM */
typedef struct {
    int width;
    int height;
    int **pixels; // 0 ou 1
} PBMImage;

/**
 * Cria uma imagem PBM vazia (pixels inicializados com 0) com as dimensões fornecidas.
 * Retorna ponteiro para PBMImage alocada dinamicamente.
 */
PBMImage* createPBM(int width, int height);

/**
 * Libera a memória de uma imagem PBM criada com createPBM.
 */
void freePBM(PBMImage *img);

/**
 * Escreve uma imagem PBM em um arquivo. Retorna true se tiver sucesso.
 */
bool writePBM(const PBMImage *img, const char *filename);

/**
 * Lê uma imagem PBM de um arquivo. Retorna ponteiro para PBMImage ou NULL em caso de erro.
 */
PBMImage* readPBM(const char *filename);

/**
 * Seta o pixel (x, y) da imagem para o valor dado (0 ou 1).
 */
void setPixel(PBMImage *img, int x, int y, int value);

/**
 * Retorna o valor do pixel (x, y) da imagem (0 ou 1).
 */
int getPixel(const PBMImage *img, int x, int y);

#endif
