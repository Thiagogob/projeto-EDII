// Exemplo de uso da funcao SHA256 da biblioteca Open SSL
//  Este código requer a instalação do pacote libssl-dev
//  Para compilar este código use gcc hash.c -lcrypto

#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h"    //arquivo de definição necessário função SHA256
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"
#include <stdlib.h>


//-------------------------------------------------------------------------------

typedef struct BlocoNaoMinerado
{
  unsigned int numero;
  unsigned int nonce;
  unsigned char data[184];
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
} BlocoNaoMinerado;

//-------------------------------------------------------------------------------

typedef struct BlocoMinerado
{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH];
} BlocoMinerado;

//-------------------------------------------------------------------------------

typedef struct TIndiceMinerador{
  unsigned int minerador;
  BlocoMinerado blocoDoMinerador;
}TIndiceMinerador;

//-------------------------------------------------------------------------------

typedef struct TListaIndiceNonce{
  unsigned int nonce;
  BlocoMinerado dadosBloco;
  struct TListaIndiceNonce* next;
}TListaIndiceNonce;
//-------------------------------------------------------------------------------

typedef struct HashTable{
  TListaIndiceNonce *tabelaNonces[30011];
}HashTable;

//-------------------------------------------------------------------------------
// lista que vai conter os enderecos que possuem BTC
typedef struct listaBTC
{
  unsigned int endereco;
  struct listaBTC *next;
} listaBTC;

typedef struct TBlocoETransacoes{
  unsigned int qtdTransacoes;
  BlocoMinerado bloco;
}TBlocoETransacoes;

// Funcao para criar um no na lista dos enderecos que tem BTC
listaBTC *criaNo(unsigned int endereco)
{
  listaBTC *novoEndereco = (listaBTC *)malloc(sizeof(listaBTC));
  if (novoEndereco == NULL)
  {
    printf("Alocacao de memoria falhou\n");
    return NULL;
  }
  novoEndereco->endereco = endereco;
  novoEndereco->next = NULL;
  return novoEndereco;
}
//-------------------------------------------------------------------------------
TListaIndiceNonce* criarNovoNo(unsigned int nonce, BlocoMinerado dadosBloco) {
  TListaIndiceNonce* novoNo = (TListaIndiceNonce*)malloc(sizeof(TListaIndiceNonce));
  novoNo->nonce = nonce;
  novoNo->dadosBloco = dadosBloco;
  novoNo->next = NULL;
  return novoNo;
}
//-------------------------------------------------------------------------------
void inserirNoInicioHashTable(TListaIndiceNonce** lista, unsigned int nonce, BlocoMinerado dadosBloco) {
  TListaIndiceNonce* novoNo = criarNovoNo(nonce, dadosBloco);
  novoNo->next = *lista;
  *lista = novoNo;
}
//-------------------------------------------------------------------------------
void exibirLista(TListaIndiceNonce* lista, unsigned char nonce) {
  while (lista != NULL) {
    if(lista->nonce == nonce){
      printf("\n");
      printf("=======================================\n");
      printf("Bloco: %d\n", lista->dadosBloco.bloco.numero);
      printf("Nonce: %d\n", lista->dadosBloco.bloco.nonce);
      printf("Minerador: %u\n", lista->dadosBloco.bloco.data[183]);
      printf("Hash anterior: ");
      for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        printf("%02x", lista->dadosBloco.bloco.hashAnterior[i]);

      printf("\n");
      printf("Hash valido: ");
      for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        printf("%02x", lista->dadosBloco.hash[i]);

      printf("\n");

      printf("\nTRANSACOES\n");
      for (int i = 0; i < 184; i++)
      {
        printf("[%u] - ", lista->dadosBloco.bloco.data[i]);
        if ((i + 1) % 3 == 0)
        {
          printf("\n");
        }
      }
      printf("\n=======================================\n");
    }
    // Exibir os dados do bloco, se necessário
    lista = lista->next;
  }
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Funcao para inserir um endereco na lista dos que tem BTC
void insereNoInicio(listaBTC **inicio, unsigned int endereco, int *contador)
{
  listaBTC *novoEndereco = criaNo(endereco);
  if (*inicio == NULL)
  {
    *inicio = novoEndereco;
    (*contador)++;
  }
  else
  {
    novoEndereco->next = *inicio;
    *inicio = novoEndereco;
    (*contador)++;
  }
}

//-------------------------------------------------------------------------------

// essa funcao serve para verificar se o minerador pode entrar na lista
// pois nao eh interessante a repeticao de enderecos na lista
int verificaSeMineradorPodeEntrarNaLista(listaBTC *enderecosComBTC, unsigned int endereco)
{
  listaBTC *tmp = enderecosComBTC;
  while (tmp != NULL)
  {
    if (tmp->endereco == endereco)
    {
      return 0;
    }
    tmp = tmp->next;
  }
  return 1;
}

//-------------------------------------------------------------------------------

// Funcao para printar a lista
void mostraLista(listaBTC *inicio)
{
  listaBTC *aux = inicio;
  int i=1;
  while (aux != NULL)
  {
    printf("(%d) %u - ", i, aux->endereco);
    aux = aux->next;
    i++;
  }
  printf("\n");
}

//-------------------------------------------------------------------------------

// printar hash
void printHash(unsigned char hash[], int length)
{
  int i;

  for (i = 0; i < length; ++i)
    printf("%02x", hash[i]);

  printf("\n");
}

//-------------------------------------------------------------------------------

void printarHashArquivo(FILE *pArq, unsigned char *hash)
{
  const char *FORMATO_BLOCO_MINERADO_HASH = "%02x";
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    fprintf(pArq, FORMATO_BLOCO_MINERADO_HASH, hash[i]);
  }
}

//-------------------------------------------------------------------------------


//-----------------------------MERGE SORT----------------------------------------

