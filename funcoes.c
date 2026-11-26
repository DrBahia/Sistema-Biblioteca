#include <stdio.h>
#include "funcoes.h"
#include <stdlib.h>
#include <string.h>


/*Utilitários de terminal*/

void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void limpar_buffer() {
    scanf("%*[^\n]");
    scanf("%*c");
}

void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    limpar_buffer();
}

/* Lê um inteiro; retorna -1 se a entrada for inválida */
int ler_opcao() {
    int opcao;
    if (scanf("%d", &opcao) != 1)
        opcao = -1;
    limpar_buffer();
    return opcao;
}


/*Estruturas de dados*/

#define ARQ_LIVROS      "livros.csv"
#define ARQ_USUARIOS    "usuarios.csv"
#define ARQ_EMPRESTIMOS "emprestimos.csv"

/*Capacidade inicial de cada vetor dinamico.*/
#define CAP_INICIAL 4


typedef struct {
    int  codigo;
    char titulo[100];
    char autor[100];
    int  ano;
    int  quantidade;
} Livro;

typedef struct {
    char matricula[20];
    char nome[100];
    char curso[100];
} Usuario;

typedef struct {
    int  codigo_livro;
    char matricula[20];
    int  devolvido;     /* 0 = em aberto, 1 = devolvido */
} Emprestimo;

Livro      *livros      = NULL;
Usuario    *usuarios    = NULL;
Emprestimo *emprestimos = NULL;

int total_livros      = 0;
int total_usuarios    = 0;
int total_emprestimos = 0;


int cap_livros      = 0;           // capacidade reservada
int cap_usuarios    = 0;
int cap_emprestimos = 0;

int inicializar_dados() {
    if (!inicializar_dados()) {
        printf("\nErro: sem memoria para iniciar o sistema.\n");
        liberar_dados();
        exit(1);
    }
    if (livros == NULL) {
        livros = malloc(CAP_INICIAL * sizeof *livros);
        if (livros == NULL) return 0;
        cap_livros = CAP_INICIAL;
    }
    if (usuarios == NULL) {
        usuarios = malloc(CAP_INICIAL * sizeof *usuarios);
        if (usuarios == NULL) return 0;
        cap_usuarios = CAP_INICIAL;
    }
    if (emprestimos == NULL) {
        emprestimos = malloc(CAP_INICIAL * sizeof *emprestimos);
        if (emprestimos == NULL) return 0;
        cap_emprestimos = CAP_INICIAL;
    }
    return 1;
}
int garantir_cap_livros() { 
    if (total_livros < cap_livros) return 1;
    int nova = (cap_livros == 0) ? CAP_INICIAL : cap_livros * 2;
    Livro *aux = realloc(livros, nova * sizeof *livros);
    if (aux == NULL) return 0;
    livros = aux;
    cap_livros = nova;
    return 1;
}
int garantir_cap_usuarios() {
    if (total_usuarios < cap_usuarios) return 1;
    int nova = (cap_usuarios == 0) ? CAP_INICIAL : cap_usuarios * 2;
    Usuario *aux = realloc(usuarios, nova * sizeof *usuarios);
    if (aux == NULL) return 0;
    usuarios = aux;
    cap_usuarios = nova;
    return 1;
}
int garantir_cap_emprestimos() {
    if (total_emprestimos < cap_emprestimos) return 1;
    int nova = (cap_emprestimos == 0) ? CAP_INICIAL : cap_emprestimos * 2;
    Emprestimo *aux = realloc(emprestimos, nova * sizeof *emprestimos);
    if (aux == NULL) return 0;
    emprestimos = aux;
    cap_emprestimos = nova;
    return 1;
}
void liberar_dados() {
    free(livros);
    livros = NULL;
    free(usuarios);
    usuarios = NULL;
    free(emprestimos);
    emprestimos = NULL;

    total_livros = total_usuarios = total_emprestimos = 0;
    cap_livros = cap_usuarios = cap_emprestimos = 0;
}

/* ---------- Persistência em arquivos CSV ---------- */
/* Formato: uma linha de cabeçalho e depois um registro por linha,
   com os campos separados por ';'*/


