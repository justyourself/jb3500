#ifndef __UARTBUS_H__
#define __UARTBUS_H__


#ifdef __cplusplus
extern "C" {
#endif













//External Functions
void swuart_Init(p_dev_uart p);
sys_res swuart_Open(uint_t nId, p_uart_para pPara);
void swuart_TxStart(uint_t nId);




#ifdef __cplusplus
}
#endif

#endif