void merge(TBlocoETransacoes arr[], int left, int middle, int right) {
    int i, j, k;
    int n1 = middle - left + 1;
    int n2 = right - middle;

    // Cria vetores temporários
    TBlocoETransacoes L[n1], R[n2];

    // Copia os dados para os vetores temporários L[] e R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[middle + 1 + j];

    // Mescla os vetores temporários de volta em arr[l..r]
    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (L[i].qtdTransacoes <= R[j].qtdTransacoes) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes de L[], se houver algum
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copia os elementos restantes de R[], se houver algum
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(TBlocoETransacoes arr[], int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;

        // Ordena a metade esquerda
        mergeSort(arr, left, middle);

        // Ordena a metade direita
        mergeSort(arr, middle + 1, right);

        // Une as duas metades ordenadas
        merge(arr, left, middle, right);
    }
}

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------


// funcao para preencher o bloco genesis com os campos
// solicitados no enunciado do projeto
void preencheBlocoGenesis(BlocoNaoMinerado *blocoGenesis)
{

  // PREENCHENDO VETOR DATA COM ZEROS
  for (int i = 0; i < 184; i++)
  {
    blocoGenesis->data[i] = 0;
  }

  // PREENCHENDO VETOR DE HASH ANTERIOR COM ZEROS, POIS ESSE É O PRIMEIRO BLOCO
  for (int i = 0; i < (SHA256_DIGEST_LENGTH); i++)
  {
    blocoGenesis->hashAnterior[i] = 0;
  }

  // COLOCANDO A FRASE DO BLOCO GÊNESIS
  char *str = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
  strcpy(blocoGenesis->data, str);

  // DEFININDO NONCE COMO 0 E NUMERO DO BLOCO COMO 1, COMO DITO NO ENUNCIADO
  blocoGenesis->nonce = 0;
  blocoGenesis->numero = 1;
}

//-------------------------------------------------------------------------------

// inicializar a carteira do projeto com zeros
void inicializaCarteira(unsigned int *carteira)
{
  for (int i = 0; i < 256; i++)
  {
    carteira[i] = 0;
  }
}

//-------------------------------------------------------------------------------
// funcao para minerar bloco genesis
BlocoMinerado minerarBlocoGenesis(BlocoNaoMinerado *blocoGenesis, unsigned int *carteira, listaBTC **enderecosComBTC, MTRand *r, int *contador)
{

  // vetor que armazenará o resultado do hash. Tamanho definidio pela libssl
  unsigned char hash[SHA256_DIGEST_LENGTH];

  // PROCESSO PARA DEFINIR NUMERO ALETORIO QUE REPRESENTA O MINERADOR DO BLOCO:
  unsigned char minerador = genRandLong(r) % 256;
  blocoGenesis->data[183] = minerador;

  // gerando um 1° hash para o bloco
  SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);

  // gerando um novo hash e atualizando o nonce
  // ate que hajam 2 zeros hexadecimais no comeco
  // esse eh o criterio para validar um hash
  // assim sabemos se um bloco foi minerado
  while (hash[0] != 0)
  {
    blocoGenesis->nonce++;
    SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);
  }

  // Agora que o bloco foi minerado
  // Vamos passar ele para a struct de bloco minerado
  BlocoMinerado blocoGenesisMinerado;

  // os campos do bloco nao minerado sao enviados para o bloco minerado
  blocoGenesisMinerado.bloco = *blocoGenesis;

  // passando o hash que valida o bloco para a struct de bloco minerado
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    blocoGenesisMinerado.hash[i] = hash[i];
  }

  // ATUALIZA CARTEIRA DO SISTEMA DEPOIS DA VALIDAÇÃO DO BLOCO
  carteira[minerador] = 50;

  // COLOCA PRIMEIRO MINERADOR NA LISTA DE ENDERECOS COM BTC
  insereNoInicio(enderecosComBTC, minerador, contador);

  return blocoGenesisMinerado;
}

//-------------------------------------------------------------------------------
// funcao para procurar um endereco de destino na lista de quem tem BTC
unsigned char procuraEndereco(listaBTC *enderecosComBTC, int indice)
{
  listaBTC *tmp = enderecosComBTC;
  for (int i = 0; i < indice; i++)
  {
    tmp = tmp->next;
  }
  return tmp->endereco;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// funcao para remover um endereco
// usada caso algum endereco que tinha BTC tenha transferido todas que tinha
void removerEndereco(listaBTC **enderecosComBTC, unsigned char endereco, int *contador)
{

  listaBTC *tmp = *enderecosComBTC;
  listaBTC *anterior = NULL;

  if (tmp != NULL && tmp->endereco == endereco)
  {
    *enderecosComBTC = tmp->next;
    // free(tmp);
    (*contador)--;
    return;
  }

  while (tmp != NULL && tmp->endereco != endereco)
  {
    anterior = tmp;
    tmp = tmp->next;
  }

  if (tmp != NULL)
  {
    anterior->next = tmp->next;
    // free(tmp);
    (*contador)--;
  }

  return;
}

//-------------------------------------------------------------------------------

void inicializarBloco(unsigned char *hashAnterior, BlocoNaoMinerado *blocoN, int numeroBloco, MTRand *r, int *contador, listaBTC *enderecosComBTC, unsigned int *carteira, unsigned char qtdTransacoes)
{
  // preenchendo o campo do numero que representa
  blocoN->numero = numeroBloco;

  // inicializando campo nonce
  blocoN->nonce = 0;

  // PREENCHENDO VETOR DATA COM ZEROS
  for (int i = 0; i < 184; i++)
  {
    blocoN->data[i] = 0;
  }

  // preenchendo o campo de hash anterior
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    blocoN->hashAnterior[i] = hashAnterior[i];
  }

  // printf("Quantidade de transacoes no bloco: %u\n", qtdTransacoes);
  //  definindo os elementos das transacoes
  if (numeroBloco == 2)
  {
    for (int i = 0; i < qtdTransacoes; i++)
    {
      for (int j = 0; j < 3; j++)
      {

        blocoN->data[(i * 3) + j] = 140;
        j++;

        // gerar endereco de destino
        blocoN->data[(i * 3) + j] = genRandLong(r) % 256;
        j++;

        // definir quantidade de bitcoins
        unsigned char qtdBTC = (genRandLong(r) % (carteira[140] + 1));
        blocoN->data[(i * 3) + j] = qtdBTC;

        // atualizando carteira para nao haver inconsistencia
        // por exemplo um endereco que possui 10BTC estar repassar 20BTC
        // printf("quantidade na carteira[140]: %u\n", carteira[140]);
        carteira[140] = carteira[140] - qtdBTC;
      }
    }
  }
  else
  {
    for (int i = 0; i < qtdTransacoes; i++)
    {
      for (int j = 0; j < 3; j++)
      {

        // gerar endereco de origem
        int indice = genRandLong(r) % (*contador);
        unsigned char endOrigem = procuraEndereco(enderecosComBTC, indice);
        blocoN->data[(i * 3) + j] = endOrigem;
        j++;

        // gerar endereco de destino
        blocoN->data[(i * 3) + j] = genRandLong(r) % 256;
        j++;

        // definir quantidade de bitcoins
        unsigned char qtdBTC = genRandLong(r) % (carteira[endOrigem] + 1);
        blocoN->data[(i * 3) + j] = qtdBTC;

        // atualizando carteira para nao haver inconsistencia
        // por exemplo um endereco que possui 10BTC estar repassar 20BTC
        carteira[endOrigem] = carteira[endOrigem] - qtdBTC;
      }
    }
  }

  // decidindo quem será o minerador do bloco
  unsigned char minerador = genRandLong(r) % 256;
  blocoN->data[183] = minerador;
}