//Formato: Codigo, Titulo, Autor, Ano, Quantidade
void carregar_livros() {
    FILE *arq = fopen(ARQ_LIVROS, "r");
    if (arq == NULL)
        return;     /* primeira execução: arquivo ainda não existe */

    fscanf(arq, "%*[^\n]");     /* pula o cabeçalho */

    while (1) {
         if (!garantir_cap_livros()) {
            printf("\nErro: sem memoria para carregar todos os livros.\n");
            break;
        }
        Livro *l = &livros[total_livros];
        if (fscanf(arq, " %d;%99[^;];%99[^;];%d;%d",
                   &l->codigo, l->titulo, l->autor, &l->ano, &l->quantidade) != 5)
            break;
        total_livros++;
    }

    fclose(arq);
}

//Formato: Matricula, Nome, Curso
void carregar_usuarios() {
    FILE *arq = fopen(ARQ_USUARIOS, "r");
    if (arq == NULL)
        return;

    fscanf(arq, "%*[^\n]");

    while (1) {
        if (!garantir_cap_usuarios()) {
            printf("\nErro: sem memoria para carregar todos os ususarios.\n");
            break;
        }
        Usuario *u = &usuarios[total_usuarios];
        if (fscanf(arq, " %19[^;];%99[^;];%99[^\r\n]",
                   u->matricula, u->nome, u->curso) != 3)
            break;
        total_usuarios++;
    }

    fclose(arq);
}

//Formato: Codigo, Matricula, Devolvido(0 = em aberto, 1 = devolvido)
void carregar_emprestimos() {
    FILE *arq = fopen(ARQ_EMPRESTIMOS, "r");
    if (arq == NULL)
        return;

    fscanf(arq, "%*[^\n]");

    while (1) {
        if (!garantir_cap_emprestimos()) {
            printf("\nErro: sem memoria para carregar todos os emprestimos.\n");
            break;
        }
        Emprestimo *e = &emprestimos[total_emprestimos];
        if (fscanf(arq, " %d;%19[^;];%d",
                   &e->codigo_livro, e->matricula, &e->devolvido) != 3)
            break;
        total_emprestimos++;
    }

    fclose(arq);
}

void carregar_dados() {
    carregar_livros();
    carregar_usuarios();
    carregar_emprestimos();
}

/* Retornam 1 se salvou, 0 se não conseguiu abrir o arquivo */
int salvar_livros() {
    FILE *arq = fopen(ARQ_LIVROS, "w");
    if (arq == NULL)
        return 0;

    fprintf(arq, "codigo;titulo;autor;ano;quantidade\n");
    for (int i = 0; i < total_livros; i++)
        fprintf(arq, "%d;%s;%s;%d;%d\n", livros[i].codigo, livros[i].titulo,
                livros[i].autor, livros[i].ano, livros[i].quantidade);

    fclose(arq);
    return 1;
}

int salvar_usuarios() {
    FILE *arq = fopen(ARQ_USUARIOS, "w");
    if (arq == NULL)
        return 0;

    fprintf(arq, "matricula;nome;curso\n");
    for (int i = 0; i < total_usuarios; i++)
        fprintf(arq, "%s;%s;%s\n", usuarios[i].matricula,
                usuarios[i].nome, usuarios[i].curso);

    fclose(arq);
    return 1;
}

int salvar_emprestimos() {
    FILE *arq = fopen(ARQ_EMPRESTIMOS, "w");
    if (arq == NULL)
        return 0;

    fprintf(arq, "codigo_livro;matricula;devolvido\n");
    for (int i = 0; i < total_emprestimos; i++)
        fprintf(arq, "%d;%s;%d\n", emprestimos[i].codigo_livro,
                emprestimos[i].matricula, emprestimos[i].devolvido);

    fclose(arq);
    return 1;
}

