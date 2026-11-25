#include <stdio.h>

int main(void)
{
    int n;

    printf("Digite um numero decimal (0 a 63): ");
    if (scanf("%d", &n) != 1) {
        printf("Erro: digite um numero inteiro.\n");
        return 1;
    }
    if (n < 0 || n > 63) {
        printf("Erro: fora do limite de 6 bits (0 a 63).\n");
        return 1;
    }

    printf("Hexadecimal: 0x%02X\n", n);
    printf("Binario: ");
    for (int i = 5; i >= 0; i--)
        printf("%d", (n >> i) & 1);
    printf("\n");
    return 0;
}
