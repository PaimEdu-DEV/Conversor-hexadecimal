# Conversor Decimal ↔ Hexadecimal (6 bits)

Programa em C que converte números **decimais para hexadecimais** e **hexadecimais para decimais**
respeitando o limite de **6 bits sem sinal**:

| Base        | Faixa válida   |
|-------------|----------------|
| Decimal     | 0 a 63         |
| Hexadecimal | 0x00 a 0x3F    |
| Binário     | 000000 a 111111 |

Valores fora da faixa (ex.: `-1` e `64` / `0x40`) e entradas que não são números são rejeitados
com uma mensagem de erro.

## Como a conversão funciona

- **Decimal → Hexadecimal:** divisões sucessivas por 16; os restos (lidos de baixo para cima) são os dígitos hexadecimais.
  Ex.: `42 / 16 = 2 resto 10 (A)` → `2 / 16 = 0 resto 2` → **0x2A**
- **Hexadecimal → Decimal:** método posicional, `valor = valor * 16 + dígito`.
  Ex.: `2A = 2×16¹ + 10×16⁰ = 32 + 10 =` **42**
- O programa também mostra a representação binária de 6 bits do resultado.

## Compilar e executar

```bash
gcc -Wall -Wextra -std=c99 -o conversor conversor.c
./conversor
```

No Windows (MinGW): `gcc -Wall -Wextra -std=c99 -o conversor.exe conversor.c` e depois `conversor.exe`.

## Testes

A pasta `testes/` contém arquivos de entrada para rodar os casos de teste por redirecionamento:

```bash
./conversor < testes/entrada_completa.txt
```

| Arquivo | Casos |
|---|---|
| `t1_dec_validos.txt`   | Dec → Hex: 0, 42, 63 |
| `t2_dec_invalidos.txt` | Dec → Hex: -1, 64, abc |
| `t3_hex_validos.txt`   | Hex → Dec: 00, 2A, 3F |
| `t4_hex_invalidos.txt` | Hex → Dec: -1, 40, G1 |
| `entrada_completa.txt` | Todos os casos acima (saída em `saida_completa.txt`) |

Os prints das execuções estão na pasta `prints/`.