int salvar_dados() {
    int teste = 1;

    if (!salvar_livros()) {
        printf("\nErro: nao foi possivel gravar %s\n", ARQ_LIVROS);
        teste = 0;
    }
    if (!salvar_usuarios()) {
        printf("\nErro: nao foi possivel gravar %s\n", ARQ_USUARIOS);
        teste = 0;
    }
    if (!salvar_emprestimos()) {
        printf("\nErro: nao foi possivel gravar %s\n", ARQ_EMPRESTIMOS);
        teste = 0;
    }

    return teste;
}

/*Funções do sistema (a fazer)*/

void cadastrar_livro() {
    printf("\n[Cadastrar livro]\n");
    if (!garantir_cap_livros()) {
        printf("\nErro: sem memoria para cadastrar outro livro.\n");;
        return;
    }
    int temp_codigo;
    printf("Codigo numerico do Livro: ");
    scanf("%d", &temp_codigo);

    for (int i = 0; i < total_livros; i++) {
        if (livros[i].codigo == temp_codigo) {
            printf("\nErro: Ja existe um livro registado com esse codigo\n");
            return;
        }
    }

    livros[total_livros].codigo = temp_codigo;

    printf("Titulo: ");
    scanf(" %99[^\n]", livros[total_livros].titulo);

    printf("Autor: ");
    scanf(" %99[^\n]", livros[total_livros].autor);

    printf("Ano de Publicacao: ");
    scanf("%d", &livros[total_livros].ano);

    printf("Quantidade em Estoque: ");
    scanf("%d", &livros[total_livros].quantidade);

    total_livros++;
    salvar_dados();

    printf("\nLivro cadastrado com sucesso.\n");
}
void cadastrar_usuario() {
    printf("\n[Cadastrar usuario]\n");
    if (!garantir_cap_usuarios()) {
        printf("\nErro: sem memoria para cadastrar outro usuario.\n");
        return;
    }
    char temp_matricula[20];
    printf("Matricula (ex: 2026001): ");
    scanf(" %19[^\n]", temp_matricula);

    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].matricula, temp_matricula) == 0) {
            printf("\nErro: Ja existe um aluno registado com essa matricula\n");
            return;
        }
    }

    strcpy(usuarios[total_usuarios].matricula, temp_matricula);

    printf("Nome do aluno: ");
    scanf(" %99[^\n]", usuarios[total_usuarios].nome);

    printf("Curso: ");
    scanf(" %99[^\n]", usuarios[total_usuarios].curso);

    total_usuarios++;
    salvar_dados();

    printf("\nUsuario cadastrado com sucesso\n");
}
void registrar_emprestimo() {
    printf("\n[Registrar emprestimo]\n");

    if (!garantir_cap_emprestimos()) {
        printf("\nErro: sem memoria para cadastrar outro emprestimo.");
        return;
    }
    Emprestimo novo;
    int indice_livro = -1;

    printf("Codigo do livro: ");
    scanf("%d", &novo.codigo_livro);

    for (int i = 0; i < total_livros; i++) {
        if (livros[i].codigo == novo.codigo_livro) {
            indice_livro = i;
            break;
        }
    }
    if (indice_livro == -1) {
        printf("Erro: Codigo do livro nao encontrado no sistema.\n");
        return;
    }
    if (livros[indice_livro].quantidade <= 0) {
        printf("Erro: Nao ha exemplares deste livro disponiveis no momento.\n");
        return;
    }

    printf("Matrícula do Utilizador: ");
    scanf(" %19[^\n]", novo.matricula);

    int indice_usuario = -1;
    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].matricula, novo.matricula) == 0) {
            indice_usuario = i;
            break;
        }
    }
    if (indice_usuario == -1) {
        printf("Erro: Matricula nao cadastrada no sistema.\n");
        return;
    }
    novo.devolvido = 0;
    emprestimos[total_emprestimos] = novo;
    total_emprestimos++;
    livros[indice_livro].quantidade--;
    salvar_dados();
    printf("\nEmprestimo registrado.\n");
}
void registrar_devolucao() {
    printf("\n[Registrar devolucao]\n");
    int cod_livro;
    char mat_aluno[20];
    int encontrado = 0;

    printf("Código do Livro devolvido: ");
    scanf("%d", &cod_livro);

    printf("Matrícula do Utilizador: ");
    scanf(" %19[^\n]", mat_aluno);

    for(int i = 0; i < total_emprestimos; i++) {
        if (emprestimos[i].codigo_livro == cod_livro && 
            strcmp(emprestimos[i].matricula, mat_aluno) == 0 && 
            emprestimos[i].devolvido == 0) {
            emprestimos[i].devolvido = 1;
            encontrado = 1;

            for (int j = 0; j < total_livros; j++) {
                if(livros[j].codigo == cod_livro) {
                    livros[j].quantidade++;
                    break;
                }
            }
            salvar_dados();
            printf("Devolucao cadastrada com sucesso.");
            break;
        }
    }
    if (encontrado == 0){
        printf("Erro, nenhum emprestimo em aberto condizentes com esses dados.");
    }
}