//-------------------------------------------------------------------------------

BlocoMinerado minerarBloco(BlocoNaoMinerado *blocoN, unsigned int *carteira, listaBTC **enderecosComBTC, int *contador, unsigned char qtdTransacoes)
{

  BlocoMinerado blocoNMinerado;

  unsigned char hash[SHA256_DIGEST_LENGTH];

  // gerando um hash para o bloco
  SHA256((unsigned char *)blocoN, sizeof(BlocoNaoMinerado), hash);

  // alterando hash e nonce ate
  // o hash ser valido
  while (hash[0] != 0)
  {
    blocoN->nonce++;
    SHA256((unsigned char *)blocoN, sizeof(BlocoNaoMinerado), hash);
  }

  // preenchendo campo hash do bloco minerado com o hash valido
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    blocoNMinerado.hash[i] = hash[i];
  }

  // atribuindo as 50 BTC para o minerador do bloco
  unsigned char minerador = blocoN->data[183];
  carteira[minerador] += 50;

  // inserindo minerador na lista dos que tem BTC
  // caso ele nao esteja
  if (verificaSeMineradorPodeEntrarNaLista(*enderecosComBTC, minerador))
  {
    insereNoInicio(enderecosComBTC, minerador, contador);
  }

  // repassando campos do blocoN para o bloco minerado
  blocoNMinerado.bloco = *blocoN;

  // atualizando a carteira apos validada as transacoes
  for (int i = 0; i < qtdTransacoes; i++)
  {
    for (int j = 0; j < 3; j++)
    {

      // agora que o bloco foi validado
      // devemos atualizar os saldos
      // dos enderecos que receberam BTC
      if (j == 1)
      {
        // printf("indice: %u\n", blocoN->data[(i*3)+1]);
        carteira[blocoN->data[(i * 3) + 1]] += blocoN->data[(i * 3) + 2];

        if (blocoN->data[(i * 3) + 2] != 0)
        {
          if (verificaSeMineradorPodeEntrarNaLista(*enderecosComBTC, blocoN->data[(i * 3) + 1]))
          {
            insereNoInicio(enderecosComBTC, blocoN->data[(i * 3) + 1], contador);
          }
        }
      }
    }
  }

  // antes de retornar o bloco minerado
  // vamos verificar a necessidade de remover
  // algum endereco da lista de enderecos com BTC
  // afinal, alguem pode ter doado todas as bitcoins que tinha num bloco
  listaBTC *tmp = *enderecosComBTC;
  while (tmp != NULL)
  {
    if (carteira[tmp->endereco] == 0)
    {
      removerEndereco(enderecosComBTC, tmp->endereco, contador);
    }
    tmp = tmp->next;
  }

  return blocoNMinerado;
}
//---------------------------------------------------------------------------------------
  void printarDadosDeUmBloco(BlocoMinerado blocoMinerado){
    printf("\n");
      printf("=======================================\n");
      printf("Bloco: %d\n", blocoMinerado.bloco.numero);
      printf("Nonce: %d\n", blocoMinerado.bloco.nonce);
      printf("Minerador: %u\n", blocoMinerado.bloco.data[183]);
      printf("Hash anterior: ");
      printHash(blocoMinerado.bloco.hashAnterior, SHA256_DIGEST_LENGTH);
      printf("Hash valido: ");
      printHash(blocoMinerado.hash, SHA256_DIGEST_LENGTH);

      printf("\nTRANSACOES\n");
      for (int i = 0; i < 184; i++)
      {
        printf("[%u] - ", blocoMinerado.bloco.data[i]);
        if ((i + 1) % 3 == 0)
        {
          printf("\n");
        }
      }
      printf("\n=======================================\n");
  }
//---------------------------------------------------------------------------------------
int binarySearchBaseadoNoNumero(BlocoMinerado *vetorBlocosMinerados, int menor, int maior, unsigned int numeroBlocoProcurado)
{
  while (menor <= maior)
  {

    int meio = menor + (maior - menor) / 2;

    if (vetorBlocosMinerados[meio].bloco.numero == numeroBlocoProcurado)
    {
      printarDadosDeUmBloco(vetorBlocosMinerados[meio]);
      return 1;
    }
    else if (vetorBlocosMinerados[meio].bloco.numero < numeroBlocoProcurado)
    {
      menor = meio + 1;
    }
    else
    {
      maior = meio - 1;
    }
  }
  return -1;
}
//---------------------------------------------------------------------------------------------------------------

//----------------------------------------- FUNÇÕES MANIPULAÇÃO DE ARQUIVOS -------------------------------------


