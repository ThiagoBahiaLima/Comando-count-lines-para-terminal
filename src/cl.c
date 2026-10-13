#define _DEFAULT_SOURCE // Para ter acesso aos macros de tipo de entry de diretório
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "hashset.h"

static uint64_t flags = 0; // Variável com capacidade para armazenar 64 flags

extern HashSet * SetIncExt; // HashSet que armazena extensões a incluir na contagem
extern HashSet * SetExcExt; // HashSet que armazena extensões a excluir na contagem
extern HashSet * SetExcDir; // HashSet que armazena diretórios a excluir na contagem

// flags escritas
#define NOBLANKSSTR "-nb"           
#define RECURSIVOSTR "-r"           
#define CONTARTUDOSTR "-a"          
#define MEDIASTR "-m"               // a fazer
#define INCEXTENSAOSTR "-ie"
#define EXCEXTENSAOSTR "-xe"
#define EXCLUIRDIRSTR "-d"  

// flags em números
#define NOBLANKBIT (1 << 0)
#define RECURSIVOBIT (1 << 1)
#define CONTARTUDOBIT (1 << 2)
#define MEDIABIT (1 << 3)
#define INCEXTENSAOBIT (1 << 4)
#define EXCEXTENSAOBIT (1 << 5)
#define EXCLUIRDIRBIT (1 << 6)

// Função que verifica se uma string só possui caracteres "inválidos" (espaço, quebra de linha, tab e '\r' (windows))
//
// Argumentos:
// A string a ser verificada
//
// Retorno:
// 1 - true em caso de string "vazia"
// 2 - false caso tenha pelo menos um caractere válido
bool stringVazia(const char * __s) {
    while(*__s) { // enquanto não for '\0'

        // se não for nenhum caractere inválido, ent é um válido, logo, não está vazia
        if(*__s != ' ' && *__s != '\n' && *__s != '\t' && *__s != '\r') return false;

        __s++; // próximo caractere
    }

    return true; // Se saiu do while ent é tudo inválido
}

// Função que acha a posição última ocorrência de um caractere especificado
//
// Argumentos:
// 1 - A string onde procurar
// 2 - O caractere a procurar
//
// Retorna:
// 1 - A posição onde achou o caractere
// 2 - -1 caso não encontre o caractere
int32_t acharUltChar(const char* string, const char alvo) {
    const char * const pos_inv = string - 1; // Guarda o endereço imediatamente anterior ao começo da string

    string += (strlen(string) - 1); // Salta para o começo da string

    while(string != pos_inv) { // Enquanto estiver numa posição válida
        if(*string == alvo) return string - (pos_inv + 1); // Calcula posição do caractere
        string--; // Confere o caractere imediatamente anterior caso não tenha encontrado
    }

    return -1; // Retorna -1 se não tiver encontrado
}

// Função que retorna uma bitmask das flags ativadas pelo usuário
//
// Argumentos:
// 1 - Contangem de argumentos de entrada de programa
// 2 - Argumentos de entrada de programa
//
// Retorno:
// Uma bitmask na forma de um uint64_t
void ligarFlags(int argc, char *argv[]) {
    for(int i = 0; i < argc; i++) { // Enquanto tiver argumentos de entrada
        if(strcmp(argv[i], NOBLANKSSTR) == 0) flags |= NOBLANKBIT; // Liga o bit "no blank", caso argumento seja "-nb"
        if(strcmp(argv[i], RECURSIVOSTR) == 0) flags |= RECURSIVOBIT; // Liga o bit "recursivo", caso argumento seja "-r"
        if(strcmp(argv[i], CONTARTUDOSTR) == 0) flags |= CONTARTUDOBIT; // Liga o bit "contar tudo", caso argumento seja "-a"
        if(strcmp(argv[i], MEDIASTR) == 0) flags |= MEDIABIT; // Liga o bit "média", caso argumento seja "-m"
        if(strcmp(argv[i], INCEXTENSAOSTR) == 0) flags |= INCEXTENSAOBIT; // Liga o bit "incluir extensão", caso argumento seja "-ie"
        if(strcmp(argv[i], EXCEXTENSAOSTR) == 0) flags |= EXCEXTENSAOBIT; // Liga o bit "excluir extensão", caso argumento seja "-xe"
        if(strcmp(argv[i], EXCLUIRDIRSTR) == 0) flags |= EXCLUIRDIRBIT; // Liga o bit "excluir diretório", caso argumento seja "-d"
    }
}

