/*
 * Conversor Decimal <-> Hexadecimal (6 bits)
 *
 * Com 6 bits conseguimos representar 2^6 = 64 valores sem sinal:
 *   Decimal:      0 a 63
 *   Hexadecimal:  0x00 a 0x3F
 *   Binario:      000000 a 111111
 *
 * Qualquer valor fora dessa faixa (ex.: -1 ou 64) e rejeitado.
 *
 * Compilar:  gcc -Wall -Wextra -std=c99 -o conversor conversor.c
 * Executar:  ./conversor
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

#define NUM_BITS   6
#define VALOR_MIN  0
#define VALOR_MAX  ((1 << NUM_BITS) - 1)   /* 2^6 - 1 = 63 */
#define TAM_LINHA  64

/* Remove o '\n' do final e espacos nas pontas da string lida. */
static void limpar_linha(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1]))
        s[--n] = '\0';

    size_t i = 0;
    while (s[i] != '\0' && isspace((unsigned char)s[i]))
        i++;
    if (i > 0)
        memmove(s, s + i, strlen(s + i) + 1);
}

/*
 * Quando a entrada vem de um arquivo/pipe (testes automaticos), o texto
 * digitado nao aparece na tela; entao repetimos a entrada para o log ficar legivel.
 */
static void ecoar(const char *s)
{
    if (!isatty(fileno(stdin)))
        printf("%s\n", s);
}

/* Converte um digito (0-15) no caractere hexadecimal correspondente. */
static char digito_para_hex(int d)
{
    const char tabela[] = "0123456789ABCDEF";
    return tabela[d];
}

