#ifndef _LOOPBACK_H_
#define _LOOPBACK_H_

#include <stdint.h>
#include "socket.h"
#include "W7500x_wztoe.h"


/* Loopback test debug message printout enable */
#define	_LOOPBACK_DEBUG_

/* DATA_BUF_SIZE define for Loopback example */
#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			2048
#endif

/************************/
/* Select LOOPBACK_MODE */
/************************/
#define LOOPBACK_MAIN_NOBLOCK    0
#define LOOPBACK_MODE   LOOPBACK_MAIN_NOBLOCK


/* TCP server Loopback test example */
int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port);

/* TCP client Loopback test example */
int32_t loopback_tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport);

/* UDP Loopback test example */
int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port);

/* Detect_Gateway test example*/
uint8_t Detect_Gateway(void);

/* delay test example*/
void delay(volatile uint32_t );

#define D_OUTPUT1	GPIO_Pin_14
#define D_OUTPUT2	GPIO_Pin_15

#endif
