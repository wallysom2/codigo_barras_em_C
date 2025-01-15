#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ean8.h"
#include "image.h"

/**
 * Tenta localizar o padrão EAN-8 em uma linha horizontal da imagem.
 * Supondo que a imagem contenha, em alguma linha, as 67 "barras" (0 ou 1)
 * sem interrupções (ou ao menos, um bloco de 67*m repetições).
 *
 * Abordagem simples: 
 *   - Descobrimos qual o "pixelsPorArea" testando divisores do width (ou divisores plausíveis).
 *   - Para cada y (linha) tentamos ver se há 67 blocos no sentido x que formam
 *     101 (start), 4 dígitos L, 01010 (middle), 4 dígitos R, 101 (end).
 *
 * Neste exemplo simples, tentamos achar uma 'largura de área' fixada por heurística.
 * Caso encontrado, retornamos true e gravamos o EAN-8 em codigoSaida.
 */
static bool encontraEAN8NaImagem(const PBMImage *img, char *codigoSaida) {
    int w = img->width;
    int h = img->height;

    // Precisamos encontrar um "pixelsPorArea" e um "y" onde o código apareça.
    // As sequências do EAN-8 têm 67 bits. Então a largura do código é (67 * pixelsPorArea).
    // Precisamos varrer "pixelsPorArea" possíveis. Por segurança, tentamos
    // todos de 1 até, por ex., 20 ou 30 (limite arbitrário).
    // (Em um caso real, pode ser mais sofisticado.)
    for (int ppa = 1; ppa <= 100; ppa++) {
        int codeWidth = 67 * ppa;
        if (codeWidth > w) {
            // se codeWidth for maior que a largura da imagem, não faz sentido prosseguir
            break;
        }

        // para cada linha, tentamos extrair esses 67 bits
        for (int y = 0; y < h; y++) {
            // tentamos todos os possíveis offsets x_inicial onde caiba codeWidth
            for (int xStart = 0; xStart <= w - codeWidth; xStart++) {
                // Vamos ler 67 bits, cada bit corresponde a ppa colunas (0 ou 1).
                // Precisamos definir se consideramos '1' se a maioria dos pixels do bloco ppa for 1 etc.
                // Para simplificar, consideraremos que se "todos" (ou a maioria) forem 1, bit=1; senão, bit=0.
                // (Ajuste conforme desejado.)
                char bits[68];
                bits[67] = '\0';

                for (int bitIndex = 0; bitIndex < 67; bitIndex++) {
                    // contagem de pixels pretos dentro do bloco
                    int blacks = 0;
                    for (int px = 0; px < ppa; px++) {
                        int x = xStart + bitIndex * ppa + px;
                        if (getPixel(img, x, y) == 1) {
                            blacks++;
                        }
                    }
                    // se mais da metade é 1, consideramos bit=1
                    if (blacks > (ppa / 2)) {
                        bits[bitIndex] = '1';
                    } else {
                        bits[bitIndex] = '0';
                    }
                }

                // Agora tentamos decodificar
                char codigo[9];
                if (ean8ExtraiCodigo(bits, codigo)) {
                    // Valida o dígito verificador
                    if (ean8Valida(codigo)) {
                        strcpy(codigoSaida, codigo);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

int main(int argc, char *argv[]) {
    // Uso:
    //   ./extrai <arquivo PBM>

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivoPBM>\n", argv[0]);
        return 1;
    }

    const char *arquivoEntrada = argv[1];

    // tenta ler a imagem
    PBMImage *img = readPBM(arquivoEntrada);
    if (!img) {
        fprintf(stderr, "ERRO: arquivo inválido ou não pôde ser aberto.\n");
        return 1;
    }

    // tenta encontrar o código
    char codigo[9];
    if (!encontraEAN8NaImagem(img, codigo)) {
        fprintf(stderr, "ERRO: código de barras não encontrado.\n");
        freePBM(img);
        return 1;
    }

    freePBM(img);

    // Se encontrou, imprime
    printf("%s\n", codigo);
    return 0;
}
