#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

bool stringVazia(const char * __s) {
    while(*__s) {
        if(*__s != ' ' && *__s != '\n' && *__s != '\t' && *__s != '\r') return false;

        __s++;
    }

    return true;
}

bool flagBlank(int argc, char *argv[]) {
    for(uint8_t i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-nb") == 0) return false;
    }
    return true;
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

    bool blank = flagBlank(argc, argv);

    struct dirent* entry;

    size_t linhas = 0;
    while(entry = readdir(pasta)) {
        if(entry->d_type != DT_REG) continue;

        char caminhoTotal[1024];  // strcat(strcat(strdup(path), "/"), entry->d_name);

        int n = snprintf(caminhoTotal, sizeof(caminhoTotal), "%s/%s", path, entry->d_name);

        if (n < 0 || (size_t)n >= sizeof(caminhoTotal)) {
            fprintf(stderr, "Caminho muito longo\n");
            continue;
        }

        FILE * arquivo = fopen(caminhoTotal, "r");

        if(!arquivo) {
            puts("Erro ao abrir arquivo com texto!");
            closedir(pasta);
            return 2;
        }

        char buffer[1024];

        while(fgets(buffer, sizeof(buffer), arquivo)) {
            if(!blank) if(stringVazia(buffer)) continue;
            linhas++;
        }

        fclose(arquivo);
    }

    printf("Os arquivos dessa pasta totalizam %zu linhas! Wowzers.", linhas);

    closedir(pasta);

    return 0;
}