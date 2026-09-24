#include <stdio.h>

#ifndef FUNCOES_H
#define FUNCOES_H

typedef struct{
    int codigo;
    char titulo[50];
    char autor[50];
    int ano;
    int quantidade;
}Livro;

/*Utilitários de terminal*/
void limpar_tela();


/*Funções do sistema (a implementar)*/
void cadastrar_livro();
void cadastrar_usuario();
void registrar_emprestimo();
void registrar_devolucao();
void listar_livros();
void listar_usuarios();
void listar_emprestimos();
void buscar_por_titulo();
void buscar_por_autor();
void buscar_por_matricula();
void carregar_dados();
void salvar_dados();


/*Menu Principal*/
void menu_principal();



#endif