//Funções de listagem
void listar_livros() {
    printf("\n[Listar livros]\n");

    /* Grava o que estiver em memoria para nao perder cadastros recentes */
    salvar_dados();

    /* Zera os contadores antes de recarregar para nao duplicar registros */
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_livros == 0) {
        printf("\nNenhum livro cadastrado em %s.\n", ARQ_LIVROS);
        return;
    }

    printf("\n%-6s | %-35s | %-25s | %-4s | %-5s\n",
           "Codigo", "Titulo", "Autor", "Ano", "Qtd");
    printf("-------+-------------------------------------+---------------------------+------+------\n");

    for (int i = 0; i < total_livros; i++) {
        printf("%-6d | %-35.35s | %-25.25s | %-4d | %-5d\n",
               livros[i].codigo, livros[i].titulo, livros[i].autor,
               livros[i].ano, livros[i].quantidade);
    }

    printf("\nTotal de livros cadastrados: %d\n", total_livros);
}

void listar_usuarios(){
    printf("\n[Listar usuarios]\n");

    /* Grava o que estiver em memoria para nao perder cadastros recentes */
    salvar_dados();

    /* Zera os contadores antes de recarregar para nao duplicar registros */
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_usuarios == 0) {
        printf("\nNenhum usuario cadastrado em %s.\n", ARQ_USUARIOS);
        return;
    }

    printf("\n%-12s | %-35s | %-25s\n",
           "Matricula", "Nome", "Curso");
    printf("-------------+-------------------------------------+---------------------------\n");

    for (int i = 0; i < total_usuarios; i++) {
        printf("%-12s | %-35s | %-25s\n",
            usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso);
    }

    printf("\nTotal de usuarios cadastrados: %d\n", total_usuarios);
}


void listar_emprestimos(){
    printf("\n[Listar emprestimos]\n");

    /* Grava o que estiver em memoria para nao perder cadastros recentes */
    salvar_dados();

    /* Zera os contadores antes de recarregar para nao duplicar registros */
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_emprestimos == 0) {
        printf("\nNenhum emprestimo cadastrado em %s.\n", ARQ_EMPRESTIMOS);
        return;
    }

    
    printf("\n%-16s | %-12s | %-11s\n",
           "Codigo do livro", "Matricula", "Devolucao");
    printf("-----------------+-------------+-------------\n");
    for (int i = 0; i < total_emprestimos; i++) {
        char *status = (emprestimos[i].devolvido == 1) ? "FEITA" : "PENDENTE";
        printf("%-16d | %-12s | %-11s\n",
            emprestimos[i].codigo_livro, emprestimos[i].matricula, status);
    }

    printf("\nTotal de emprestimos cadastrados: %d\n", total_emprestimos);
}

//Funções de busca
void buscar_por_titulo(){
    printf("\n[Buscar livro por titulo]\n"); 
    salvar_dados();

    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char titulo[100];
    printf("Titulo:\n");
    scanf("%99[^\n]", titulo);
    limpar_buffer();

    int n = 0;
    printf("\n%-6s | %-35s | %-25s | %-4s | %-5s\n",
           "Codigo", "Titulo", "Autor", "Ano", "Qtd");
    printf("-------+-------------------------------------+---------------------------+------+------\n");
    for(int i=0; i <total_livros; i++){
        if (strstr(livros[i].titulo, titulo) != NULL){
            printf("%-6d | %-35.35s | %-25.25s | %-4d | %-5d\n",
               livros[i].codigo, livros[i].titulo, livros[i].autor,
               livros[i].ano, livros[i].quantidade);
               n++;
            }
    };
    if(n == 0){
        printf("Nenhum livro encontrado");
    }
}


