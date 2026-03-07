#include <stdio.h>

struct posicao {
    int r, c;
};

enum acoes {
    LIMPAR, MOVER_N, MOVER_S, MOVER_L, MOVER_O, FICAR
};

struct dimensoes {
    int N, M, T, sujeira_total;
    char **g;
};

void definirValores() {
    struct dimensoes dimensoes;
    printf("Digite o valor de N: ");
    scanf("%d", &dimensoes.N);
    printf("Digite o valor de M: ");
    scanf("%d", &dimensoes.M);
    printf("Digite o valor de T: ");
    scanf("%d", &dimensoes.T);
    printf("%d %d %d", dimensoes.N, dimensoes.M, dimensoes.T);
}

int main() {
    system("cls");
    struct posicao posicao;
    definirValores();


    return 0;
}