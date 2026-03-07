#include <stdio.h>
#include <stdlib.h>

struct posicao {
    int r, c;
};

enum acoes {
    LIMPAR, MOVER_N, MOVER_S, MOVER_L, MOVER_O, FICAR
};

enum acoes decide_reflex(struct dimensoes *d, struct posicao *p) {
    if (d->g[p->r][p->c] == '*') return LIMPAR;

    if (p->c % 2 == 0) return MOVER_L;
    else return MOVER_O;
}

struct dimensoes {
    int N, M, T, sujeira_total;
    char **g;
};

void definirValores(struct dimensoes *d, struct posicao *p) {
    printf("Digite N, M e T (ex: 5 7 80): ");
    scanf("%d %d %d", &d->N, &d->M, &d->T);

    d->sujeira_total = 0;

    d->g = (char **)malloc(d->N * sizeof(char *));

    getchar(); 

    printf("Digite as %d linhas do mapa:\n", d->N);
    for (int i = 0; i < d->N; i++) {
        d->g[i] = (char *)malloc((d->M + 1) * sizeof(char));

        fgets(d->g[i], d->M + 2, stdin); 

        for (int j = 0; j < d->M; j++) {
            if (d->g[i][j] == 'S') {
                p->r = i;
                p->c = j;
                d->g[i][j] = '.';
            } else if (d->g[i][j] == '*') {
                d->sujeira_total++;
            }
        }
    }
}

void imprimir_mapa(struct dimensoes *d, struct posicao *p) {
    for (int i = 0; i < d->N; i++) {
        for (int j = 0; j < d->M; j++) {
            if (i == p->r && j == p->c) {
                printf("R "); // Desenha o robô na posição atual
            } else {
                printf("%c ", d->g[i][j]);
            }
        }
        printf("\n");
    }
}

int main() {
    struct dimensoes d;
    struct posicao p;

    definirValores(&d, &p);
    for (int t = 0; t < d.T && d.sujeira_total > 0; t++) {
    printf("\nPasso %d:\n", t);
    imprimir_mapa(&d, &p);
    
    enum acoes acao = decide_reflex(&d, &p);
}

    printf("\nRobo comeca em: L:%d C:%d", p.r, p.c);
    printf("\nTotal de sujeira no mapa: %d\n", d.sujeira_total);

    for (int i = 0; i < d.N; i++) free(d.g[i]);
    free(d.g);

    return 0;
}