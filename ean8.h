#ifndef EAN8_H
#define EAN8_H

#include <stdbool.h>

/**
 * Verifica se a string dada contém apenas dígitos (0-9).
 * Retorna true se for numérico, false caso contrário.
 */
bool ean8EhNumerico(const char *str);

/**
 * Verifica se a string dada tem exatamente 8 dígitos.
 * Retorna true se tiver 8 dígitos, false caso contrário.
 */
bool ean8TamanhoValido(const char *str);

/**
 * Calcula o dígito verificador a partir dos 7 primeiros dígitos (string com pelo menos 7 dígitos).
 * Retorna o dígito verificador (0 a 9).
 */
int ean8CalculaDigitoVerificador(const char *str7);

/**
 * Verifica se o identificador de 8 dígitos é válido:
 *   1) possui apenas dígitos;
 *   2) possui 8 dígitos;
 *   3) o último dígito confere com o dígito verificador calculado.
 * Retorna true se for válido, false caso contrário.
 */
bool ean8Valida(const char *codigo);

/**
 * Gera a sequência de bits (0s e 1s) de tamanho 67 (3 + 28 + 5 + 28 + 3)
 * representando o código EAN-8.
 *
 *    - codigo: string de 8 dígitos
 *    - bitsSaida: buffer de saída que deve ter espaço para pelo menos 68 chars
 *                 (67 bits + caractere terminador '\0')
 */
void ean8GeraSequencia(const char *codigo, char *bitsSaida);

/**
 * A partir de uma sequência de bits EAN-8 (de tamanho exato 67), extrai
 * o identificador de 8 dígitos. Retorna true em caso de sucesso.
 *
 *    - bits: sequência de 67 bits (char '0' ou '1')
 *    - codigoSaida: buffer de saída que deve ter espaço para pelo menos 9 chars
 *                   (8 dígitos + '\0')
 */
bool ean8ExtraiCodigo(const char *bits, char *codigoSaida);

#endif