/* Converte um caractere hexadecimal no seu valor (0-15). Retorna -1 se invalido. */
static int hex_para_digito(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    c = (char)toupper((unsigned char)c);
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

/*
 * Decimal -> Hexadecimal por divisoes sucessivas por 16.
 * Os restos, lidos de baixo para cima, formam o numero em hexadecimal.
 * Com 6 bits o resultado sempre cabe em 2 digitos hexadecimais.
 */
static void decimal_para_hex(int valor, char saida[3])
{
    int quociente = valor;

    saida[1] = digito_para_hex(quociente % 16);  /* digito menos significativo */
    quociente /= 16;
    saida[0] = digito_para_hex(quociente % 16);  /* digito mais significativo  */
    saida[2] = '\0';
}

/* Monta a representacao binaria em 6 bits (do bit 5 ao bit 0). */
static void decimal_para_binario(int valor, char saida[NUM_BITS + 1])
{
    for (int i = NUM_BITS - 1; i >= 0; i--)
        saida[NUM_BITS - 1 - i] = ((valor >> i) & 1) ? '1' : '0';
    saida[NUM_BITS] = '\0';
}

/*
 * Le uma string decimal e valida. Aceita sinal '-' para conseguir
 * detectar e recusar valores negativos. Retorna 1 se for um inteiro.
 */
static int ler_decimal(const char *s, long *valor)
{
    int i = 0, negativo = 0;
    long acumulado = 0;

    if (s[0] == '-' || s[0] == '+') {
        negativo = (s[0] == '-');
        i = 1;
    }
    if (s[i] == '\0')
        return 0;

    for (; s[i] != '\0'; i++) {
        if (!isdigit((unsigned char)s[i]))
            return 0;
        acumulado = acumulado * 10 + (s[i] - '0');
        if (acumulado > 100000)          /* evita overflow em entradas enormes */
            acumulado = 100000;
    }
    *valor = negativo ? -acumulado : acumulado;
    return 1;
}

/*
 * Hexadecimal -> Decimal pelo metodo posicional:
 *   valor = valor * 16 + digito   (para cada digito, da esquerda p/ direita)
 * Equivale a somar digito * 16^posicao.
 * Aceita prefixo opcional "0x". Retorna 1 se a string for hexadecimal valida.
 */
static int ler_hex(const char *s, long *valor, int *negativo)
{
    int i = 0;
    long acumulado = 0;

    *negativo = 0;
    if (s[0] == '-') {
        *negativo = 1;
        i = 1;
    } else if (s[0] == '+') {
        i = 1;
    }
    if (s[i] == '0' && (s[i + 1] == 'x' || s[i + 1] == 'X'))
        i += 2;
    if (s[i] == '\0')
        return 0;

    for (; s[i] != '\0'; i++) {
        int d = hex_para_digito(s[i]);
        if (d < 0)
            return 0;
        acumulado = acumulado * 16 + d;
        if (acumulado > 0xFFFFF)
            acumulado = 0xFFFFF;
    }
    *valor = *negativo ? -acumulado : acumulado;
    return 1;
}

static int dentro_do_limite(long v)
{
    return v >= VALOR_MIN && v <= VALOR_MAX;
}

static void opcao_decimal_para_hex(void)
{
    char linha[TAM_LINHA];
    long valor;

    printf("Digite um numero decimal (%d a %d): ", VALOR_MIN, VALOR_MAX);
    if (!fgets(linha, sizeof linha, stdin))
        return;
    limpar_linha(linha);
    ecoar(linha);

    if (!ler_decimal(linha, &valor)) {
        printf("  [ERRO] \"%s\" nao e um numero decimal valido.\n\n", linha);
        return;
    }
    if (!dentro_do_limite(valor)) {
        printf("  [ERRO] %ld esta fora do limite de %d bits (%d a %d).\n",
               valor, NUM_BITS, VALOR_MIN, VALOR_MAX);
        if (valor < VALOR_MIN)
            printf("         Valores negativos nao podem ser representados (sem sinal).\n\n");
        else
            printf("         %ld precisaria de mais de %d bits.\n\n", valor, NUM_BITS);
        return;
    }

    char hex[3], bin[NUM_BITS + 1];
    decimal_para_hex((int)valor, hex);
    decimal_para_binario((int)valor, bin);

    printf("  Divisoes sucessivas por 16:\n");
    printf("    %2ld / 16 = %ld  resto %2ld -> '%c'\n", valor, valor / 16, valor % 16, hex[1]);
    printf("    %2ld / 16 = %ld  resto %2ld -> '%c'\n", valor / 16, 0L, valor / 16, hex[0]);
    printf("  Resultado: %ld (decimal) = 0x%s (hexadecimal) = %s (binario 6 bits)\n\n",
           valor, hex, bin);
}

static void opcao_hex_para_decimal(void)
{
    char linha[TAM_LINHA];
    long valor;
    int negativo;

    printf("Digite um numero hexadecimal (00 a %X): ", VALOR_MAX);
    if (!fgets(linha, sizeof linha, stdin))
        return;
    limpar_linha(linha);
    ecoar(linha);

    if (!ler_hex(linha, &valor, &negativo)) {
        printf("  [ERRO] \"%s\" nao e um numero hexadecimal valido (use 0-9 e A-F).\n\n", linha);
        return;
    }
    if (!dentro_do_limite(valor)) {
        printf("  [ERRO] %s (= %ld em decimal) esta fora do limite de %d bits (00 a %X).\n\n",
               linha, valor, NUM_BITS, VALOR_MAX);
        return;
    }

    char hex[3], bin[NUM_BITS + 1];
    decimal_para_hex((int)valor, hex);
    decimal_para_binario((int)valor, bin);

    int alto = hex_para_digito(hex[0]);
    int baixo = hex_para_digito(hex[1]);
    printf("  Metodo posicional:\n");
    printf("    %c x 16^1 + %c x 16^0 = %d x 16 + %d x 1 = %d + %d = %ld\n",
           hex[0], hex[1], alto, baixo, alto * 16, baixo, valor);
    printf("  Resultado: 0x%s (hexadecimal) = %ld (decimal) = %s (binario 6 bits)\n\n",
           hex, valor, bin);
}

int main(void)
{
    char linha[TAM_LINHA];

    printf("==============================================\n");
    printf("  CONVERSOR DECIMAL <-> HEXADECIMAL (6 BITS)\n");
    printf("  Faixa valida: 0 a 63  |  0x00 a 0x3F\n");
    printf("==============================================\n\n");

    for (;;) {
        printf("1 - Decimal para Hexadecimal\n");
        printf("2 - Hexadecimal para Decimal\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        if (!fgets(linha, sizeof linha, stdin))
            break;
        limpar_linha(linha);
        ecoar(linha);

        if (strcmp(linha, "1") == 0)
            opcao_decimal_para_hex();
        else if (strcmp(linha, "2") == 0)
            opcao_hex_para_decimal();
        else if (strcmp(linha, "0") == 0) {
            printf("Encerrando o programa.\n");
            break;
        } else
            printf("  [ERRO] Opcao invalida.\n\n");
    }
    return 0;
}
