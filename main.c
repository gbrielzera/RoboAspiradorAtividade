#include <stdio.h>
#include <stdlib.h>

struct posicao {
    int r, c;
};

enum acoes {
    LIMPAR, MOVER_N, MOVER_S, MOVER_L, MOVER_O, FICAR
};

struct dimensoes {
    int N, M, T, sujeira_total;
    int limpezas, bloqueios, inicial_sujeira;
    char **g;
};

int pode_mover(struct dimensoes *d, int r, int c) {
    if (r < 0 || r >= d->N || c < 0 || c >= d->M) return 0;
    if (d->g[r][c] == '#') return 0;
    return 1;
}

enum acoes decide_reflex(struct dimensoes *d, struct posicao *p) {
    // Regra 1: Limpar [cite: 6, 140]
    if (d->g[p->r][p->c] == '*') return LIMPAR;

    if (p->r - 1 >= 0 && d->g[p->r - 1][p->c] == '*') return MOVER_N;
    if (p->r + 1 < d->N && d->g[p->r + 1][p->c] == '*') return MOVER_S;
    if (p->c + 1 < d->M && d->g[p->r][p->c + 1] == '*') return MOVER_L;
    if (p->c - 1 >= 0 && d->g[p->r][p->c - 1] == '*') return MOVER_O;

    if (p->c % 2 == 0) {
        if (pode_mover(d, p->r, p->c + 1)) return MOVER_L;
        if (pode_mover(d, p->r + 1, p->c)) return MOVER_S;
        return MOVER_O;
    } else {
        if (pode_mover(d, p->r, p->c - 1)) return MOVER_O;
        if (pode_mover(d, p->r + 1, p->c)) return MOVER_S;
        return MOVER_L;
    }
}

void aplicar_acao(struct dimensoes *d, struct posicao *p, enum acoes acao) {
    if (acao == LIMPAR) {
        d->g[p->r][p->c] = '.';
        d->sujeira_total--;
        d->limpezas++;
    } else if (acao != FICAR) {
        int dr = 0, dc = 0;
        if (acao == MOVER_N) dr = -1;
        else if (acao == MOVER_S) dr = 1;
        else if (acao == MOVER_L) dc = 1;
        else if (acao == MOVER_O) dc = -1;

        if (pode_mover(d, p->r + dr, p->c + dc)) {
            p->r += dr;
            p->c += dc;
        } else {
            d->bloqueios++;
        }
    }
}

void definirValores(struct dimensoes *d, struct posicao *p) {
    printf("Digite N, M e T: ");
    scanf("%d %d %d", &d->N, &d->M, &d->T);
    d->sujeira_total = 0;
    d->limpezas = 0;
    d->bloqueios = 0;

    d->g = (char **)malloc(d->N * sizeof(char *));
    getchar(); 

    printf("Digite as %d linhas do mapa:\n", d->N);
    for (int i = 0; i < d->N; i++) {
        d->g[i] = (char *)malloc((d->M + 2) * sizeof(char));
        fgets(d->g[i], d->M + 2, stdin); 
        for (int j = 0; j < d->M; j++) {
            if (d->g[i][j] == 'S') {
                p->r = i; p->c = j;
                d->g[i][j] = '.';
            } else if (d->g[i][j] == '*') {
                d->sujeira_total++;
            }
        }
    }
    d->inicial_sujeira = d->sujeira_total;
}

void imprimir_mapa(struct dimensoes *d, struct posicao *p) {
    for (int i = 0; i < d->N; i++) {
        for (int j = 0; j < d->M; j++) {
            if (i == p->r && j == p->c) printf("R ");
            else printf("%c ", d->g[i][j]);
        }
        printf("\n");
    }
}

int main() {
    struct dimensoes d;
    struct posicao p;
    int modo_passo;

    definirValores(&d, &p);

    printf("Deseja modo passo-a-passo? (1=sim, 0=nao): ");
    scanf("%d", &modo_passo);
    getchar();

    int t;
    for (t = 0; t < d.T && d.sujeira_total > 0; t++) {
        if (modo_passo) {
            printf("\033[H\033[J");
            imprimir_mapa(&d, &p);
            printf("Passo: %d | Sujeira: %d | Limpezas: %d\n", t, d.sujeira_total, d.limpezas);
            printf("Pressione Enter para continuar...");
            getchar();
        }
        
        enum acoes acao = decide_reflex(&d, &p);
        aplicar_acao(&d, &p, acao);
    }

    printf("\n--- RESULTADOS FINAIS ---");
    printf("\nPassos executados: %d", t);
    printf("\nLimpezas: %d", d.limpezas);
    printf("\nBloqueios: %d", d.bloqueios);
    printf("\nPorcentagem limpa: %.2f%%", ((float)d.limpezas / d.inicial_sujeira) * 100);
    printf("\n-------------------------\n");

    for (int i = 0; i < d.N; i++) free(d.g[i]);
    free(d.g);
    return 0;
}