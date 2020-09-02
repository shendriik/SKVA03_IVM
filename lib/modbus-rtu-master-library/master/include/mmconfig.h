/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/

#ifndef _MM_CONFIG_H
#define _MM_CONFIG_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Defines ------------------------------------------*/

/*! \brief If the <em>Read Input Registers</em> function should be enabled. */
#define MM_FUNC_READ_INPUT_ENABLED              ( 0 )
/*! \brief If the <em>Read Holding Registers</em> function should be enabled. */
#define MM_FUNC_READ_HOLDING_ENABLED            ( 1 )
/*! \brief If the <em>Write Single Register</em> function should be enabled. */
#define MM_FUNC_WRITE_HOLDING_ENABLED           ( 1 )
/*! \brief If the <em>Write Multiple registers</em> function should be enabled. */
#define MM_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED  ( 1 )
/*! \brief If the <em>Read Coils</em> function should be enabled. */
#define MM_FUNC_READ_COILS_ENABLED              ( 0 )
/*! \brief If the <em>Write Coils</em> function should be enabled. */
#define MM_FUNC_WRITE_COIL_ENABLED              ( 0 )
/*! \brief If the <em>Write Multiple Coils</em> function should be enabled. */
#define MM_FUNC_WRITE_MULTIPLE_COILS_ENABLED    ( 0 )
/*! \brief If the <em>Read Discrete Inputs</em> function should be enabled. */
#define MM_FUNC_READ_DISCRETE_INPUTS_ENABLED    ( 0 )

/*! \brief If the <em>Max time waiting for the first byte of the slave. */
#define MM_SLAVE_TIMEOUT_US                     (50)

#define MM_RESPONSE_TRIES_COUNT                 ( 2 )

#ifdef __cplusplus
    PR_END_EXTERN_C
#endif
#endif
