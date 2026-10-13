# `cl` --- Count Lines

Um comando simples para **contar linhas de arquivos pelo terminal**.

------------------------------------------------------------------------

## ⚙️ Instalação

### 1. Prepare a pasta de compilação

Como o GitHub não mantém diretórios vazios, a pasta `build/` pode não
estar presente após clonar ou baixar o repositório. Na raiz do projeto,
crie-a antes de começar:

``` bash
mkdir build
```

> Se a pasta `build/` já existir, não é necessário executar esse comando
> novamente.

### 2. Compile o programa com CMake

Entre na pasta de compilação e configure o projeto usando o
`CMakeLists.txt` que está na raiz:

``` bash
cd build
cmake ..
cmake --build .
```

Ao final da compilação, o executável `cl` será gerado dentro da pasta
`build/`.

### 3. Instale o executável

No Arch Linux, você pode movê-lo para `/usr/bin` para executá-lo de
qualquer diretório. Execute o comando a partir da pasta `build/`:

``` bash
sudo mv cl /usr/bin
```

O `sudo` é necessário porque `/usr/bin` é uma pasta protegida do
sistema.

### 4. Execute o comando

Depois da instalação, use:

``` bash
cl
```

------------------------------------------------------------------------

## 🧰 Opções de uso

-   **Diretório como entrada:** passe o nome de um diretório, sem `/` no
    final, para especificar onde contar as linhas.
-   **Ignorar linhas em branco:** use `-nb` (*no blank*) para
    desconsiderar linhas em branco.
-   **Recursão:** use `-r` para percorrer também os subdiretórios da
    pasta informada.
-   **Arquivos ocultos:** use `-a` para incluir arquivos ocultos na
    contagem. No Linux, arquivos e diretórios cujos nomes começam com
    `.` são considerados ocultos.
-   **Incluir extensões específicas:** use `-ie` seguido imediatamente
    pelas extensões que devem ser consideradas. Quando essa opção é
    usada, arquivos com extensões que não foram listadas são ignorados.
    É possível informar várias extensões.
-   **Excluir extensões específicas:** use `-xe` seguido imediatamente
    pelas extensões que devem ser desconsideradas. É possível informar
    várias extensões.
-   **Excluir diretórios da recursão:** use `-d` seguido imediatamente
    pelos nomes dos diretórios que devem ser ignorados. Essa opção é
    destinada ao uso com `-r` e aceita vários diretórios.

### Atenção ao usar `-ie`, `-xe` e `-d`

Os valores dessas opções devem aparecer **imediatamente após a flag**.
Você pode listar vários valores em sequência.

Nos exemplos abaixo, `.c`, `.h` e `.md` são extensões, e `build` e
`vendor` são nomes de diretórios:

``` bash
# Contar apenas arquivos .c e .h
cl minha_pasta -ie .c .h

# Excluir arquivos .md e .txt
cl minha_pasta -xe .md .txt

# Percorrer subdiretórios, ignorando build e vendor
cl minha_pasta -r -d build vendor

# Contar recursivamente apenas arquivos .c e .h
cl minha_pasta -r -ie .c .h

# Combinar recursão, arquivos ocultos e exclusão de diretórios
cl minha_pasta -r -a -d build vendor
```

> **Atenção:** se você passar uma pasta como entrada, ela deve ser o
> **primeiro argumento**. As flags podem ser informadas depois dela. Ao
> usar `-ie`, `-xe` ou `-d`, coloque os valores correspondentes
> imediatamente após a flag.

------------------------------------------------------------------------

## 📌 Resumo

  -----------------------------------------------------------------------
  Recurso                             Descrição
  ----------------------------------- -----------------------------------
  `mkdir build`                       Cria a pasta de compilação

  `cd build`                          Entra na pasta de compilação

  `cmake ..`                          Configura o projeto a partir do
                                      `CMakeLists.txt`

  `cmake --build .`                   Compila o projeto

  `sudo mv cl /usr/bin`               Instala o executável em `/usr/bin`
                                      (execute a partir de `build/`)

  `-nb`                               Ignora linhas em branco

  `-r`                                Percorre os subdiretórios
                                      recursivamente

  `-a`                                Inclui arquivos ocultos na contagem

  `-ie .c .h`                         Considera apenas as extensões
                                      informadas

  `-xe .md .txt`                      Exclui as extensões informadas

  `-d build vendor`                   Ignora os diretórios informados
                                      durante a recursão

  Diretório como argumento            Define a pasta em que contar as
                                      linhas
  -----------------------------------------------------------------------

------------------------------------------------------------------------

Feito para facilitar a contagem de linhas direto pelo terminal. 🙂
