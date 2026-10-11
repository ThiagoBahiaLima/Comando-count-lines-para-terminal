#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

// flags escritas
#define NOBLANKSSTR "-nb"
#define RECURSIVOSTR "-r"
#define CONTARTUDOSTR "-a"

// flags em números
#define NOBLANKBIT (1)
#define RECURSIVOBIT (1 << 1)
#define CONTARTUDOBIT (1 << 2)

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

// Função que retorna uma bitmask das flags ativadas pelo usuário
//
// Argumentos:
// 1 - Contangem de argumentos de entrada de programa
// 2 - Argumentos de entrada de programa
//
// Retorno:
// Uma bitmask na forma de um uint64_t
uint64_t ligarFlags(int argc, char *argv[]) {
    
    uint64_t flags = 0; // Começa sem flags ativas
    for(int i = 0; i < argc; i++) { // Enquanto tiver argumentos de entrada
        if(strcmp(argv[i], NOBLANKSSTR) == 0) flags |= NOBLANKBIT; // Liga o bit "no blank", caso argumento seja "-nb"
        if(strcmp(argv[i], RECURSIVOSTR) == 0) flags |= RECURSIVOBIT; // Liga o bit "recursivo", caso argumento seja "-r"
        if(strcmp(argv[i], CONTARTUDOSTR) == 0) flags |= CONTARTUDOBIT; // Liga o bit "contar tudo", caso argumento seja "-a"
    }

    return flags; // Retorna bitmask com bits de flag setados
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
size_t contarLinhasDir(DIR *pasta, char *path, uint64_t flags) {
    size_t linhas = 0; // Começa contagem de linhas em 0

    struct dirent *entry; // Ponteiro de entries de diretórios 
    
    while(entry = readdir(pasta)) { // Enquanto houver entries para serem extraídas (readdir passa para a próxima entry a cada chamada no mesmo DIR*)
        if(entry->d_type == DT_DIR) { // Se a entry for outro diretório
            if(!(flags & RECURSIVOBIT)) continue; // Se a flag "-r" estiver desligada, ignore diretórios

            if(strcmp(entry->d_name, ".") == 0) continue; // Ignore o diretório atual (pasta oculta)
            if(strcmp(entry->d_name, "..") == 0) continue; // Ignore o diretório pai (pasta oculta)

            if(entry->d_name[0] == '.') if(!(flags & CONTARTUDOBIT)) continue; // Se a flag "-a" estiver desligada, ignore diretórios ocultos
            
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

            linhas += contarLinhasDir(prox_pasta, caminho, flags); // Conta as linhas do próximo diretório

            closedir(prox_pasta); // Fecha diretório
            continue; // Vai pra próxima entry
        }
        
        if(entry->d_type != DT_REG) continue; // Se não for um potencial arquivo com texto, ignore (binários tbm são lidos btw)

        if(entry->d_name[0] == '.') if(!(flags & CONTARTUDOBIT)) continue; // Se a flag "-a" estiver desligada, ignore arquivos ocultos

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

    uint64_t flags = ligarFlags(argc, argv); // Recolhe a bitmask de flags

    size_t linhas = contarLinhasDir(pasta, path, flags); // conta as linhas totais do diretório requisitado

    printf("Os arquivos dessa pasta totalizam %zu linhas! Wowzers.", linhas); // Fala quantas linhas foram contadas

    closedir(pasta); // Fecha diretório

    return 0; // Sucesso
}