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
    int **visitado;
};

int pode_mover(struct dimensoes *d, int r, int c) {
    if (r < 0 || r >= d->N || c < 0 || c >= d->M) return 0;
    if (d->g[r][c] == '#') return 0;
    return 1;
}

enum acoes decide_reflex(struct dimensoes *d, struct posicao *p) {

    if (d->g[p->r][p->c] == '*')
        return LIMPAR;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, 1, -1};
    enum acoes direcoes[4] = {MOVER_N, MOVER_S, MOVER_L, MOVER_O};

    int menor_visita = 999999;
    enum acoes melhor = FICAR;

    for (int i = 0; i < 4; i++) {
        int nr = p->r + dr[i];
        int nc = p->c + dc[i];

        if (pode_mover(d, nr, nc)) {

            if (d->g[nr][nc] == '*')
                return direcoes[i];

            if (d->visitado[nr][nc] < menor_visita) {
                menor_visita = d->visitado[nr][nc];
                melhor = direcoes[i];
            }
        }
    }

    return melhor;
}

void aplicar_acao(struct dimensoes *d, struct posicao *p, enum acoes acao) {

    if (acao == LIMPAR) {

        d->g[p->r][p->c] = '.';
        d->sujeira_total--;
        d->limpezas++;

    } else if (acao != FICAR) {

        int dr = 0, dc = 0;

        if (acao == MOVER_N) dr = -1;
        if (acao == MOVER_S) dr = 1;
        if (acao == MOVER_L) dc = 1;
        if (acao == MOVER_O) dc = -1;

        if (pode_mover(d, p->r + dr, p->c + dc)) {

            p->r += dr;
            p->c += dc;

            d->visitado[p->r][p->c]++;

        } else {

            d->bloqueios++;

        }
    }
}

void definirValores(struct dimensoes *d, struct posicao *p) {

    char nome_arquivo[100];

    printf("Digite o nome do arquivo (ex: mapa_facil.txt): ");
    scanf("%s", nome_arquivo);

    FILE *f = fopen(nome_arquivo, "r");

    if (f == NULL) {
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

    if (fscanf(f, "%d %d %d", &d->N, &d->M, &d->T) != 3) {
        printf("Erro ao ler dimensoes\n");
        fclose(f);
        exit(1);
    }

    d->sujeira_total = 0;
    d->limpezas = 0;
    d->bloqueios = 0;

    d->g = malloc(d->N * sizeof(char *));
    d->visitado = malloc(d->N * sizeof(int*));

    for (int i = 0; i < d->N; i++) {

        d->g[i] = malloc((d->M + 2) * sizeof(char));
        d->visitado[i] = calloc(d->M, sizeof(int));

    }

    fgetc(f);

    for (int i = 0; i < d->N; i++) {

        fgets(d->g[i], d->M + 2, f);

        for (int j = 0; j < d->M; j++) {

            if (d->g[i][j] == 'S') {

                p->r = i;
                p->c = j;
                d->g[i][j] = '.';

            }

            if (d->g[i][j] == '*')
                d->sujeira_total++;

        }
    }

    d->inicial_sujeira = d->sujeira_total;

    fclose(f);

    printf("Mapa carregado com sucesso!\n");
}

void imprimir_mapa(struct dimensoes *d, struct posicao *p) {

    for (int i = 0; i < d->N; i++) {

        for (int j = 0; j < d->M; j++) {

            if (i == p->r && j == p->c)
                printf("R ");
            else
                printf("%c ", d->g[i][j]);

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

            printf("Passo: %d | Sujeira: %d | Limpezas: %d\n",
                   t, d.sujeira_total, d.limpezas);

            printf("Pressione Enter...");
            getchar();
        }

        enum acoes acao = decide_reflex(&d, &p);

        aplicar_acao(&d, &p, acao);
    }

    printf("\n--- RESULTADOS FINAIS ---");

    printf("\nPassos executados: %d", t);
    printf("\nLimpezas: %d", d.limpezas);
    printf("\nBloqueios: %d", d.bloqueios);

    printf("\nPorcentagem limpa: %.2f%%",
           ((float)d.limpezas / d.inicial_sujeira) * 100);

    printf("\n-------------------------\n");

    for (int i = 0; i < d.N; i++) {
        free(d.g[i]);
        free(d.visitado[i]);
    }

    free(d.g);
    free(d.visitado);

    return 0;
}