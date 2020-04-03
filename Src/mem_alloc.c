#include "mem_alloc.h"
#include <string.h>

#define MEM_ALLOC 1
#if defined (MEMALLOC)&&MEM_ALLOC 
#define alloc_printf 		printf
#else
#define alloc_printf(argv,...)
#endif

#define MEM_SIZE			2*1024   //�ڴ�صĴ�СΪ2kb

static char mem[MEM_SIZE];			//�������������ڴ������

#define MEM_START  &mem[0]				//�����ڴ�ص��׵�ַ
#define MEM_END	   &mem[MEM_SIZE]		//�����ڴ�ص�β��ַ


enum  USE_STA{						//�����ڴ���ʹ��״̬(UNUSEDδʹ�ã�����USEDΪʹ�ã�
	UNUSED=0,
	USED=1
};

#pragma pack(1)
typedef struct mem_block{			//�����ڴ���������ݽṹ
	void 	*mem_ptr;				//��ǰ�ڴ��ĵ�ַ
	struct mem_block *nxt_ptr;		//��һ���ڴ�����ĵ�ַ
	unsigned int mem_size;			//��ǰ�ڴ���С
	enum USE_STA mem_sta;			//��ǰ�ڴ��ʹ��״̬
}mem_block;
#pragma pack()

	
#define BLK_SIZE sizeof(mem_block)	//�ڴ�����Ĵ�С
#define HEAD_NODE	(MEM_END-BLK_SIZE)//ͷ�ڴ�����ĵ�ַ


static signed char mem_init_flag = -1;//�ڴ����ϵͳ��ʼ���ı�־(-1 δ��ʼ��),(1 �ѳ�ʼ��)

//�ڴ����ϵͳ��ʼ��
void mem_init(void)
{
	mem_block *node;
	memset(mem ,0x00UL,  sizeof(mem));
	node=(mem_block   *)HEAD_NODE ;
	node->mem_ptr=		MEM_START;
	node->nxt_ptr=		(mem_block   *)HEAD_NODE;
	node->mem_size=		MEM_SIZE-BLK_SIZE;
	node->mem_sta	= 		UNUSED;
	
	mem_init_flag=1;
}

//�ڴ����뺯��
static void *malloc(unsigned nbytes)
{
	unsigned int suit_size = 0xFFFFFFFFUL;
	mem_block	*head_node=NULL ,*tmp_node=NULL , *suit_node=NULL;
	
	if(nbytes == 0)
	{
		alloc_printf("�����Ƿ�\r\n");
		return NULL;
	}
	if(mem_init_flag<0)
	{
		alloc_printf("δ��ʼ�����ȳ�ʼ��\r\n");
		mem_init();
	}
	
	head_node=tmp_node =(mem_block *)HEAD_NODE;
	while(1)
	{
		if(tmp_node ->mem_sta == UNUSED)
		{
			if(nbytes <=tmp_node->mem_size&&tmp_node->mem_size<suit_size)
			{
				suit_node=tmp_node ;
				suit_size=suit_node->mem_size;
			}
		}
		tmp_node=tmp_node ->nxt_ptr;
		if(tmp_node==head_node)
		{
			if(suit_node==NULL)
			{
				alloc_printf("NULL!\r\n");
				return NULL;
			}
			break;
		}
	}
	if(nbytes <= suit_node->mem_size && (nbytes + BLK_SIZE) >= suit_node->mem_size)
	{
		suit_node->mem_sta = USED;
		return suit_node->mem_ptr;
	}
	else if(suit_node->mem_size > (nbytes + BLK_SIZE))
	{
		tmp_node = suit_node->mem_ptr;
		tmp_node = (mem_block *)((unsigned char *)tmp_node + nbytes);
		tmp_node->mem_ptr = suit_node->mem_ptr;
		tmp_node->nxt_ptr = suit_node->nxt_ptr;
		tmp_node->mem_size = nbytes;
		tmp_node->mem_sta= USED;

		suit_node->mem_ptr  = (mem_block *)((unsigned char *)tmp_node + BLK_SIZE);
		suit_node->nxt_ptr  = tmp_node;
		suit_node->mem_size -= (nbytes + BLK_SIZE);
		suit_node->mem_sta  = UNUSED;

		return tmp_node->mem_ptr;
		}
		else
		{
			alloc_printf("%s,size err!\r\n",__FUNCTION__);
		}

		return NULL;
}
//�ڴ��ͷź���
static void free(void *ap)
{
	mem_block     *head_node, *tmp_node, *nxt_node;
	if(ap == NULL)
		return;
	if(mem_init_flag < 0)
	{
		return;
	}
	head_node = tmp_node = (mem_block *)HEAD_NODE;
	 while(1)
	 {
		 if(tmp_node->mem_ptr == ap)
		 {
			 if(tmp_node->mem_sta != UNUSED)
			 {
				 tmp_node->mem_sta = UNUSED;
				 break;
			 }
			 else
			 {
				 alloc_printf("ap:0x%08x �Ѿ��ͷ�,�����ٴ��ͷ�\r\n",ap);
				return;
			 }
		 }
		 tmp_node = tmp_node->nxt_ptr;
		 if(tmp_node == head_node)
		 {
			 alloc_printf("%s,can not found ap!\r\n",__FUNCTION__);
			 return ;
		 }
	 }
	 AGAIN:
	 head_node = tmp_node = (mem_block *)HEAD_NODE;
	 while(1)
	 {
		 nxt_node = tmp_node->nxt_ptr;
		 if(nxt_node == head_node)
		 {
			 break;
		 }
		 if(tmp_node->mem_sta == UNUSED && nxt_node->mem_sta == UNUSED)
		 {
			 tmp_node->mem_ptr     = nxt_node->mem_ptr;
			 tmp_node->nxt_ptr     = nxt_node->nxt_ptr;
			 tmp_node->mem_size += nxt_node->mem_size + BLK_SIZE;
			 tmp_node->mem_sta    =  UNUSED;
			 
			 goto AGAIN;
		 }
		  tmp_node = nxt_node;
	 }
 }

 void *m_malloc(unsigned nbytes)
 {
	 return malloc(nbytes);
 }
 void m_free(void *ap)
 {
	 free(ap);
 }




















	