void buscar_por_autor()       {
    printf("\n[Buscar livro por autor]\n"); 
    salvar_dados();

    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char autor[100];
    printf("Titulo:\n");
    scanf("%99[^\n]", autor);
    limpar_buffer();

    int n = 0;

    
    printf("\n%-6s | %-35s | %-25s | %-4s | %-5s\n",
           "Codigo", "Titulo", "Autor", "Ano", "Qtd");
    printf("-------+-------------------------------------+---------------------------+------+------\n");
    for(int i=0; i <total_livros; i++){
        if (strstr(livros[i].autor, autor) != NULL){
            printf("%-6d | %-35.35s | %-25.25s | %-4d | %-5d\n",
               livros[i].codigo, livros[i].titulo, livros[i].autor,
               livros[i].ano, livros[i].quantidade);
               n++;
            }
    };
    if(n == 0){
        printf("Nenhum livro encontrado");
    }
}
void buscar_por_matricula()   { 
    printf("\n[Buscar usuario por matricula]\n");

    salvar_dados();

    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char matricula[100];
    printf("Matricula:\n");
    scanf("%99[^\n]", matricula);
    limpar_buffer();

    int n = 0;
    printf("\n%-12s | %-35s | %-25s\n",
           "Matricula", "Nome", "Curso");
    printf("-------------+-------------------------------------+---------------------------\n");
    for(int i=0; i <total_usuarios; i++){
        if (strstr(usuarios[i].matricula, matricula) != NULL){
            printf("%-12s | %-35s | %-25s\n",
            usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso);
            n++;
            }
    };
    if(n == 0){
        printf("Nenhum usuario encontrado");
    }
    
}





/*Menu Principal*/



void menu_principal(){
    int opcao = -1;
    while (opcao != 0){
        limpar_tela();
        printf("          SISTEMA DE BIBLIOTECA\n");
        printf("=============================================\n");
        printf("  CADASTROS\n");
        printf("   1 - Cadastrar livro\n");
        printf("   2 - Cadastrar usuario\n");
        printf("   3 - Registrar emprestimo\n");
        printf("   4 - Registrar devolucao\n");
        printf("---------------------------------------------\n");
        printf("  LISTAGENS\n");
        printf("   5 - Listar livros\n");
        printf("   6 - Listar usuarios\n");
        printf("   7 - Listar emprestimos\n");
        printf("---------------------------------------------\n");
        printf("  BUSCAS\n");
        printf("   8 - Buscar livro por titulo\n");
        printf("   9 - Buscar livro por autor\n");
        printf("  10 - Buscar usuario por matricula\n");
        printf("---------------------------------------------\n");
        printf("  ORDENACAO\n");
        printf("  11 - Livros por titulo (Bubble Sort)\n");
        printf("  12 - Usuarios por nome (Selection Sort)\n");
        printf("---------------------------------------------\n");
        printf("  HISTORICO\n");
        printf("  13 - Historico de um usuario\n");
        printf("  14 - Historico de todos os usuarios\n");
        printf("---------------------------------------------\n");
        printf("  BUSCA POR MULTIPLOS CRITERIOS\n");
        printf("  15 - Livros (titulo + autor + ano)\n");
        printf("  16 - Usuarios (curso + livro emprestado + situacao)\n");
        printf("---------------------------------------------\n");
        printf("   0 - Salvar e sair\n");
        printf("=============================================\n");
        printf("Escolha uma opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 1:  cadastrar_livro();      break;
            case 2:  cadastrar_usuario();    break;
            case 3:  registrar_emprestimo(); break;
            case 4:  registrar_devolucao();  break;
            case 5:  listar_livros();        break;
            case 6:  listar_usuarios();      break;
            case 7:  listar_emprestimos();   break;
            case 8:  buscar_por_titulo();    break;
            case 9:  buscar_por_autor();     break;
            case 10: buscar_por_matricula(); break;
            case 11: listar_livros_ordenados();   break;
            case 12: listar_usuarios_ordenados(); break;
            case 13: historico_por_usuario();     break;
            case 14: historico_todos_usuarios();  break;
            case 15: busca_multipla_livros();     break;
            case 16: busca_multipla_usuarios();   break;
            case 0:
                salvar_dados();
                printf("\nDados salvos. Encerrando o sistema\n");
                break;
            default:
                printf("\nOpcao invalida! Digite um numero de 0 a 16.\n");
        }

        if (opcao != 0)
            pausar();

    };
}

