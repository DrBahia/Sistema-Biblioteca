#include <stdio.h>
#include "funcoes.h"
#include <stdlib.h>



/*Utilitários de terminal*/

void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void limpar_buffer(void) {
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

#define MAX_LIVROS      500
#define MAX_USUARIOS    500
#define MAX_EMPRESTIMOS 1000

#define ARQ_LIVROS      "livros.csv"
#define ARQ_USUARIOS    "usuarios.csv"
#define ARQ_EMPRESTIMOS "emprestimos.csv"

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

Livro      livros[MAX_LIVROS];
Usuario    usuarios[MAX_USUARIOS];
Emprestimo emprestimos[MAX_EMPRESTIMOS];

int total_livros      = 0;
int total_usuarios    = 0;
int total_emprestimos = 0;

/* ---------- Persistência em arquivos CSV ---------- */
/* Formato: uma linha de cabeçalho e depois um registro por linha,
   com os campos separados por ';'*/


//Formato: Codigo, Titulo, Autor, Ano, Quantidade
void carregar_livros() {
    FILE *arq = fopen(ARQ_LIVROS, "r");
    if (arq == NULL)
        return;     /* primeira execução: arquivo ainda não existe */

    fscanf(arq, "%*[^\n]");     /* pula o cabeçalho */

    while (total_livros < MAX_LIVROS) {
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

    while (total_usuarios < MAX_USUARIOS) {
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

    while (total_emprestimos < MAX_EMPRESTIMOS) {
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
    if (total_livros >= MAX_LIVROS) {
        printf("\nErro: Limite máximo de %d livros.", MAX_LIVROS);
        return;
    }
    Livro novo;
    printf("Código do livro: ");
    scanf("%d", &novo.codigo);

    for (int i = 0; i < total_livros; i++) {
        if (livros[i].codigo == novo.codigo) {
            printf("Ja existe um livro registrado com esse codigo.");
            return;
        }
    }
    printf("Titulo: ");
    scanf(" %99[^\n]", novo.titulo);

    printf("Autor: ");
    scanf(" %99[^\n]", novo.autor);

    printf("Ano de Publicacao: ");
    scanf("%d", &novo.ano);

    printf("Quantidade em Estoque: ");
    scanf("%d", &novo.quantidade);

    livros[total_livros] = novo;
    total_livros++;

    printf("\nLivro cadastrado com sucesso.");
}
void cadastrar_usuario() {
    printf("\n[Cadastrar usuario]\n");
    if (total_usuarios >= MAX_USUARIOS) {
        printf("\nErro, Limite máximo de %d usuários", MAX_USUARIOS);
        return;
    }
    Usuario novo;

    printf("Matrícula: (ex: 00008886)");
    scanf("%19[^\n]", novo.matricula);

    for (int i = 0; i < total_usuarios; i++) {
        if(strcmp(usuarios[i].matricula, novo.matricula) == 0) {
            printf("Erro, ja esixte um aluno registrado com essa matricula");
            return;
        }
    }

    printf("Nome do aluno: ");
    scanf(" %99[^\n]", novo.nome);

    printf("Curso: ");
    scanf(" %99[^\n]", novo.curso);

    usuarios[total_usuarios] = novo;
    total_usuarios++;
}
void registrar_emprestimo() {
    printf("\n[Registrar emprestimo]\n");

    if (total_emprestimos >= MAX_EMPRESTIMOS) {
        printf("\nErro, Limite maximo de emprestimos atingido.");
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

    novo.devolvido = 0;
    emprestimos[total_emprestimos] = novo;
    total_emprestimos++;
    livros[indice_livro].quantidade--;

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
            printf("Devolucao cadastrada com sucesso.");
            break;
        }
    }
    if (encontrado == 0){
        printf("Erro, nenhum emprestimo em aberto condizentes com esses dados.");
    }
}
void listar_livros()          { printf("\n[Listar livros]\n"); }
void listar_usuarios()        { printf("\n[Listar usuarios]\n"); }
void listar_emprestimos()     { printf("\n[Listar emprestimos]\n"); }
void buscar_por_titulo()      { printf("\n[Buscar livro por titulo]\n"); }
void buscar_por_autor()       { printf("\n[Buscar livro por autor]\n"); }
void buscar_por_matricula()   { printf("\n[Buscar usuario por matricula]\n"); }



/*Menu Principal*/
void menu_principal(){
    int opcao;
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
            case 0:
                salvar_dados();
                printf("\nDados salvos. Encerrando o sistema\n");
                break;
            default:
                printf("\nOpcao invalida! Digite um numero de 0 a 10.\n");
        }

        if (opcao != 0)
            pausar();

    };
}





