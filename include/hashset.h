#ifndef HASHSET_H
#define HASHSET_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define HASHMAX 23 // Tamanho arbitrário dos hash sets

// struct node (HashNode)
// Estrutura de um nó para hash sets
//
// Membros:
// 1 - id: char[257] = O identificador que o nó salva
// 2 - prox: struct node* = Endereço do próximo nó do hash correspondente em caso de colisão
typedef struct node {
    char id[257];
    struct node * prox;
} HashNode;

// struct set (HashSet)
// Estrutura de um hash set
//
// Membros:
// 1 - hash_cont: struct node*[HASHMAX] = O container que guarda os endereços dos nós do começo de cada hash
typedef struct set {
    HashNode *hash_cont[HASHMAX];
} HashSet;

// Função que inicializa os hash sets para flags relacionadas
//
// Argumentos:
// Sem argumentos
//
// Retorno:
// Sem retorno
void initHashSets();

// Função para criar um nó já inicializado com um valor de id (char[257])
//
// Argumentos:
// 1 - A string a colocar no id do nó
//
// Retorno:
// 1 - Endereço do nó inicializado
// 2 - NULL em caso de erro
HashNode *criarNo(const char * const id);

// Função que insere um nó num hash set
//
// Argumentos:
// 1 - O hash set onde adicionar
//
// Retorno:
// Sem retorno
void inserirNo(HashSet * const set, HashNode * const no);

// Função que verifica se uma string está presente num hash set
//
// Argumentos:
// 1 - O hash set onde consultar
// 2 - O id o qual procurar
//
// Retorno:
// 1 - true caso a entrada exista no hash set
// 2 - false caso a entrada não exista no hash set
bool entryExiste(const HashSet * const set, const char * const id);

// Função para liberar todos os hash sets do programa
//
// Argumentos:
// Sem argumentos
//
// Retorno:
// Sem retorno
void endHashSets();

#endif