/*Algoritmos de ordenacao*/

/*Bubble Sort por titulo em ordem alfabetica*/
void bubble_sort_livros(Livro v[], int n) {
    int i, j, trocou;
    Livro aux;

    for (i = 0; i < n - 1; i++) {
        trocou = 0;
        for (j = 0; j < n - 1 - i; j++) {
            if (strcmp(v[j].titulo, v[j + 1].titulo) > 0) {
                aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
                trocou = 1;
            }
        }
        if (trocou == 0)
            break;
    }
}

/*Selection Sort por usuario em ordem alfabetica*/
void selection_sort_usuarios(Usuario v[], int n) {
    int i, j, menor;
    Usuario aux;

    for (i = 0; i < n - 1; i++) {
        menor = i;
        for (j = i + 1; j < n; j++) {
            if (strcmp(v[j].nome, v[menor].nome) < 0)
                menor = j;
        }
        if (menor != i) {
            aux = v[i];
            v[i] = v[menor];
            v[menor] = aux;
        }
    }
}

void listar_livros_ordenados() {
    printf("\n[Livros ordenados por titulo - Bubble Sort]\n");

    salvar_dados();
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_livros == 0) {
        printf("\nNenhum livro cadastrado em %s.\n", ARQ_LIVROS);
        return;
    }

    bubble_sort_livros(livros, total_livros);

    if (salvar_livros()){
        printf("\nOrdem gravada em %s.\n", ARQ_LIVROS);}
    else{
        printf("\nErro: nao foi possivel gravar %s\n", ARQ_LIVROS);}
    for (int i = 0; i < total_livros; i++) {
        printf("%-6d | %-35.35s | %-25.25s | %-4d | %-5d\n",
              livros[i].codigo, livros[i].titulo, livros[i].autor,
              livros[i].ano, livros[i].quantidade);
    }

    printf("\nTotal de livros cadastrados: %d\n", total_livros);

}
void listar_usuarios_ordenados() {
    printf("\n[Usuarios ordenados por nome - Selection Sort]\n");

    salvar_dados();
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_usuarios == 0) {
        printf("\nNenhum usuario cadastrado em %s.\n", ARQ_USUARIOS);
        return;
    }

    selection_sort_usuarios(usuarios, total_usuarios);
   if (salvar_usuarios()){
       printf("\nOrdem gravada em %s.\n", ARQ_USUARIOS);}
    else{
        printf("\nErro: nao foi possivel gravar %s\n", ARQ_USUARIOS);}
    printf("\n%-12s | %-35s | %-25s\n",
           "Matricula", "Nome", "Curso");
    printf("-------------+-------------------------------------+---------------------------\n");

    for (int i = 0; i < total_usuarios; i++) {
        printf("%-12s | %-35s | %-25s\n",
               usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso);
    }

    printf("\nTotal de usuarios cadastrados: %d\n", total_usuarios);

}

/*Historico por usuario*/

