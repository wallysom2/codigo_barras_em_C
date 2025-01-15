#include "ean8.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Tabela com a codificação L e R. Cada dígito tem L-code e R-code */
static const char *L_TABLE[10] = {
    "0001101", // 0
    "0011001", // 1
    "0010011", // 2
    "0111101", // 3
    "0100011", // 4
    "0110001", // 5
    "0101111", // 6
    "0111011", // 7
    "0110111", // 8
    "0001011"  // 9
};

static const char *R_TABLE[10] = {
    "1110010", // 0
    "1100110", // 1
    "1101100", // 2
    "1000010", // 3
    "1011100", // 4
    "1001110", // 5
    "1010000", // 6
    "1000100", // 7
    "1001000", // 8
    "1110100"  // 9
};

bool ean8EhNumerico(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return false;
        }
    }
    return true;
}

bool ean8TamanhoValido(const char *str) {
    return (strlen(str) == 8);
}

/**
 * Soma ponderada:
 *   - dígitos em posição ímpar (1-based) têm peso 3
 *   - dígitos em posição par (1-based) têm peso 1
 *   -> soma
 *   -> a distância até o próximo múltiplo de 10 é o dígito verificador.
 */
int ean8CalculaDigitoVerificador(const char *str7) {
    int soma = 0;
    for (int i = 0; i < 7; i++) {
        int digito = str7[i] - '0';
        if ((i + 1) % 2 == 1) { 
            // posições ímpares (1,3,5,7): peso 3
            soma += digito * 3;
        } else {
            // posições pares (2,4,6): peso 1
            soma += digito * 1;
        }
    }
    int resto = soma % 10;
    int verificador = (resto == 0) ? 0 : (10 - resto);
    return verificador;
}

bool ean8Valida(const char *codigo) {
    if (!ean8EhNumerico(codigo)) {
        return false;
    }
    if (!ean8TamanhoValido(codigo)) {
        return false;
    }
    // calcula digito verificador
    int calculado = ean8CalculaDigitoVerificador(codigo);
    int informado = codigo[7] - '0';
    return (calculado == informado);
}

void ean8GeraSequencia(const char *codigo, char *bitsSaida) {
    // codigo tem 8 dígitos
    // bitsSaida terá 67 bits + '\0'
    // Formato: 
    //   INICIO (101) 
    //   4 dígitos (L-code) -> cada 7 bits => total 28
    //   CENTRO (01010) 
    //   4 dígitos (R-code) -> cada 7 bits => total 28
    //   FIM (101)

    // marcador de início: 3 bits
    const char *start = "101";
    // marcador central: 5 bits
    const char *middle = "01010";
    // marcador de fim: 3 bits
    const char *end = "101";

    // Montagem no buffer
    // 1) copia start
    strcpy(bitsSaida, start);

    // 2) 4 dígitos L-code
    for (int i = 0; i < 4; i++) {
        int dig = codigo[i] - '0';
        strcat(bitsSaida, L_TABLE[dig]);
    }

    // 3) marcador central
    strcat(bitsSaida, middle);

    // 4) 4 dígitos R-code
    for (int i = 4; i < 8; i++) {
        int dig = codigo[i] - '0';
        strcat(bitsSaida, R_TABLE[dig]);
    }

    // 5) marcador de fim
    strcat(bitsSaida, end);
}

/* 
   ean8ExtraiCodigo:
   - bits (char[]) com 67 bits 
   - codeSaida[9] deve receber o identificador EAN-8
*/
static int decodeDigitL(const char *bits7) {
    for (int d = 0; d < 10; d++) {
        if (strncmp(bits7, L_TABLE[d], 7) == 0) {
            return d;
        }
    }
    return -1;
}

static int decodeDigitR(const char *bits7) {
    for (int d = 0; d < 10; d++) {
        if (strncmp(bits7, R_TABLE[d], 7) == 0) {
            return d;
        }
    }
    return -1;
}

bool ean8ExtraiCodigo(const char *bits, char *codigoSaida) {
    // Verifica tamanho
    if (strlen(bits) != 67) {
        return false;
    }

    // bits[0..2] = 101 -> start
    // bits[3..30] = 4 dígitos L-code (4*7 = 28 bits)
    // bits[31..35] = 01010 -> middle
    // bits[36..63] = 4 dígitos R-code (4*7 = 28 bits)
    // bits[64..66] = 101 -> end

    // Checa marcadores básicos:
    if (strncmp(bits, "101", 3) != 0) return false;
    if (strncmp(bits + 31, "01010", 5) != 0) return false;
    if (strncmp(bits + 64, "101", 3) != 0) return false;

    // Decodifica 4 dígitos L
    char tmp[8];
    int pos = 3; // logo após "101"
    for (int i = 0; i < 4; i++) {
        strncpy(tmp, bits + pos, 7);
        tmp[7] = '\0';
        int d = decodeDigitL(tmp);
        if (d < 0) return false;
        codigoSaida[i] = (char)('0' + d);
        pos += 7;
    }

    // pula 5 bits do middle
    pos += 5;

    // Decodifica 4 dígitos R
    for (int i = 4; i < 8; i++) {
        strncpy(tmp, bits + pos, 7);
        tmp[7] = '\0';
        int d = decodeDigitR(tmp);
        if (d < 0) return false;
        codigoSaida[i] = (char)('0' + d);
        pos += 7;
    }

    codigoSaida[8] = '\0';
    return true;
}
