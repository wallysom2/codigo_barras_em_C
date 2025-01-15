#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>  // para verificar existência de arquivo
#include "ean8.h"
#include "image.h"

/**
 * Verifica se um arquivo existe.
 */
static bool arquivoExiste(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int main(int argc, char *argv[]) {
    // Uso:
    //  ./gera <identificador8> [espacamento=4] [pixelPorArea=3] [altura=50] [saida="saida.pbm"]

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <identificador EAN-8> [espacamento] [pixels_por_area] [altura] [arquivoSaida]\n", argv[0]);
        return 1;
    }

    // Identificador EAN-8
    char *codigo = argv[1];

    // Checa validade
    if (!ean8EhNumerico(codigo)) {
        fprintf(stderr, "ERRO: identificador contém valores não numéricos.\n");
        return 1;
    }
    if (!ean8TamanhoValido(codigo)) {
        fprintf(stderr, "ERRO: identificador não possui 8 dígitos.\n");
        return 1;
    }
    if (!ean8Valida(codigo)) {
        fprintf(stderr, "ERRO: dígito verificador inválido.\n");
        return 1;
    }

    // Parâmetros opcionais
    int espacamento = 4;
    int pixelsPorArea = 3;
    int altura = 50;
    char saida[256];
    strcpy(saida, "saida.pbm");

    if (argc >= 3) {
        espacamento = atoi(argv[2]);
        if (espacamento < 0) espacamento = 4;
    }
    if (argc >= 4) {
        pixelsPorArea = atoi(argv[3]);
        if (pixelsPorArea < 1) pixelsPorArea = 3;
    }
    if (argc >= 5) {
        altura = atoi(argv[4]);
        if (altura < 1) altura = 50;
    }
    if (argc >= 6) {
        strncpy(saida, argv[5], 255);
        saida[255] = '\0';
    }

    // Se o arquivo já existir, pergunta se pode sobrescrever
    if (arquivoExiste(saida)) {
        printf("O arquivo \"%s\" já existe. Deseja sobrescrevê-lo? (s/n) ", saida);
        char resp[10];
        if (!fgets(resp, sizeof(resp), stdin)) {
            fprintf(stderr, "ERRO de leitura.\n");
            return 1;
        }
        if (resp[0] != 's' && resp[0] != 'S') {
            fprintf(stderr, "ERRO: arquivo resultante já existe.\n");
            return 1;
        }
    }

    // Gera sequência EAN-8 com 67 bits
    char sequencia[68];
    sequencia[0] = '\0';
    ean8GeraSequencia(codigo, sequencia);

    // Tamanho total do código em pixels (largura): 
    //   67 "áreas" * pixelsPorArea
    //   + 2 * espacamento
    int larguraCodigo = 67 * pixelsPorArea;
    int larguraTotal = larguraCodigo + 2 * espacamento;

    // Altura total = altura do código + 2 * espacamento vertical (opcional)
    // para simplificar, vamos usar o mesmo espacamento vertical
    int alturaTotal = altura + 2 * espacamento;

    PBMImage *img = createPBM(larguraTotal, alturaTotal);
    if (!img) {
        fprintf(stderr, "ERRO: falha ao criar imagem.\n");
        return 1;
    }

    // Preenche com 0 (branco) - já feito no createPBM
    // Vamos marcar as barras (onde sequencia[i] == '1') com preto (1).

    // A "faixa" do código de barras estará verticalmente nos pixels
    // [espacamento..(espacamento+altura-1)] (inclusive).
    // Horizontalmente em [espacamento..(espacamento+larguraCodigo-1)].

    // Percorre cada bit da sequencia, se for '1', preenche as colunas correspondentes.
    for (int i = 0; i < 67; i++) {
        char bit = sequencia[i];
        int isPreto = (bit == '1') ? 1 : 0;

        // As colunas correspondentes são de x1 até x2 (x2 = x1 + pixelsPorArea - 1)
        int xInicio = espacamento + i * pixelsPorArea;
        for (int px = 0; px < pixelsPorArea; px++) {
            int x = xInicio + px;
            // preenche verticalmente na faixa de altura
            for (int y = espacamento; y < (espacamento + altura); y++) {
                setPixel(img, x, y, isPreto);
            }
        }
    }

    // Salva
    if (!writePBM(img, saida)) {
        fprintf(stderr, "ERRO ao escrever arquivo PBM.\n");
        freePBM(img);
        return 1;
    }

    freePBM(img);
    printf("Imagem com código de barras EAN-8 gerada em \"%s\" com sucesso!\n", saida);
    return 0;
}
