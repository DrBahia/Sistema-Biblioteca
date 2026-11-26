#include <stdio.h>

#ifndef FUNCOES_H
#define FUNCOES_H


/*Utilitários de terminal*/
void limpar_tela();
void limpar_buffer();
void pausar();
int ler_opcao();

/*Estrutura de dados*/
void carregar_livros();
void carregar_usuarios();
void carregar_emprestimos();
void carregar_dados();
int salvar_livros();
int salvar_usuarios();
int salvar_emprestimos();
int salvar_dados();
int inicializar_dados();
int garantir_cap_livros();
int garantir_cap_usuarios();
int garantir_cap_emprestimos();
void liberar_dados();

/*Funções do sistema*/
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



/*Menu Principal*/
void menu_principal();

/*Algoritmos de ordenacao*/
void listar_livros_ordenados();
void listar_usuarios_ordenados();

#endif