/* Mostra todos os emprestimos (em aberto e devolvidos) do usuario*/
void exibir_historico_usuario(int x) {
    int total = 0, em_aberto = 0, devolvidos = 0;

    printf("\nMatricula: %s\n", usuarios[x].matricula);
    printf("Nome:      %s\n", usuarios[x].nome);
    printf("Curso:     %s\n", usuarios[x].curso);

    printf("\n%-6s | %-35s | %-10s\n", "Codigo", "Titulo", "Situacao");
    printf("-------+-------------------------------------+-----------\n");

    for (int i = 0; i < total_emprestimos; i++) {
        if (strcmp(emprestimos[i].matricula, usuarios[x].matricula) != 0)
            continue;

        char *titulo = "(livro nao encontrado)";
        for (int j = 0; j < total_livros; j++) {
            if (livros[j].codigo == emprestimos[i].codigo_livro) {
                titulo = livros[j].titulo;
                break;
            }
        }

        char *status = (emprestimos[i].devolvido == 1) ? "DEVOLVIDO" : "EM ABERTO";
        printf("%-6d | %-35.35s | %-10s\n",
               emprestimos[i].codigo_livro, titulo, status);

        total++;
        if (emprestimos[i].devolvido == 1)
            devolvidos++;
        else
            em_aberto++;
    }

    if (total == 0)
        printf("Nenhum emprestimo registrado para este usuario.\n");

    printf("\nTotal: %d | Em aberto: %d | Devolvidos: %d\n",
           total, em_aberto, devolvidos);
}

/* Pede uma matricula e mostra o historico completo daquele usuario. */
void historico_por_usuario() {
    printf("\n[Historico de emprestimos por usuario]\n");

    salvar_dados();
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char matricula[20];
    printf("Matricula: ");
    scanf(" %19[^\n]", matricula);
    limpar_buffer();

    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].matricula, matricula) == 0) {
            exibir_historico_usuario(i);
            return;
        }
    }

    printf("\nErro: Matricula nao cadastrada no sistema.\n");
}

/* Mostra o historico completo de cada usuario cadastrado. */
void historico_todos_usuarios() {
    printf("\n[Historico de emprestimos de todos os usuarios]\n");

    salvar_dados();
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_usuarios == 0) {
        printf("\nNenhum usuario cadastrado em %s.\n", ARQ_USUARIOS);
        return;
    }

    for (int i = 0; i < total_usuarios; i++) {
        printf("\n=============================================\n");
        exibir_historico_usuario(i);
    }
    printf("\n=============================================\n");
    printf("Usuarios cadastrados: %d | Emprestimos registrados: %d\n",
           total_usuarios, total_emprestimos);
}


/*Busca por multiplos criterios*/

/* Le um criterio de texto opcional. Se o usuario so apertar ENTER,
   o destino fica vazio ("") e o criterio e ignorado na busca. */
void ler_criterio(char destino[100]) {
    destino[0] = '\0';
    scanf("%99[^\n]", destino);
    limpar_buffer();
}

/* Busca livros combinando titulo, autor e ano. Cada criterio deixado em
   branco e ignorado; o livro so aparece se atender TODOS os criterios
   preenchidos (E logico). Ex.: autor "Machado" + ano 1899. */
void busca_multipla_livros() {
    printf("\n[Busca de livros por multiplos criterios]\n");
    printf("Deixe em branco (ENTER) o campo que nao quiser usar.\n\n");

    salvar_dados();
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char titulo[100], autor[100], ano_txt[100];
    int ano = 0;

    printf("Titulo (ou parte): ");
    ler_criterio(titulo);
    printf("Autor (ou parte): ");
    ler_criterio(autor);
    printf("Ano de publicacao: ");
    ler_criterio(ano_txt);

    if (ano_txt[0] != '\0' && sscanf(ano_txt, "%d", &ano) != 1) {
        printf("\nErro: ano invalido.\n");
        return;
    }

    if (titulo[0] == '\0' && autor[0] == '\0' && ano_txt[0] == '\0') {
        printf("\nErro: preencha pelo menos um criterio.\n");
        return;
    }

    int n = 0;
    printf("\n%-6s | %-35s | %-25s | %-4s | %-5s\n",
           "Codigo", "Titulo", "Autor", "Ano", "Qtd");
    printf("-------+-------------------------------------+---------------------------+------+------\n");

    for (int i = 0; i < total_livros; i++) {
        if (titulo[0] != '\0' && strstr(livros[i].titulo, titulo) == NULL)
            continue;
        if (autor[0] != '\0' && strstr(livros[i].autor, autor) == NULL)
            continue;
        if (ano_txt[0] != '\0' && livros[i].ano != ano)
            continue;

        printf("%-6d | %-35.35s | %-25.25s | %-4d | %-5d\n",
               livros[i].codigo, livros[i].titulo, livros[i].autor,
               livros[i].ano, livros[i].quantidade);
        n++;
    }

    if (n == 0)
        printf("Nenhum livro atende a todos os criterios.\n");
    else
        printf("\nLivros encontrados: %d\n", n);
}

