#include "socket.h"
#include "W7500x_wztoe.h"
void delay(volatile uint32_t );		

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int8_t socket(uint8_t sn, uint8_t protocol, uint16_t port)
{
	close(sn);
	setSn_MR(sn, protocol );
	setSn_PORT(sn,port);	
	setSn_CR(sn,Sn_CR_OPEN);
	while(getSn_CR(sn));
	delay(10);
	switch(protocol) 
	{
		case Sn_MR_TCP:
			if(getSn_SR(sn)!=SOCK_INIT)
			{
				close(sn);
        return SOCKERR_SOCKCLOSED;						
      }        						
			break;
					
		case Sn_MR_UDP:
			if(getSn_SR(sn)!=SOCK_UDP)
			{
				close(sn);
        return SOCKERR_SOCKCLOSED;						
      }
			break;
					
		default:
			break; 
	}
  return (int8_t)sn;
}	


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int8_t close(uint8_t sn)
{
  setSn_CR(sn,Sn_CR_CLOSE);
  while( getSn_CR(sn) );
  setSn_IR(sn, 0xFF);
  while(getSn_SR(sn) != SOCK_CLOSED);
  return SOCK_OK;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int8_t listen(uint8_t sn)
{
  setSn_CR(sn,Sn_CR_LISTEN);
  while(getSn_CR(sn));
  while(getSn_SR(sn) != SOCK_LISTEN)
  {
    if(getSn_CR(sn) == SOCK_CLOSED)
    {
      close(sn);
      return SOCKERR_SOCKCLOSED;
    }
  }
  return SOCK_OK;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int8_t connect(uint8_t sn, uint8_t * addr, uint16_t port)   //2015-12-20
{
  setSn_DIPR(sn,addr);
  setSn_DPORT(sn,port);
  setSn_CR(sn,Sn_CR_CONNECT);
  while(getSn_CR(sn));
  return SOCK_OK;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int8_t disconnect(uint8_t sn)
{
  setSn_CR(sn,Sn_CR_DISCON);
  while(getSn_CR(sn));
  while(getSn_SR(sn) != SOCK_CLOSED)
  {
    if(getSn_IR(sn) & Sn_IR_TIMEOUT)
    {
      close(sn);
      return SOCKERR_TIMEOUT;
		}
	}
  return SOCK_OK;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int32_t send(uint8_t sn, uint8_t * buf, uint16_t len)
{
	uint8_t tmp=0;
	while(len >(getSn_TX_FSR(sn)))
  {
		tmp = getSn_SR(sn);
		if ((tmp != SOCK_ESTABLISHED) && (tmp != SOCK_CLOSE_WAIT))
    {
      close(sn);
      return SOCKERR_SOCKSTATUS;
    }
  }
  wiz_send_data(sn, buf, len);
  setSn_CR(sn,Sn_CR_SEND);
  while(getSn_CR(sn));
  return len;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int32_t recv(uint8_t sn, uint8_t * buf)
{
	uint16_t len;
	if((len=getSn_RX_RSR(sn))<=0)
	{
		return len;
	}	
	if(len>1460)
	{
		len=1460;
  }	
	wiz_recv_data(sn, buf, len);
  setSn_CR(sn,Sn_CR_RECV);
  while(getSn_CR(sn));
  return len;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int32_t send_udp(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port)
{
	setSn_DIPR(sn,addr);
  setSn_DPORT(sn,port); 
  while(len >(getSn_TX_FSR(sn)))  
  {
		if(getSn_SR(sn) == SOCK_CLOSED)	
		{	
			return SOCKERR_SOCKCLOSED;
    }				  
  }
  wiz_send_data(sn, buf, len);
  setSn_CR(sn,Sn_CR_SEND);
  /* wait to process the command... */
  while(getSn_CR(sn));
  return len;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int32_t recv_udp(uint8_t sn, uint8_t * buf, uint8_t * addr, uint16_t *port)
{
  uint8_t  head[8];
  uint16_t len=0;
    
  if((len = getSn_RX_RSR(sn))<= 0) 
	{
		return len;
	}	
  wiz_recv_data(sn, head, 8);
  setSn_CR(sn,Sn_CR_RECV);
  while(getSn_CR(sn));
  //read peer's IP address, port number & packet length
  addr[0] = head[0];
  addr[1] = head[1];
  addr[2] = head[2];
  addr[3] = head[3];
	
  *port = head[4];
  *port = (*port << 8) + head[5];
		
	len = head[6];
  len = (len << 8) + head[7];
		
  wiz_recv_data(sn, buf, len); // data copy.
  setSn_CR(sn,Sn_CR_RECV);
  while(getSn_CR(sn));	
  return len;
}


