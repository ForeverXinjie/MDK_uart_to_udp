#ifndef __MY_DMA_H
#define	__MY_DMA_H	 

#include "W7500x.h"


/*初始化DMA数据结构用结构体*/
typedef struct
{
	uint32_t src_end_pointer;		//源地址结束指针
	uint32_t dst_end_pointer;		//目的址结束指针
	uint32_t n_minus_l;				//传输数据长度
	uint32_t src_inc;				//源地址增量
	uint32_t dst_inc;				//目的址增量
	uint32_t src_size;				//源地址数据宽度
	uint32_t dst_size;				//目的址数据宽度
	uint32_t src_port_ctrl;			//源地址数据保护位
	uint32_t dst_port_ctrl;			//目的址数据保护位
	uint32_t next_useburst;			//是否交换下次DMA周期数据结构
	uint32_t r_power;				//DMA仲裁间隔周期
	uint32_t cycle_ctrl;			//DMA循环方式	
}My_DMA_InitTypeDef;
/*--------------------------------------------*/

/**************dst_inc值****************/
#define DMA_dst_inc_byte             		((uint32_t)0	<< 30)
#define DMA_dst_inc_halfword           		((uint32_t)1	<< 30)
#define DMA_dst_inc_word             		((uint32_t)2	<< 30)
#define DMA_dst_inc_noinc             		((uint32_t)3	<< 30)
/**************src_inc值****************/
#define DMA_src_inc_byte             		((uint32_t)0	<< 26)
#define DMA_src_inc_halfword           		((uint32_t)1	<< 26)
#define DMA_src_inc_word             		((uint32_t)2	<< 26)
#define DMA_src_inc_noinc             		((uint32_t)3	<< 26)
/**************dst_size值****************/
#define DMA_dst_size_byte             		((uint32_t)0	<< 28)
#define DMA_dst_size_halfword           	((uint32_t)1	<< 28)
#define DMA_dst_size_word             		((uint32_t)2	<< 28)
#define DMA_dst_size_reserved          	 	((uint32_t)3	<< 28)
/**************src_size值****************/						//注意  dst_size和src_size的值必须相同
#define DMA_src_size_byte             		((uint32_t)0	<< 24)
#define DMA_src_size_halfword           	((uint32_t)1	<< 24)
#define DMA_src_size_word             		((uint32_t)2	<< 24)
#define DMA_src_size_reserved           	((uint32_t)3	<< 24)
/*************src_port_ctrl值[3:0]****************/
#define DMA_src_port_ctrl_privileged		((uint32_t)1    << 18)
#define DMA_src_port_ctrl_non_privileged	((uint32_t)0    << 18)
#define DMA_src_port_ctrl_bufferable		((uint32_t)1    << 19)
#define DMA_src_port_ctrl_non_bufferable	((uint32_t)0    << 19)
#define DMA_src_port_ctrl_cacheable			((uint32_t)1    << 20)
#define DMA_src_port_ctrl_non_cacheable		((uint32_t)0    << 20)
/*************src_port_ctrl值[3:0]****************/
#define DMA_dst_port_ctrl_privileged		((uint32_t)1    << 21)
#define DMA_dst_port_ctrl_non_privileged	((uint32_t)0    << 21)
#define DMA_dst_port_ctrl_bufferable		((uint32_t)1    << 22)
#define DMA_dst_port_ctrl_non_bufferable	((uint32_t)0    << 22)
#define DMA_dst_port_ctrl_cacheable			((uint32_t)1    << 23)
#define DMA_dst_port_ctrl_non_cacheable		((uint32_t)0    << 23)
/*******************R_power值**********************/
#define DMA_R_power_each_DMA_transfer		((uint32_t)0    << 14)
#define DMA_R_power_2_DMA_transfer			((uint32_t)1    << 14)
#define DMA_R_power_4_DMA_transfer			((uint32_t)2    << 14)
#define DMA_R_power_8_DMA_transfer			((uint32_t)3    << 14)
#define DMA_R_power_16_DMA_transfer			((uint32_t)4    << 14)
#define DMA_R_power_32_DMA_transfer			((uint32_t)5    << 14)
#define DMA_R_power_64_DMA_transfer			((uint32_t)6    << 14)
#define DMA_R_power_128_DMA_transfer		((uint32_t)7    << 14)
#define DMA_R_power_256_DMA_transfer		((uint32_t)8    << 14)
#define DMA_R_power_512_DMA_transfer		((uint32_t)9    << 14)
#define DMA_R_power_1024_DMA_transfer		((uint32_t)10   << 14)
/******************next_useburst值*****************/
#define DMA_next_useburst_enable			((uint32_t)1    <<  3)
#define DMA_next_useburst_disable			((uint32_t)0    <<  3)
/******************cycle_ctrl值********************/
#define DMA_cycle_Stop						((uint32_t)0x00000000)
#define DMA_cycle_Basic						((uint32_t)0x00000001)
#define DMA_cycle_Auto_request				((uint32_t)0x00000002)
#define DMA_cycle_Ping_pong					((uint32_t)0x00000003)
#define DMA_cycle_Mem_sgm_primary			((uint32_t)0x00000004)			//sgm = scatter-gather mode
#define DMA_cycle_Mem_sgm_alternate			((uint32_t)0x00000005)			//primary = primary data structure
#define DMA_cycle_Per_sgm_primary			((uint32_t)0x00000006)			//alternate = alternate data structure
#define DMA_cycle_Per_sgm_alternate			((uint32_t)0x00000007)






void my_dma_structinit(My_DMA_InitTypeDef* my_dma_initstruct,uint32_t src,uint32_t dst,uint32_t len);
void my_dma_config(uint32_t chnl_num,My_DMA_InitTypeDef* my_dma_initstruct,uint32_t src,uint32_t dst,uint32_t len);
void dma_enable(uint32_t chnl_num);


#endif

