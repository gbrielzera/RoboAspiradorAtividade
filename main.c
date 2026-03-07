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

int main() {
    struct posicao posicao;

    
    return 0;
}