/* Busca usuarios combinando curso, livro emprestado (titulo ou parte) e
   situacao do emprestimo. Cada criterio em branco e ignorado.
   Se livro ou situacao forem informados, o usuario so aparece se tiver
   pelo menos um emprestimo que atenda aos dois; esses emprestimos sao
   listados abaixo dele. Ex.: curso "Computacao" + livro "Principe". */
void busca_multipla_usuarios() {
    printf("\n[Busca de usuarios por multiplos criterios]\n");
    printf("Deixe em branco (ENTER) o campo que nao quiser usar.\n\n");

    salvar_dados();
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char curso[100], livro[100], situacao_txt[100];
    int situacao = 0;   /* 0 = qualquer, 1 = em aberto, 2 = devolvido */

    printf("Curso (ou parte): ");
    ler_criterio(curso);
    printf("Titulo do livro emprestado (ou parte): ");
    ler_criterio(livro);
    printf("Situacao (1 = em aberto, 2 = devolvido, ENTER = qualquer): ");
    ler_criterio(situacao_txt);

    if (situacao_txt[0] != '\0' &&
        (sscanf(situacao_txt, "%d", &situacao) != 1 || situacao < 1 || situacao > 2)) {
        printf("\nErro: situacao invalida. Use 1, 2 ou ENTER.\n");
        return;
    }

    if (curso[0] == '\0' && livro[0] == '\0' && situacao == 0) {
        printf("\nErro: preencha pelo menos um criterio.\n");
        return;
    }

    int filtra_emprestimo = (livro[0] != '\0' || situacao != 0);
    int n = 0;

    for (int i = 0; i < total_usuarios; i++) {
        if (curso[0] != '\0' && strstr(usuarios[i].curso, curso) == NULL)
            continue;

        int encontrados = 0;

        for (int e = 0; e < total_emprestimos && filtra_emprestimo; e++) {
            if (strcmp(emprestimos[e].matricula, usuarios[i].matricula) != 0)
                continue;
            if (situacao == 1 && emprestimos[e].devolvido != 0)
                continue;
            if (situacao == 2 && emprestimos[e].devolvido != 1)
                continue;

            char *titulo = "(livro nao encontrado)";
            for (int j = 0; j < total_livros; j++) {
                if (livros[j].codigo == emprestimos[e].codigo_livro) {
                    titulo = livros[j].titulo;
                    break;
                }
            }
            if (livro[0] != '\0' && strstr(titulo, livro) == NULL)
                continue;

            if (encontrados == 0) {
                printf("\n%s | %s | %s\n", usuarios[i].matricula,
                       usuarios[i].nome, usuarios[i].curso);
            }
            char *status = (emprestimos[e].devolvido == 1) ? "DEVOLVIDO" : "EM ABERTO";
            printf("    %-6d | %-35.35s | %-10s\n",
                   emprestimos[e].codigo_livro, titulo, status);
            encontrados++;
        }

        if (filtra_emprestimo && encontrados == 0)
            continue;

        if (!filtra_emprestimo) {
            printf("\n%s | %s | %s\n", usuarios[i].matricula,
                   usuarios[i].nome, usuarios[i].curso);
        }
        n++;
    }

    if (n == 0)
        printf("\nNenhum usuario atende a todos os criterios.\n");
    else
        printf("\nUsuarios encontrados: %d\n", n);
}