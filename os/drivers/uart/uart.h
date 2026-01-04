#ifndef __UART_H__
#define __UART_H__

enum uart_config_parity {
	UART_CFG_PARITY_NONE,   /**< No parity */
	UART_CFG_PARITY_ODD,    /**< Odd parity */
	UART_CFG_PARITY_EVEN,   /**< Even parity */
	UART_CFG_PARITY_MARK,   /**< Mark parity */
	UART_CFG_PARITY_SPACE,  /**< Space parity */
};

/** @brief Number of stop bits. */
enum uart_config_stop_bits {
	UART_CFG_STOP_BITS_0_5,  /**< 0.5 stop bit */
	UART_CFG_STOP_BITS_1,    /**< 1 stop bit */
	UART_CFG_STOP_BITS_1_5,  /**< 1.5 stop bits */
	UART_CFG_STOP_BITS_2,    /**< 2 stop bits */
};

/** @brief Number of data bits. */
enum uart_config_data_bits {
	UART_CFG_DATA_BITS_5,    /**< 5 data bits */
	UART_CFG_DATA_BITS_6,    /**< 6 data bits */
	UART_CFG_DATA_BITS_7,    /**< 7 data bits */
	UART_CFG_DATA_BITS_8,    /**< 8 data bits */
	UART_CFG_DATA_BITS_9,    /**< 9 data bits */
};

struct uart_data {
  struct device *dev;
  int baudrate;
  enum uart_config_parity parity;
  enum uart_config_stop_bits stop_bits;
  enum uart_config_data_bits data_bits;
};

#endif/*__UART_H__*/