// Função que conta as linhas escritas em todos os arquivos de um diretório
//
// Argumentos:
// 1 - Ponteiro para o diretório onde procurar arquivos
// 2 - O path inteiro até o diretório citado acima
// 3 - A bitmask de flags
//
// Retorno:
// Quantas linhas há no todo de arquivos do diretório passado como parâmetro
uint64_t contarLinhasDir(DIR *pasta, char *path) {
    uint64_t linhas = 0; // Começa contagem de linhas em 0

    struct dirent *entry; // Ponteiro de entries de diretórios 
    
    while(entry = readdir(pasta)) { // Enquanto houver entries para serem extraídas (readdir passa para a próxima entry a cada chamada no mesmo DIR*)
        if(entry->d_type == DT_DIR) { // Se a entry for outro diretório
            if(!(flags & RECURSIVOBIT)) continue; // Se a flag "-r" estiver desligada, ignore diretórios

            if(strcmp(entry->d_name, ".") == 0) continue; // Ignore o diretório atual (pasta oculta)
            if(strcmp(entry->d_name, "..") == 0) continue; // Ignore o diretório pai (pasta oculta)

            if(entry->d_name[0] == '.') if(!(flags & CONTARTUDOBIT)) continue; // Se a flag "-a" estiver desligada, ignore diretórios ocultos

            if(flags & EXCLUIRDIRBIT) if(entryExiste(SetExcDir, entry->d_name)) continue; // Se a flag "-d" estiver ativa, checa se nome de diretório está registrado e pula se estiver
            
            char caminho[4096]; // Buffer para guardar caminho total até próximo diretório

            int n = snprintf(caminho, sizeof(caminho), "%s/%s", path, entry->d_name); // Tenta colocar o path inteiro do próximo diretório no buffer mencionado acima

            if (n < 0 || (size_t)n >= sizeof(caminho)) { // Se falhar na leitura ou ultrapassar o tamanho do buffer, ignore diretório
                fprintf(stderr, "Caminho muito longo\n");
                continue;
            }

            DIR * prox_pasta = opendir(caminho); // Tenta abrir próximo diretório para recursão

            if(!prox_pasta) { // Caso não tenha aberto
                perror("opendir");
                continue;
            }

            linhas += contarLinhasDir(prox_pasta, caminho); // Conta as linhas do próximo diretório

            closedir(prox_pasta); // Fecha diretório
            continue; // Vai pra próxima entry
        }
        
        if(entry->d_type != DT_REG) continue; // Se não for um potencial arquivo com texto, ignore (binários tbm são lidos btw)

        if(entry->d_name[0] == '.') if(!(flags & CONTARTUDOBIT)) continue; // Se a flag "-a" estiver desligada, ignore arquivos ocultos

        {
            int32_t ponto = -1; // Armazenará a posição dos começos de extensões

            if(flags & INCEXTENSAOBIT) { // Se a flag "-ie" estiver ativa, leia apenas arquivos com extensões que foram incluídos
                if((ponto = acharUltChar(entry->d_name, '.')) != -1) { // Acha a posição do começo da extensão do arquivo e vê se extensão existe no nome
                    if(!entryExiste(SetIncExt, entry->d_name + ponto)) continue; // Pula arquivo se extensão não estiver registrada
                }
                else continue; // Pula arquivo se ele não tem extensão
            }

            if(flags & EXCEXTENSAOBIT) { // Se a flag "-xe" estiver ativa, leia apenas arquivos com extensões que foram incluídos
                if((ponto = acharUltChar(entry->d_name, '.')) != -1) { // Acha a posição do começo da extensão do arquivo e vê se extensão existe no nome
                    if(entryExiste(SetExcExt, entry->d_name + ponto)) continue; // Pula o arquivo se a extensão estiver registrada
                }
            }
        }

        char caminhoArquivo[4096]; // Buffer para guardar caminho total até próximo arquivo

        int n = snprintf(caminhoArquivo, sizeof(caminhoArquivo), "%s/%s", path, entry->d_name); // Tenta colocar o path inteiro do próximo arquivo no buffer mencionado acima

        if (n < 0 || (size_t)n >= sizeof(caminhoArquivo)) { // Se falhar na leitura ou ultrapassar o tamanho do buffer, ignore arquivo
            fprintf(stderr, "Caminho muito longo\n");
            continue;
        }

        FILE * arquivo = fopen(caminhoArquivo, "r"); // tenta abrir arquivo para contagem

        if(!arquivo) { // Se falhar a abertura, ignore arquivo
            perror("fopen");
            continue;
        }

        char buffer[4096]; // Buffer só para ler linha inteira do arquivo (espero que não tenha uma linha com mais de 4095 caracteres, kapa kapa)

        while(fgets(buffer, sizeof(buffer), arquivo)) { // Enquanto tiver linhas para ler
            if(flags & NOBLANKBIT) if(stringVazia(buffer)) continue; // Se a flag "-nb" estiver ligada, ignore linhas vazias
            linhas++; // Conte a linha
        }

        fclose(arquivo); // Feche arquivo (evitar memory leak é legal)
    }

    return linhas; // Retorne linhas contadas
}

