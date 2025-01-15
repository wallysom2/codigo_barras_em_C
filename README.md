# **Projeto EAN-8 em C**
Este projeto implementa dois programas em C que manipulam códigos de barras no padrão **EAN-8**:
1. **Gerar um código de barras EAN-8 em formato PBM.**
2. **Extrair o identificador de um código de barras EAN-8 a partir de uma imagem PBM.**

---

## **Características**
### EAN-8
- Contém **8 dígitos**: 7 dígitos de dados + 1 dígito verificador.
- Verifica automaticamente se o identificador é válido, garantindo a conformidade com o padrão EAN-8.
- Gera imagens no formato **PBM ASCII**, ideal para imagens em preto e branco.

---

## **Estrutura do Projeto**
O projeto é dividido nos seguintes arquivos:
1. **main_gera.c**: Gera imagens PBM com códigos de barras.
2. **main_extrai.c**: Extrai o identificador de uma imagem PBM contendo um código de barras.
3. **ean8.h / ean8.c**: Lógica para validação e codificação de EAN-8.
4. **image.h / image.c**: Manipulação de imagens no formato PBM.

---

## **Compilação**
Para compilar os programas, utilize o **GCC**:
```bash
gcc main_gera.c ean8.c image.c -o gera
gcc main_extrai.c ean8.c image.c -o extrai
```

---

## **Modo de Uso**

### 1. **Gerar um Código de Barras**
```bash
./gera <identificador> [espacamento] [pixels_por_area] [altura] [arquivo_saida]
```
#### **Parâmetros**:
- **identificador**: Número EAN-8 com **8 dígitos**.
- **[espacamento]** (opcional): Número de pixels de margem lateral. *(Padrão: 4)*
- **[pixels_por_area]** (opcional): Largura de cada barra em pixels. *(Padrão: 3)*
- **[altura]** (opcional): Altura do código de barras em pixels. *(Padrão: 50)*
- **[arquivo_saida]** (opcional): Nome do arquivo de saída. *(Padrão: `saida.pbm`)*

#### **Exemplo**:
```bash
./gera 40170725 4 3 50 codigo.pbm
```
- Gera uma imagem `codigo.pbm` com o código de barras para o número **40170725**.

---

### 2. **Extrair um Código de Barras**
```bash
./extrai <arquivo_pbm>
```
#### **Parâmetros**:
- **arquivo_pbm**: Nome do arquivo PBM que contém o código de barras.

#### **Exemplo**:
```bash
./extrai codigo.pbm
```
- Extrai o identificador do arquivo `codigo.pbm` e imprime no terminal.

---

## **Formato PBM**
- **Cabeçalho**:
  ```
  P1
  largura altura
  ```
- **Dados**:
  Uma matriz de 0s (branco) e 1s (preto) que representam os pixels.

#### **Exemplo de PBM**:
```pbm
P1
7 10
0 0 0 0 0 0 0
0 0 0 0 0 1 0
0 0 0 0 0 1 0
...
```

---

## **Validações Implementadas**
1. **Identificador Inválido**:
   - Apenas números são permitidos.
   - Deve conter exatamente **8 dígitos**.
   - O último dígito é validado como o **dígito verificador**.

2. **Arquivo Existente**:
   - Antes de sobrescrever, o programa solicita confirmação.

3. **Arquivo PBM Inválido**:
   - Verifica se o arquivo de entrada é um PBM válido antes de processar.

---

## **Exemplos de Execução**

### Gerando Código de Barras
```bash
./gera 12345670 5 4 60 exemplo.pbm
```
**Resultado**:
- Gera um arquivo `exemplo.pbm` com um código de barras para o número **12345670**.

---

### Extraindo Código de Barras
```bash
./extrai exemplo.pbm
```
**Resultado**:
- Imprime o identificador **12345670** no terminal.

---

## **Formato de Saída**
### Arquivo Gerado
O arquivo gerado é uma imagem no formato PBM, que pode ser visualizado com ferramentas compatíveis ou diretamente no terminal.

---

## **Possíveis Erros**
- **"Identificador inválido"**: O número EAN-8 fornecido não atende ao padrão.
- **"Arquivo não encontrado ou inválido"**: O arquivo PBM não existe ou está corrompido.
- **"Código de barras não encontrado"**: O arquivo PBM não contém um código de barras reconhecível.

---
