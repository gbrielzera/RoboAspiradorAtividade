#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// structs funcionam como fichas que agrupam informações

// guarda apenas a linha (r) e a coluna (c) onde o robô está no momento
struct posicao {
    int r, c;
};


// cria uma lista de comandos que o robô pode entender, como LIMPAR, mover para o Norte, Sul, Leste, Oeste ou FICAR parado
enum acoes {
    LIMPAR, MOVER_N, MOVER_S, MOVER_L, MOVER_O, FICAR
};

// é tipo o processador do robô, guarda o tamanho do mapa (n x m), o limite de tempo (T), a quantidade de sujeira, o desenho do mapa (g) e o histórico de onde o robô já passou (visitado)
struct dimensoes {
    int N, M, T, sujeira_total;
    int limpezas, bloqueios, inicial_sujeira;
    char **g;
    int **visitado;
};


// antes do robô dar um passo, ele precisa saber se o caminho está livre, essa função verifica duas coisas:
// se a posição está dentro dos limites do mapa para não bater na borda
// se a posição não é uma parede (#).
int pode_mover(struct dimensoes *d, int r, int c) {
    if (r < 0 || r >= d->N || c < 0 || c >= d->M) return 0;
    if (d->g[r][c] == '#') return 0;
    return 1;
}


// inteligência do robô: este é o bloco que decide o que fazer a cada turno:
// prioridade 1: se onde ele está agora tem sujeira (*), a ação é LIMPAR
// prioridade 2: Se houver sujeira em qualquer um dos quatro quadrados vizinhos, ele se move para lá imediatamente
// prioridade 3 (exploração): Se não houver sujeira por perto, ele olha para os vizinhos e escolhe o caminho que ele visitou menos vezes (o menor valor na matriz visitado) garantindo que ele explore áreas novas
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


// execução da ação: após decidir o que fazer, esta função altera o estado do mundo: 
// se for limpar, ele troca o * por . (limpo) e diminui o contador de sujeira total
// se for se mover, ele atualiza a posição do robô e soma +1 na matriz de visitado para marcar que passou por ali
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


// carregamento do mapa: essa função lê arquivos de texto
// ela lê o tamanho do mapa e o tempo disponível
// ela reserva espaço na memória do computador para o mapa com o malloc
// ela identifica onde o robô começa (S) e conta quanta sujeira existe no total para saber quando o trabalho vai terminar
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

    // variáveis para contar o tempo de execução do programa
    LARGE_INTEGER inicio, fim, frequencia;

    QueryPerformanceFrequency(&frequencia);
    
    // reserva memória para a "ficha" do robô e para o "mundo" dele
    struct dimensoes d;
    struct posicao p;
    int modo_passo;
    
    definirValores(&d, &p);
    
    //se sim, o programa vai limpar a tela e mostrar o mapa a cada movimento do robô, com o enter para continuar
    printf("Deseja modo passo-a-passo? (1=sim, 0=nao): ");
    scanf("%d", &modo_passo);
    getchar();
    QueryPerformanceCounter(&inicio);

    int t;

    // esse é o coração do programa. 
    // o robô entra num loop que se repete enquanto duas condições forem verdadeiras:
    // tempo: se o tempo atual t for menor que o limite T definido no mapa
    // sujeira: se ainda houver sujeira no mapa (sujeira_total > 0)
    for (t = 0; t < d.T && d.sujeira_total > 0; t++) {
    // dentro deste loop, acontecem três coisas:
    // visualização: se o modo passo-a-passo estiver ligado, ele imprime o mapa e as estatísticas atuais como passos dados e sujeira restante
    // decisão: chama a função decide_reflex, onde o robô analisa os arredores e decide qual a melhor ação
    // ação: chama a função aplicar_acao para efetivamente mudar a posição do robô ou remover a sujeira do mapa


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

    QueryPerformanceCounter(&fim);

    double tempo_ms = (double)(fim.QuadPart - inicio.QuadPart) * 1000.0 / frequencia.QuadPart;

    printf("Tempo de execucao: %.3f ms\n", tempo_ms);


    return 0;
}