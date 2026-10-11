#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define NOBLANKS "-nb"
#define RECURSIVO "-r"
#define CONTARTUDO "-a"

bool stringVazia(const char * __s) {
    while(*__s) {
        if(*__s != ' ' && *__s != '\n' && *__s != '\t' && *__s != '\r') return false;

        __s++;
    }

    return true;
}

uint64_t ligarFlags(int argc, char *argv[]) {
    uint64_t flags = 0;
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], NOBLANKS) == 0) flags |= 1;
        if(strcmp(argv[i], RECURSIVO) == 0) flags |= (1 << 1);
        if(strcmp(argv[i], CONTARTUDO) == 0) flags |= (1 << 2);
    }

    return flags;
}

size_t contarLinhasDir(DIR *pasta, char *path, uint64_t flags) {
    size_t linhas = 0;

    struct dirent *entry;
    
    while(entry = readdir(pasta)) {
        if(entry->d_type == DT_DIR) {
            if(!(flags & (1 << 1))) continue;

            if(strcmp(entry->d_name, ".") == 0) continue;
            if(strcmp(entry->d_name, "..") == 0) continue;

            if(entry->d_name[0] == '.') if(!(flags & (1 << 2))) continue;
            
            char caminho[4096];

            int n = snprintf(caminho, sizeof(caminho), "%s/%s", path, entry->d_name);

            if (n < 0 || (size_t)n >= sizeof(caminho)) {
                fprintf(stderr, "Caminho muito longo\n");
                continue;
            }

            DIR * prox_pasta = opendir(caminho);

            if(!prox_pasta) {
                perror("opendir");
                continue;
            }

            linhas += contarLinhasDir(prox_pasta, caminho, flags);

            closedir(prox_pasta);
            continue;
        }
        
        if(entry->d_type != DT_REG) continue;

        char caminhoTotal[4096];

        int n = snprintf(caminhoTotal, sizeof(caminhoTotal), "%s/%s", path, entry->d_name);

        if (n < 0 || (size_t)n >= sizeof(caminhoTotal)) {
            fprintf(stderr, "Caminho muito longo\n");
            continue;
        }

        FILE * arquivo = fopen(caminhoTotal, "r");

        if(!arquivo) {
            perror("fopen");
            continue;
        }

        char buffer[4096];

        while(fgets(buffer, sizeof(buffer), arquivo)) {
            if(flags & 1) if(stringVazia(buffer)) continue;
            linhas++;
        }

        fclose(arquivo);
    }

    return linhas;
}

int main(int argc, char *argv[]) {

    char *path = argc > 1 ? argv[1] : ".";

    DIR * pasta = opendir(path);

    if(!pasta) {
        pasta = opendir(".");
        path = ".";
    }

    if(!pasta) {
        perror("opendir");
        return 1;
    }

    uint64_t flags = ligarFlags(argc, argv);

    size_t linhas = contarLinhasDir(pasta, path, flags);

    printf("Os arquivos dessa pasta totalizam %zu linhas! Wowzers.", linhas);

    closedir(pasta);

    return 0;
}