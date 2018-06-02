#include "my_dac.h"
#include "W7500x_dma.h"


uint32_t dma_datalen;
extern dma_data_structure *dma_data;

void my_dma_structinit(My_DMA_InitTypeDef* my_dma_initstruct,uint32_t src,uint32_t dst,uint32_t len)
{
	uint8_t len_t = 4;
	my_dma_initstruct->src_end_pointer = src;
	my_dma_initstruct->dst_end_pointer = dst;
	my_dma_initstruct->n_minus_l = len<<len_t;
	
	my_dma_initstruct->src_size = DMA_src_size_byte  ;
	my_dma_initstruct->dst_size = DMA_dst_size_byte  ;
	my_dma_initstruct->src_inc = DMA_src_inc_byte ;
	my_dma_initstruct->dst_inc = DMA_dst_inc_noinc;
	
	my_dma_initstruct->dst_port_ctrl = DMA_dst_port_ctrl_non_privileged|DMA_dst_port_ctrl_non_bufferable|DMA_dst_port_ctrl_non_cacheable;
	my_dma_initstruct->src_port_ctrl = DMA_src_port_ctrl_non_privileged|DMA_src_port_ctrl_non_bufferable|DMA_src_port_ctrl_non_cacheable;
	
	my_dma_initstruct->r_power = DMA_R_power_16_DMA_transfer;
	
	my_dma_initstruct->next_useburst = DMA_next_useburst_disable;
	
	my_dma_initstruct->cycle_ctrl = DMA_cycle_Auto_request;
	
}

void my_dma_config(uint32_t chnl_num,My_DMA_InitTypeDef* my_dma_initstruct,uint32_t src,uint32_t dst,uint32_t len)
{
	unsigned long src_end_pointer=0;
	unsigned long dst_end_pointer=0;
	unsigned long control=0;
	
	dma_datalen = len;
	
	dma_data_struct_init();
	dma_init();
	my_dma_structinit(my_dma_initstruct,src,dst,len);
	
	src_end_pointer = my_dma_initstruct->src_end_pointer + (1<<0) * (len-1);
	dst_end_pointer = my_dma_initstruct->dst_end_pointer;
	
	control =	my_dma_initstruct->src_size 	| my_dma_initstruct->dst_size	 	|
				my_dma_initstruct->src_inc  	| my_dma_initstruct->dst_inc  		|
				my_dma_initstruct->src_port_ctrl| my_dma_initstruct->dst_port_ctrl	|
				my_dma_initstruct->r_power		| my_dma_initstruct->next_useburst	| my_dma_initstruct->cycle_ctrl|
				my_dma_initstruct->n_minus_l
	;
	
	dma_data->Primary[chnl_num].SrcEndPointer  = src_end_pointer;
	dma_data->Primary[chnl_num].DestEndPointer = dst_end_pointer;
	dma_data->Primary[chnl_num].Control		   = control;
	
	DMA->CHNL_PRIORITY_SET 	= (1<<chnl_num); /*SET channel privorty high*/
//	DMA->CHNL_ENABLE_SET 	= (1<<chnl_num); /* Enable channel */
	DMA->CHNL_SW_REQUEST 	= (1<<chnl_num); /* request channel DMA */
	
}


void dma_enable(uint32_t chnl_num)
{
	DMA->CHNL_ENABLE_SET 	= (0<<chnl_num);
	dma_data->Primary[chnl_num].Control |= dma_datalen<<4;
	DMA->CHNL_ENABLE_SET 	= (1<<chnl_num);
}
