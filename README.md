# Streaming de Vídeo

Implementação de um straming de vídeo baseado em um protocolo próprio descrito posteriormente. Não se trata estritamente de streaming, pois não acontece uma transmissão contínua com visualização progressiva.

O sistema desenvolvido consiste em dois elementos, um cliente e um servidor. Nesse caso, o mesmo programa serve para ambas funcionalidades, distinguindo-as pelos parâmetros.

O servidor e o cliente executam em máquinas distintas, conectadas por um cabo de rede.

Uma vez conectados e iniciados cliente e servidor, no cliente é mostrado uma lista de títulos de vídeos disponíveis no servidor, em mp4 e/ou avi. O usuário, no cliente, pode escolher um dos vídeos. A partir da escolha, o vídeo é transferido para o cliente. Após a transferência, o cliente chama um player para a exibição iniciar.

O vídeo será transmitido no navegador através do MPlayer

### Tecnologias Utilizadas

* C

## Como rodar o projeto ✅

Descreva o passo a passo necessário para rodar sua aplicação. Lembre-se: a pessoa nunca rodou seu projeto. Não tenha medo de detalhar o máximo possível. Isso é necessário!

Uma boa forma de descrever o passo a passo é:

```
Comando 1
```

Depois, rode o seguinte comando:

```
Comando 2
```

Deixe claro como a pessoa pode confirmar que a aplicação está rodando da forma correta. Pode ser com prints ou a mensagem que ela deve esperar.

## Como rodar os testes

Explique como rodar os testes da aplicação. Exemplo de um comando usando Makefile para rodar os testes:

```
make test
```

## 📌 (Título) - Informações importantes sobre a aplicação (exemplo) 📌

As máquinas devem operar em modo root e devem estar interligadas por cabo de rede diretamente.

A comunicação é baseada em raw sockets.

O frame deve ter o seguinte formato

* Marcador de início: 0111 1110 (8 bits)
* Tamanho em bytes (6 bits - Tamanho da área de dados)
* Sequência (5 bits)
* Tipo (5 bits)
* Dados (0 a 63 bytes)
* CRC-8 (8 bits)

O controle de fluxo é por janela deslizante, go-back N (volta N), com tamanho de janela fixo em 5 mensagens nas transmissões de arquivos e em uma mensagem (para-espera) nas demais mensagens.

O nome de cada arquivo pode ter no máximo 63 bytes, possuindo somente caracteres ASCII na faixa de 0x20 0x7E.

Os tipos de mensagens (campo "Tipo") são:

* 00000 ack
* 00001 nack
* 01010 lista
* 01011 baixar
* 10000 mostra na tela
* 10001 descritor no arquivo
* 10010 dados
* 11110 fim tx
* 11111 erro

  Os erros reportados são:

  * 1 acesso negado
  * 2 não encontrado
  * 3 disco cheio
