#include "hashset.h"

HashSet * SetIncExt = NULL; // HashSet que armazena extensões a incluir na contagem
HashSet * SetExcExt = NULL; // HashSet que armazena extensões a excluir na contagem
HashSet * SetExcDir = NULL; // HashSet que armazena diretórios a excluir na contagem

// Função que inicializa os hash sets para flags relacionadas
//
// Argumentos:
// Sem argumentos
//
// Retorno:
// Sem retorno
void initHashSets() {
    SetIncExt = calloc(1, sizeof(HashSet)); // Inicializa hash set relacionado à flag "-ie"
    SetExcExt = calloc(1, sizeof(HashSet)); // Inicializa hash set relacionado à flag "-xe"
    SetExcDir = calloc(1, sizeof(HashSet)); // Inicializa hash set relacionado à flag "-d"
}

// Função para criar um nó já inicializado com um valor de id (char[257])
//
// Argumentos:
// 1 - A string a colocar no id do nó
//
// Retorno:
// 1 - Endereço do nó inicializado
// 2 - NULL em caso de erro
HashNode *criarNo(const char * const id) {
    HashNode * novo = calloc(1, sizeof(HashNode)); // Aloca um nó para hash set

    if(!novo) return NULL; // Se o nó não foi alocado, retorne erro

    strncpy(novo->id, id, sizeof(novo->id)); // Copia argumento id para o id do nó sem causar buffer overflow
    novo->id[256] = '\0'; // Coloca caractere terminador caso argumento id tenha mais de 256 caracteres

    return novo; // Retorna o nó inicializado
}

// Função para calcular o valor hash de uma string
//
// Argumentos:
// 1 - A string para fazer o hash
//
// Retorno:
// O índice do hash no hash set
uint64_t hash(const char * id) {
    uint64_t hash_ac = 0; // Valor acumulado do hash
    for( uint16_t i = 0 ; *id ; i++, id++ ) { // Enquanto tiver caracteres
        hash_ac += (*id << i) + *id; // Calculo do valor do hash
    }

    return hash_ac % HASHMAX; // Retorna valor do hash como índice do hash set
}

// Função que insere um nó num hash set
//
// Argumentos:
// 1 - O hash set onde adicionar
//
// Retorno:
// Sem retorno
void inserirNo(HashSet * const set, HashNode * const no) {
    if(!no) return; // Se não houver nó, encerre
    uint64_t indice = hash(no->id); // Pega índice do id passado

    HashNode * cadeia = set->hash_cont[indice]; // Pega o primeiro nó do hash relacionado

    while(cadeia) { // Enquanto tiver nós
        if(strcmp(cadeia->id, no->id) == 0) { // Se esse id já está presente no hash set
            free(no); // Delete o nó
            return; // Encerre
        }
        cadeia = cadeia->prox; // Pega próximo nó
    }

    no->prox = set->hash_cont[indice]; // Faz o nó apontar para todos os nós já encadeados no hash set nessa posição
    set->hash_cont[indice] = no; // Posiciona nó no começo da cadeia do hash na posição relacionada
}

// Função que verifica se uma string está presente num hash set
//
// Argumentos:
// 1 - O hash set onde consultar
// 2 - O id o qual procurar
//
// Retorno:
// 1 - true caso a entrada exista no hash set
// 2 - false caso a entrada não exista no hash set
bool entryExiste(const HashSet * const set, const char * const id) {
    uint64_t indice = hash(id); // Calcula posição de hash da chave

    HashNode * cadeia = set->hash_cont[indice]; // Pega o primeiro nó da cadeia desse hash

    while(cadeia) { // Enquanto tiver nós
        if(strcmp(cadeia->id, id) == 0) return true; // Caso encontre o id no hash set, retorne true
        cadeia = cadeia->prox; // Pega o próximo nó da cadeia
    }

    return false; // Retorna false se acabaram os nós e não encontrou
}

// Função para liberar um hash set da memória e zerar o ponteiro 
//
// Argumentos:
// 1 - O hash set o qual liberar
//
// Retorno:
// Sem retorno
void freeSet(HashSet ** const set) {
    for(uint16_t i = 0; i < HASHMAX; i++) { // Enquanto houver posições de hash
        HashNode * no = (*set)->hash_cont[i]; // Extrai começo da cadeia de nós
        
        while(no) { // Enquanto tiver nós
            void * tmp = no; // Salve o endereço do nó atual
            no = no->prox; // Pegue o próximo nó
            free(tmp); // Libere o nó que foi salvo
        }
    }

    free(*set); // Libera hash set
    *set = NULL; // Zera o ponteiro de fora da função
}

// Função para liberar todos os hash sets do programa
//
// Argumentos:
// Sem argumentos
//
// Retorno:
// Sem retorno
void endHashSets() {
    freeSet(&SetIncExt); // Libera hash set relacionado à flag "-ie"
    freeSet(&SetExcExt); // Libera hash set relacionado à flag "-xe"
    freeSet(&SetExcDir); // Libera hash set relacionado à flag "-d"
}
