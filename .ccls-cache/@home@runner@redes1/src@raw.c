#include "define.h"

int openRSocket(char *interface){

  int rsocket;
  struct packet_mreq mreq;
  struct ifreq ifr;
  struct sockaddr_ll local;

  if((rsocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    return -1;

  memset(&ifr, 0, sizeof(ifr)); //aloca um espaço para os dados da interface

  strcpy(ifr.ifr_name, interface); //copia o nome da interface para o ifr

  if(ioctl(rsocket, SIOCGIFINDEX, &ifr) < 0) //obtem o indice da interface
    return -2;

  /* Liga o socket a interface */
  memset(&local, 0, sizeof(local)); //aloca um espaço para os dados do socket

  local.sll_family = AF_PACKET; //familia do protocolo
  local.sll_protocol = htons(ETH_P_ALL); //protocolo
  local.sll_ifindex = ifr.ifr_ifindex; //indice da interface

  if(bind(rsocket, (struct sockaddr *)&local, sizeof(local)) < 0) //liga o socket a interface
    return -3;  

  /* Liga o modo promiscuo (escuta a rede)*/
  memset(&mreq, 0, sizeof(mreq)); //aloca um espaço para os dados do socket
  mreq.mr_ifindex = ifr.ifr_ifindex; //indice da interface
  mreq.mr_type = PACKET_MR_PROMISC; //tipo de pacote

  if(setsockopt(rsocket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) //liga o socket a interface
    return -4;

  return rsocket;
    
}