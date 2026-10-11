# `cl` — Count Lines

Um comando simples para **contar linhas** de arquivos pelo terminal.

---

## ⚙️ Instalação

### 1. Compile o programa

No diretório onde está o arquivo `cl.c`, execute:

```bash
gcc cl.c -o cl
```

> Você pode substituir `cl` pelo nome que preferir para o executável.

### 2. Mova o executável para `/usr/bin`

```bash
sudo mv cl /usr/bin
```

O `sudo` é necessário porque `/usr/bin` é uma pasta protegida do sistema.

### 3. Execute o comando

Depois da instalação, você pode chamar o programa pelo nome escolhido:

```bash
cl
```

---

## 🧰 Opções de uso

- **Diretório como entrada:** passe o nome de um diretório, sem `/` no final, para especificar onde contar as linhas.
- **Ignorar linhas em branco:** use a opção `-nb` (*no blank*) para desconsiderar linhas em branco.
- **Recursão:** use a opção `-r` para percorrer também os subdiretórios da pasta informada, contando as linhas dos arquivos encontrados neles.

### Exemplos

```bash
# Usar o comando
cl

# Contar linhas em um diretório
cl minha_pasta

# Ignorar linhas em branco
cl -nb

# Contar linhas nos subdiretórios também
cl minha_pasta -r

# Contar linhas recursivamente, ignorando linhas em branco
cl minha_pasta -r -nb
```

> **Atenção:** se você passar uma pasta como entrada, ela deve ser o **primeiro argumento**. As flags podem ser informadas depois dela.

---

## 📌 Resumo

| Recurso | Descrição |
| --- | --- |
| `gcc cl.c -o cl` | Compila o programa |
| `sudo mv cl /usr/bin` | Instala o executável em `/usr/bin` |
| `-nb` | Ignora linhas em branco |
| `-r` | Percorre os subdiretórios recursivamente |
| Diretório como argumento | Define a pasta em que contar as linhas |

---

Feito para facilitar a contagem de linhas direto pelo terminal. 🙂