//--------------------------------------------------------------------------------------
void escreverArquivoBinarioDeIndicesBaseadoNoNonce(BlocoMinerado *vetorBlocosMinerados, int quantidadeDeElementosParaEscrever, const char* nomeArquivo){
    FILE *arqBinario = fopen(nomeArquivo, "ab");
    if (arqBinario == NULL)
    {
      printf("\nErro ao criar o arquivo binario de indices sobre o nonce.\n");
      return;
    }

    TListaIndiceNonce vetorAux[16];

    for(int i=0; i<16; i++){
      vetorAux[i].nonce = vetorBlocosMinerados[i].bloco.nonce;
      vetorAux[i].dadosBloco = vetorBlocosMinerados[i];
      vetorAux[i].next = NULL;
    }

    size_t quantidadeElementosEscritos = fwrite(vetorAux, sizeof(TListaIndiceNonce), 16, arqBinario);

    if(quantidadeElementosEscritos != quantidadeDeElementosParaEscrever){
      printf("\nErro na escrita do arquivo binario de indices nonce\n");
      fclose(arqBinario);
      return;
    }

    fclose(arqBinario);
    return;
}
//---------------------------------------------------------------------------------------------------
void escreverArquivoBinarioDeIndicesBaseadoNoMinerador(BlocoMinerado *vetorBlocosMinerados, int quantidadeDeElementosParaEscrever, const char* nomeArquivo, unsigned int *contagemDeMineracoesPorEndereco){
    FILE *arqBinario = fopen(nomeArquivo, "ab");
    if (arqBinario == NULL)
    {
      printf("\nErro ao criar o arquivo binario de indices sobre o minerador.\n");
      return;
    }

    TIndiceMinerador vetorIndicesMineradores[16];
    for(int j=0; j<16; j++){
      vetorIndicesMineradores[j].minerador = vetorBlocosMinerados[j].bloco.data[183];
      vetorIndicesMineradores[j].blocoDoMinerador = vetorBlocosMinerados[j];
      //printf("\nMinerador: %u\n",vetorIndicesMineradores[i].minerador);
      //printf("Bloco: %u\n", vetorIndicesMineradores[i].blocoDoMinerador.bloco.numero);
    }

    for (int i = 0; i < 16; i++)
    {
      //printf("\nMinerador: %u\n",vetorIndicesMineradores[i].minerador);
      //printf("Bloco: %u\n", vetorIndicesMineradores[i].blocoDoMinerador.bloco.numero);
      contagemDeMineracoesPorEndereco[vetorIndicesMineradores[i].minerador]++;
    }
    /*
    unsigned char elementosParaEscrever[16];
    for (int i = 0; i < 16; i++) {
        elementosParaEscrever[i] = vetorBlocosMinerados[i].bloco.data[183];
    }
    */
    size_t quantidadeDeElementosEscritos = fwrite(vetorIndicesMineradores, sizeof(TIndiceMinerador), quantidadeDeElementosParaEscrever, arqBinario);
    //fread(vetorIndicesMineradores, sizeof(TIndiceMinerador), 16, arqBinario);
    /*
    for(int j=0; j<16; j++){
      printf("\nMinerador: %u\n",vetorIndicesMineradores[j].minerador);
      printf("Bloco: %u\n", vetorIndicesMineradores[j].blocoDoMinerador.bloco.numero);
    }
    */
    if (quantidadeDeElementosEscritos != quantidadeDeElementosParaEscrever)
    {
      printf("Erro na escrita do arquivo binario");
    }

  fclose(arqBinario);
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
void carregarArquivoNoncesEmRAM(const char* nomeArquivo, int quantidadeDeElementosEscritosNoArquivo){
  FILE *arqBinario = fopen(nomeArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Erro ao criar o arquivo binario de blocos minerados.\n");
    return;
  }
  
  TListaIndiceNonce *vetorLista[30011];
  for(int i=0; i<30011; i++){
    vetorLista[i] = NULL;
  }
  
  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;

  TListaIndiceNonce vetorAux[16];
  for(int chunk=0; chunk < quantidadeDeChunksEscritos; chunk++){
    fread(vetorAux, sizeof(TListaIndiceNonce), 16, arqBinario);
    for(int j=0; j<16; j++){
      int index = vetorAux[j].nonce%30011;
      inserirNoInicioHashTable(&vetorLista[index], vetorAux[j].nonce, vetorAux[j].dadosBloco);

      //insert(tabelaNonces, vetorAux[j].dadosBloco, vetorAux[j].dadosBloco.bloco.nonce);
    }
  }


  printf("Digite um Nonce: ");
  unsigned int nonceProcurado;
  scanf("%u", &nonceProcurado);
  int index = nonceProcurado%30011;
  exibirLista(vetorLista[index], nonceProcurado);
  //printf("\n%u\n", vetorLista[index]->nonce);

}
//-------------------------------------------------------------------------------
void escreverArquivoBinarioComTransacoes(TBlocoETransacoes *vetorBlocosComTransacoes, int quantidadeDeElementosEscritosNoArquivo, const char *nomeArquivo){
  FILE *arqBinario = fopen(nomeArquivo, "wb");
  if (arqBinario == NULL)
  {
    printf("Erro ao criar o arquivo binario de blocos minerados.\n");
    return;
  }

    // calculo para saber a quantidade de "chunks" escritas no arquivo
  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;

  // se nao for multiplo de 16 haverao elementos restantes
  int elementosRestantes = quantidadeDeElementosEscritosNoArquivo % 16;
  int indiceDestino=0;
  for (int chunk = 0; chunk < quantidadeDeChunksEscritos; chunk++)
  {
    fwrite(&vetorBlocosComTransacoes[indiceDestino], sizeof(TBlocoETransacoes), 16, arqBinario);
    indiceDestino+=16;
  }

  //verificar isso aqui
  if(elementosRestantes>0){
    fwrite(&vetorBlocosComTransacoes[indiceDestino], sizeof(TBlocoETransacoes), elementosRestantes, arqBinario);
  }
}
//-------------------------------------------------------------------------------
void escreverArquivoBinario(BlocoMinerado *vetorBlocosMinerados, int quantidadeDeElementosParaEscrever, const char *nomeArquivo)
{
  FILE *arqBinario = fopen(nomeArquivo, "ab");
  if (arqBinario == NULL)
  {
    printf("Erro ao criar o arquivo binario de blocos minerados.\n");
    return;
  }

  size_t quantidadeDeElementosEscritos = fwrite(vetorBlocosMinerados, sizeof(BlocoMinerado), quantidadeDeElementosParaEscrever, arqBinario);
  

  if (quantidadeDeElementosEscritos != quantidadeDeElementosParaEscrever)
  {
    printf("Erro na escrita do arquivo binario");
  }

  fclose(arqBinario);
}


//-------------------------------------------------------------------------------

void escreverArquivoTexto(BlocoMinerado *vetorBlocosMinerados)
{
  const char *FORMATO_BLOCO_MINERADO = "\nBloco: %d\nNonce: %d\nMinerador: %u\n";
  const char *FORMATO_BLOCO_MINERADO_DATA = "[%u] -";
  FILE *pArqTexto = fopen("blocos_minerados.txt", "a+");
  if (pArqTexto == NULL)
  {
    printf("Erro ao criar/abrir arquivo texto de blocos minerados.\n");
    return;
  }
  for (int i = 0; i < 16; i++)
  {
    fprintf(pArqTexto, FORMATO_BLOCO_MINERADO, vetorBlocosMinerados[i].bloco.numero, vetorBlocosMinerados[i].bloco.nonce, vetorBlocosMinerados[i].bloco.data[183]);
    fprintf(pArqTexto, "Hash anterior: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].bloco.hashAnterior);
    fprintf(pArqTexto, "\n");
    fprintf(pArqTexto, "Hash valido: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].hash);
    fprintf(pArqTexto, "\n");
    for (unsigned char j = 0; j < 184; j++)
    {
      fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA, vetorBlocosMinerados[i].bloco.data[j]);

      if ((j + 1) % 3 == 0)
      {
        fprintf(pArqTexto, "\n");
      }
    }
    fprintf(pArqTexto, "\n=======================================");
  }
  fclose(pArqTexto);
}

//--------------------------------------------------------------------------------------

void escreverArquivoTextoComGenesis(BlocoMinerado *vetorBlocosMinerados)
{
  const char *FORMATO_BLOCO_MINERADO = "Bloco: %d\nNonce: %d\nMinerador: %u\n";
  const char *FORMATO_BLOCO_MINERADO_DATA = "[%u] -";
  const char *FORMATO_BLOCO_MINERADO_DATA_GENESIS = "%c";
  FILE *pArqTexto = fopen("blocos_minerados.txt", "a+");
  if (pArqTexto == NULL)
  {
    printf("Erro ao criar/abrir arquivo texto de blocos minerados.\n");
    return;
  }
  for (int i = 0; i < 16; i++)
  {
    fprintf(pArqTexto, FORMATO_BLOCO_MINERADO, vetorBlocosMinerados[i].bloco.numero, vetorBlocosMinerados[i].bloco.nonce, vetorBlocosMinerados[i].bloco.data[183]);
    fprintf(pArqTexto, "Hash anterior: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].bloco.hashAnterior);
    fprintf(pArqTexto, "\n");
    fprintf(pArqTexto, "Hash valido: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].hash);
    fprintf(pArqTexto, "\n");
    for (unsigned char j = 0; j < 184; j++)
    {
      if (i == 0)
      {
        fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA_GENESIS, vetorBlocosMinerados[i].bloco.data[j]);
      }
      else
      {
        fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA, vetorBlocosMinerados[i].bloco.data[j]);
        if ((j + 1) % 3 == 0)
        {
          fprintf(pArqTexto, "\n");
        }
      }
    }
    fprintf(pArqTexto, "\n=======================================\n");
  }
  fclose(pArqTexto);
}

//-------------------------------------------------------------------------------------------------------------------
void imprimirDadosArquivo(const char* nomeArquivo)
{
  FILE* arquivo = fopen(nomeArquivo, "rb");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return;
  }

  TListaIndiceNonce bloco;
  //unsigned char minerador;
  for (int i = 0; i < 64; i++) {
    fread(&bloco, sizeof(TListaIndiceNonce), 1, arquivo);
    printf("Numero: %u\n", bloco.dadosBloco.bloco.numero);
    printf("Nonce: %u\n", bloco.dadosBloco.bloco.nonce);
    printf("\n\n");
  }

  fclose(arquivo);
}
//-------------------------------------------------------------------------------------------------------------------
  void imprimirDadosArquivoTeste(const char* nomeArquivo)
{
  FILE* arquivo = fopen(nomeArquivo, "rb");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return;
  }

  TIndiceMinerador index; 
  //unsigned char minerador;
  for (int i = 0; i < 16; i++) {
    fread(&index, sizeof(TIndiceMinerador), 1, arquivo);
    printf("Minerador: %u\n", index.minerador);
    printf("Nonce: %u\n", index.blocoDoMinerador.bloco.nonce);

  }

  fclose(arquivo);
}
//-------------------------------------------------------------------------------------------------------------------
void procurarBlocoEmArquivoBinario(unsigned char *nomeDoArquivo, int quantidadeDeElementosEscritosNoArquivo, unsigned int numeroBlocoProcurado)
{
  FILE *arqBinario = fopen(nomeDoArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Nao foi possivel abrir o arquivo binario para leitura\nNo contexto de realizar uma busca\n");
    return;
  }
  BlocoMinerado vetorBlocosMinerados[16];

  // calculo para saber a quantidade de "chunks" escritas no arquivo
  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;

  // se nao for multiplo de 16 haverao elementos restantes
  int elementosRestantes = quantidadeDeElementosEscritosNoArquivo % 16;

  for (int chunk = 0; chunk < quantidadeDeChunksEscritos; chunk++)
  {
    fread(vetorBlocosMinerados, sizeof(BlocoMinerado), 16, arqBinario);
    int flag = binarySearchBaseadoNoNumero(vetorBlocosMinerados, 0, 15, numeroBlocoProcurado);
    if(flag==1){
      fclose(arqBinario);
      return;
    }
  }

  fclose(arqBinario);
}
//-------------------------------------------------------------------------------------------------------------------
void carregarNBlocos(const char *nomeArquivo, unsigned quantidadeDeElementosEscritosNoArquivo, unsigned int qtdBlocosImprimir)
{
  FILE *arqBinario = fopen(nomeArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Nao foi possivel abrir o arquivo binario para leitura na funcao carregarNBlocos\n");
    return;
  }
  TBlocoETransacoes vetorBlocosMinerados[qtdBlocosImprimir];

  // calculo para saber a quantidade de "chunks" escritas no arquivo
  int quantidadeDeChunksQueDevemSerLidos = qtdBlocosImprimir / 16;

  int elementosRestantes = qtdBlocosImprimir % 16;
  int indiceDestino = 0;
  for (int chunk = 0; chunk < quantidadeDeChunksQueDevemSerLidos; chunk++)
  {
    fread(&vetorBlocosMinerados[indiceDestino], sizeof(TBlocoETransacoes), 16, arqBinario);
    indiceDestino += 16;
  }

  if (elementosRestantes > 0)
  {
    fread(&vetorBlocosMinerados[indiceDestino], sizeof(TBlocoETransacoes), elementosRestantes, arqBinario);
  }


  mergeSort(vetorBlocosMinerados, 0, qtdBlocosImprimir - 1);

  for (int i = 0; i < qtdBlocosImprimir; i++)
  {
    printf("\n");
    printf("=======================================\n");
    printf("Bloco: %d\n", vetorBlocosMinerados[i].bloco.bloco.numero);
    printf("Nonce: %d\n", vetorBlocosMinerados[i].bloco.bloco.nonce);
    printf("Minerador: %u\n", vetorBlocosMinerados[i].bloco.bloco.data[183]);
    printf("Hash anterior: ");
    printHash(vetorBlocosMinerados[i].bloco.bloco.hashAnterior, SHA256_DIGEST_LENGTH);
    printf("Hash valido: ");
    printHash(vetorBlocosMinerados[i].bloco.hash, SHA256_DIGEST_LENGTH);

    printf("\nTRANSACOES\n");
    if (i == 0)
    {
      int j = 0;
      while (vetorBlocosMinerados[i].bloco.bloco.data[j] != '\0')
      {
        printf("%c", vetorBlocosMinerados[i].bloco.bloco.data[j]);
        j++;
      }
    }
    else
    {
      for (int j = 0; j < 184; j++)
      {
        printf("[%u] - ", vetorBlocosMinerados[i].bloco.bloco.data[j]);
        if ((j + 1) % 3 == 0)
        {
          printf("\n");
        }
      }
    }

    printf("\n=======================================\n");
  }

  fclose(arqBinario);
}
//-------------------------------------------------------------------------------------------------------------------
void procurarNoArquivoDeIndicesMinerador(unsigned int enderecoMinerador, int quantidadeDeBlocos, unsigned char *nomeDoArquivo, int quantidadeDeElementosEscritosNoArquivo)
{
  FILE *arqBinario = fopen(nomeDoArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Nao foi possivel abrir o arquivo binario para leitura\nNo contexto de realizar uma busca no arquivo de indices minerador\n");
    return;
  }
  TIndiceMinerador vetorBlocosMineradores[16];

  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;
  int contadorQtdBlocos=0;
  for (int chunk = 0; chunk < quantidadeDeChunksEscritos; chunk++)
  {
    fread(vetorBlocosMineradores, sizeof(TIndiceMinerador), 16, arqBinario);
    //int flag = binarySearchBaseadoNoNumero(vetorBlocosMinerados, 0, 15, numeroBlocoProcurado);
    for(int i=0; i<16; i++){
      if(vetorBlocosMineradores[i].minerador == enderecoMinerador){
        printarDadosDeUmBloco(vetorBlocosMineradores[i].blocoDoMinerador);
        contadorQtdBlocos++;
      }
    }
    if(contadorQtdBlocos==quantidadeDeBlocos){
      fclose(arqBinario);
      return;
    }
  }
  printf("Esse endereco minerou menos que %d bloco(s)\nMas esses foram os blocos que minerou\n", quantidadeDeBlocos);

}
//-------------------------------------------------------------------------------------------------------------------
void procurarQuemMinerouMaisBlocos(const char *nomeDoArquivo, int quantidadeDeElementosEscritosNoArquivo)
{
  FILE *arqBinario = fopen(nomeDoArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Nao foi possivel abrir o arquivo binario para leitura\nNo contexto de realizar uma busca no arquivo de indices minerador\n");
    return;
  }
  TIndiceMinerador vetorBlocosMineradores[16];
  unsigned int contagemDeMineracoesPorEndereco[256];

  inicializaCarteira(contagemDeMineracoesPorEndereco);

  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;
  unsigned int maiorQtdMineracoes=0;

  for (int chunk = 0; chunk < quantidadeDeChunksEscritos; chunk++)
  {
    fread(vetorBlocosMineradores, sizeof(TIndiceMinerador), 16, arqBinario);
    // int flag = binarySearchBaseadoNoNumero(vetorBlocosMinerados, 0, 15, numeroBlocoProcurado);
    for (int i = 0; i < 16; i++)
    {
      printf("\nMinerador: %u\n",vetorBlocosMineradores[i].minerador);
      printf("Bloco: %u\n", vetorBlocosMineradores[i].blocoDoMinerador.bloco.numero);
      contagemDeMineracoesPorEndereco[vetorBlocosMineradores[i].minerador]++;
      if(contagemDeMineracoesPorEndereco[vetorBlocosMineradores[i].minerador] > maiorQtdMineracoes){
        maiorQtdMineracoes = contagemDeMineracoesPorEndereco[vetorBlocosMineradores[i].minerador];
      }
    }
  }
  for(int i=0; i<256; i++){
    if(contagemDeMineracoesPorEndereco[i] == maiorQtdMineracoes){
      printf("Endereco %u minerou a maior quantidade de blocos(%u)\n", i, maiorQtdMineracoes);
    }
  }
  fclose(arqBinario);
  return;
}
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  MTRand r = seedRand(1234567);
  int contador = 0;
  
  int quantidadeDeElementosEscritosNoArquivoBinario = 0;

  // instanciando e preenchendo bloco genesis
  // da maneira solicitada no enunciado
  BlocoNaoMinerado blocoGenesis;
  preencheBlocoGenesis(&blocoGenesis);

  // inicializando carteira conforme pedido no enunciado
  unsigned int carteira[256];
  inicializaCarteira(carteira);


  // cria lista dos enderecos com BTC
  listaBTC *enderecosComBTC = NULL;

  // cria vetor de 16 posições para armazenar blocos minerados
  BlocoMinerado vetorBlocosMinerados[16];

  // minerando o bloco genesis
  BlocoMinerado blocoGenesisMinerado = minerarBlocoGenesis(&blocoGenesis, carteira, &enderecosComBTC, &r, &contador);

  vetorBlocosMinerados[0] = blocoGenesisMinerado;
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
  int qtdBlocosMinerar;
  do{
    printf("\nDigite a quantidade de blocos que deseja minerar: ");

    scanf("%d", &qtdBlocosMinerar);

    if(qtdBlocosMinerar%16!=0){
      printf("\nPor favor digite um múltiplo de 16\n");
    }

  }while(qtdBlocosMinerar%16!=0);
  TBlocoETransacoes vetorBlocosComTransacoes[qtdBlocosMinerar];
  vetorBlocosComTransacoes[0].qtdTransacoes = 0;
  vetorBlocosComTransacoes[0].bloco = blocoGenesisMinerado;

  unsigned int contagemDeMineracoesPorEndereco[256];
  inicializaCarteira(contagemDeMineracoesPorEndereco);


  for (int i = 0; i < (qtdBlocosMinerar-1); i++)
  {

    BlocoNaoMinerado blocoN;

    // decidindo quantidade de transacoes no bloco
    unsigned char qtdTransacoes = genRandLong(&r) % 62;

    if (i == 0)
    {
      inicializarBloco(blocoGenesisMinerado.hash, &blocoN, 2, &r, &contador, enderecosComBTC, carteira, qtdTransacoes);
    }
    else
    {

      inicializarBloco(hashAnterior, &blocoN, i + 2, &r, &contador, enderecosComBTC, carteira, qtdTransacoes);
    }

    BlocoMinerado blocoNMinerado = minerarBloco(&blocoN, carteira, &enderecosComBTC, &contador, qtdTransacoes);

    int indice = (i + 1) % 16;
    vetorBlocosMinerados[indice] = blocoNMinerado;

    vetorBlocosComTransacoes[i+1].qtdTransacoes = qtdTransacoes;
    vetorBlocosComTransacoes[i+1].bloco = blocoNMinerado;

    for (int k = 0; k < SHA256_DIGEST_LENGTH; k++)
    {
      hashAnterior[k] = blocoNMinerado.hash[k];
    }


    // verificando se eh para escrever no arquivo
    if ((i + 1) % 15 == 0 && i == 14)
    {

      escreverArquivoTextoComGenesis(vetorBlocosMinerados);

      escreverArquivoBinario(vetorBlocosMinerados, 16, "blocos_minerados.bin");
      
      escreverArquivoBinarioDeIndicesBaseadoNoMinerador(vetorBlocosMinerados, 16, "arquivo_indices_minerador.bin", contagemDeMineracoesPorEndereco);
      
      escreverArquivoBinarioDeIndicesBaseadoNoNonce(vetorBlocosMinerados, 16, "arquivo_indices_nonce.bin");
      
      
      
      quantidadeDeElementosEscritosNoArquivoBinario += 16;

    }

    else if (indice == 15)
    {
      escreverArquivoTexto(vetorBlocosMinerados);
      escreverArquivoBinario(vetorBlocosMinerados, 16, "blocos_minerados.bin");
      escreverArquivoBinarioDeIndicesBaseadoNoMinerador(vetorBlocosMinerados, 16, "arquivo_indices_minerador.bin", contagemDeMineracoesPorEndereco);
      escreverArquivoBinarioDeIndicesBaseadoNoNonce(vetorBlocosMinerados, 16, "arquivo_indices_nonce.bin");
      quantidadeDeElementosEscritosNoArquivoBinario += 16;
    }


  }

  int choice;
  do
  {
    printf("--------------------------- MENU ---------------------------------------\n");
    printf("1. Imprimir endereco(s) com mais bitcoins\n");
    printf("2. Imprimir endereco(s) que minerou mais blocos\n");
    printf("3. Imprimir bloco(s) com mais transacoes\n");
    printf("4. Imprimir bloco(s) com menos transacoes\n");
    printf("5. Imprimir quantidade media de bitcoins por bloco\n");
    printf("6. Imprimir todos os campos de um bloco dado seu numero\n");
    printf("7. Imprimir todos os campos dos N primeiros blocos minerados por um endereco\n");
    printf("8. Imprimir em ordem crescente de quantidade de transacoes todos os campos dos N primeiros blocos\n");
    printf("9. Imprimir todos os campos de todos os blocos que tem um dado nonce\n");
    printf("10. Finalizar programa\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
    {
      unsigned int maiorQtdBTC=0;
      unsigned int enderecoComMaisBTC;

      //loop na carteira para saber a maior quantidade de BTC
      for(int i=0; i<256; i++){
        if(carteira[i]>maiorQtdBTC){
          maiorQtdBTC = carteira[i];
        }
      }

      //loop na carteira imprimindo todos os enderecos que possuem a maior quantia de BTC
      for(int i=0; i<256; i++){
        if(carteira[i]==maiorQtdBTC){
          printf("\n=======ENDERECO(s) COM MAIS BTC=======\n");
          printf("Endereco %u tem %uBTC\n", i, maiorQtdBTC);
        }
      }
    }
    break;
    case 2:
    {
      unsigned int maiorQtdMineracoes=0;
      for (int b = 0; b < 256; b++)
      {

          if (contagemDeMineracoesPorEndereco[b] > maiorQtdMineracoes)
          {
            maiorQtdMineracoes = contagemDeMineracoesPorEndereco[b];
          }
        
      }
      for (int i = 0; i < 256; i++)
      {
        if (contagemDeMineracoesPorEndereco[i] == maiorQtdMineracoes)
        {
          printf("Endereco %u minerou a maior quantidade de blocos(%u)\n", i, maiorQtdMineracoes);
        }
      }
    }
    break;
    case 3:
    {
      unsigned int maiorQtdTransacoes=0;
      for(int i=0; i<qtdBlocosMinerar; i++){
        if(vetorBlocosComTransacoes[i].qtdTransacoes > maiorQtdTransacoes){
          maiorQtdTransacoes = vetorBlocosComTransacoes[i].qtdTransacoes;
        }
      }
      printf("Maior quantida de transacoes: %u", maiorQtdTransacoes);
      printf("\n============HASH DESSE(s) BLOCO(s)===============\n");
      for(int i=0; i<qtdBlocosMinerar; i++){
        if(vetorBlocosComTransacoes[i].qtdTransacoes == maiorQtdTransacoes){
          printf("Hash do bloco numero %u: ", vetorBlocosComTransacoes[i].bloco.bloco.numero);
          printHash(vetorBlocosComTransacoes[i].bloco.hash, SHA256_DIGEST_LENGTH);
        }
      }
    }
    break;
    case 4:
    {
      unsigned int menorQtdTransacoes=61;
      for(int i=0; i<qtdBlocosMinerar; i++){
        if(vetorBlocosComTransacoes[i].qtdTransacoes < menorQtdTransacoes){
          menorQtdTransacoes = vetorBlocosComTransacoes[i].qtdTransacoes;
        }
      }
      printf("Menor quantida de transacoes: %u", menorQtdTransacoes);
      printf("\n============HASH DESSE(s) BLOCO(s)===============\n");
      for(int i=0; i<qtdBlocosMinerar; i++){
        if(vetorBlocosComTransacoes[i].qtdTransacoes == menorQtdTransacoes){
          printf("Hash do bloco numero %u: ", vetorBlocosComTransacoes[i].bloco.bloco.numero);
          printHash(vetorBlocosComTransacoes[i].bloco.hash, SHA256_DIGEST_LENGTH);


        }
      }
    }
    break;
    case 5:
    {
      unsigned int numeroBlocoProcurado;
      printf("Numero do bloco que deseja procurar: ");
      scanf("%u", &numeroBlocoProcurado);
      int qtdTransacoes = vetorBlocosComTransacoes[numeroBlocoProcurado-1].qtdTransacoes;
      int qtdBTCNoBloco=0;
      for(int p=0; p<qtdTransacoes; p++){
        for(int j=0; j<3; j++){
            if(j==2){
              qtdBTCNoBloco+=vetorBlocosComTransacoes[numeroBlocoProcurado-1].bloco.bloco.data[(p * 3) + j];
            }
        }
      }
      printf("Quantidade media de BTC transacionadas no bloco %d somente de endereco para endereco: %.3f\n", numeroBlocoProcurado, (float)qtdBTCNoBloco/qtdTransacoes);
      printf("Quantidade media de BTC transacionadas no bloco %d de endereco para endereco e considerando as BTC dadas ao minerador: %.3f\n", numeroBlocoProcurado, (float)(qtdBTCNoBloco+50)/qtdTransacoes);
      //como nenhuma BTC sai do sistema ou some
      //Eh valido dizer que a media de BTC por bloco eh o total de BTC no sistema
      //dividido pela quantia de blocos minerados
      /*
      int qtdTotalBTC=0;
      for(int i=0; i<256; i++){
        qtdTotalBTC+=carteira[i];
      }
      float qtdBTCPorBloco = (float)qtdTotalBTC/qtdBlocosMinerar;
      printf("\nQuantidade media de BTC por bloco: %.3f\n", qtdBTCPorBloco);
      */
      
    }
    break;
    case 6:
    {
      unsigned int numeroBlocoProcurado;
      printf("Numero do bloco que deseja procurar: ");
      scanf("%u", &numeroBlocoProcurado);
      procurarBlocoEmArquivoBinario("blocos_minerados.bin", quantidadeDeElementosEscritosNoArquivoBinario, numeroBlocoProcurado);
    }
    break;
    case 7:
    {
      unsigned int enderecoMinerador;
      int quantidadeDeBlocos;
      printf("Numero do minerador: ");
      scanf("%u", &enderecoMinerador);
      printf("Digite a quantidade de blocos minerados por esse minerador\nque voce deseja imprimir: ");
      scanf("%d", &quantidadeDeBlocos);
      procurarNoArquivoDeIndicesMinerador(enderecoMinerador, quantidadeDeBlocos, "arquivo_indices_minerador.bin", quantidadeDeElementosEscritosNoArquivoBinario);
    }
    break;
    case 8:
    {
      escreverArquivoBinarioComTransacoes(vetorBlocosComTransacoes, quantidadeDeElementosEscritosNoArquivoBinario, "blocos_minerados_e_qtd_transacoes.bin");
      unsigned int qtdBlocosImprimir;
      printf("Quantidade de blocos que deseja imprimir: ");
      scanf("%u", &qtdBlocosImprimir);
      if (qtdBlocosMinerar < qtdBlocosImprimir)
      {
        while(qtdBlocosMinerar < qtdBlocosImprimir)
        {
          printf("\nQuantidade de blocos para imprimir invalida\n");
          printf("Quantidade de blocos que deseja imprimir: ");
          scanf("%u", &qtdBlocosImprimir);
        }
      }
      carregarNBlocos("blocos_minerados_e_qtd_transacoes.bin", quantidadeDeElementosEscritosNoArquivoBinario, qtdBlocosImprimir);
    }
    break;
    case 9:
    {
  
     carregarArquivoNoncesEmRAM("arquivo_indices_nonce.bin", quantidadeDeElementosEscritosNoArquivoBinario);

    }
    break;
    case 10:
    {
      printf("\n==============PROGRAMA FINALIZADO==============\n");
    }
    break;
    default:
      printf("Opcao invalida.\n");
      break;
    }
  } while (choice != 10);

  return 0;
}