// Essa é a main \o/
int main(int argc, char *argv[]) {

    char *path = argc > 1 ? argv[1] : "."; // O diretório a ser analisado deve sempre ser o primeiro argumento da execução 
                                           // Caso o usuário queira especificar o diretório, claro

    DIR * pasta = opendir(path); // tenta abrir o suposto diretório passado

    if(!pasta) { // Se não for um diretório válido, abra o diretório atual e atualize a variável path de acordo
        pasta = opendir(".");
        path = ".";
    }

    if(!pasta) { // Caso ainda assim não tenha aberto o diretório, reze e encerre a chamada
        perror("opendir");
        return 1;
    }

    ligarFlags(argc, argv); // Recolhe a bitmask de flags

    initHashSets(); // Inicializa os hash sets para as flags "-ie", "-xe" e "-d"

    if(flags & INCEXTENSAOBIT) { // Se a flag "-ie" estiver ativa, registre inclusões

        uint16_t i = 0; // Índice para navegar pelo argv[] 
        while(strcmp(argv[i++], INCEXTENSAOSTR) != 0); // As inclusões são registradas logo após a flag relacionada

        while(i < argc && argv[i][0] != '-') { // Enquanto não chegar ao fim do argc e enquanto não achar outra flag 
            inserirNo(SetIncExt, criarNo(argv[i++])); // Registre as entradas no hash set correspondente
        }
    }

    if(flags & EXCEXTENSAOBIT) { // Se a flag "-xe" estiver ativa, registre exclusões

        uint16_t i = 0; // Índice para navegar pelo argv[]
        while(strcmp(argv[i++], EXCEXTENSAOSTR) != 0); // As exclusões são registradas logo após a flag relacionada

        while(i < argc && argv[i][0] != '-') { // Enquanto não chegar ao fim do argc e enquanto não achar outra flag
            inserirNo(SetExcExt, criarNo(argv[i++])); // Registre as entradas no hash set correspondente
        }
    }

    if(flags & EXCLUIRDIRBIT) { // Se a flag "-d" estiver ativa, registre exclusões

        uint16_t i = 0; // Índice para navegar pelo argv[]
        while(strcmp(argv[i++], EXCLUIRDIRSTR) != 0); // As exclusões são registradas logo após a flag relacionada

        while(i < argc && argv[i][0] != '-') { // Enquanto não chegar ao fim do argc e enquanto não achar outra flag
            inserirNo(SetExcDir, criarNo(argv[i++])); // Registre as entradas no hash set correspondente
        }
    }

    uint64_t linhas = contarLinhasDir(pasta, path); // conta as linhas totais do diretório requisitado

    printf("Os arquivos dessa pasta totalizam %zu linhas! Wowzers.", linhas); // Fala quantas linhas foram contadas

    closedir(pasta); // Fecha diretório

    endHashSets(); // Libera todos os hash sets da memória

    return 0; // Sucesso
}