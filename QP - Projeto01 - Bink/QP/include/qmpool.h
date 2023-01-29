/*============================================================================
* QP/C Real-Time Embedded Framework (RTEF)
* Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
*
* SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
*
* This software is dual-licensed under the terms of the open source GNU
* General Public License version 3 (or any later version), or alternatively,
* under the terms of one of the closed source Quantum Leaps commercial
* licenses.
*
* The terms of the open source GNU General Public License version 3
* can be found at: <www.gnu.org/licenses/gpl-3.0>
*
* The terms of the closed source Quantum Leaps commercial licenses
* can be found at: <www.state-machine.com/licensing>
*
* Redistributions in source code must retain this top-level comment block.
* Plagiarizing this software to sidestep the license obligations is illegal.
*
* Contact information:
* <www.state-machine.com>
* <info@state-machine.com>
============================================================================*/
/*!
* @date Last updated on: 2021-12-23
* @version Last updated for: @ref qpc_7_0_0
*
* @file
* @brief QP native, platform-independent memory pool ::QMPool interface.
*/
#ifndef QMPOOL_H
#define QMPOOL_H

/*==========================================================================*/
#ifndef QF_MPOOL_SIZ_SIZE
    /*! macro to override the default ::QMPoolSize size [bytes].
    * Valid values 1U, 2U, or 4U; default 2U
    */
    #define QF_MPOOL_SIZ_SIZE 2U
#endif
#if (QF_MPOOL_SIZ_SIZE == 1U)

    /*! The data type to store the block-size based on the macro
    * #QF_MPOOL_SIZ_SIZE.
    * @details
    * The dynamic range of this data type determines the maximum size
    * of blocks that can be managed by the native QF event pool.
    */
    typedef uint8_t QMPoolSize;
#elif (QF_MPOOL_SIZ_SIZE == 2U)

    typedef uint16_t QMPoolSize;
#elif (QF_MPOOL_SIZ_SIZE == 4U)
    typedef uint32_t QMPoolSize;
#else
    #error "QF_MPOOL_SIZ_SIZE defined incorrectly, expected 1U, 2U, or 4U"
#endif

/*==========================================================================*/
#ifndef QF_MPOOL_CTR_SIZE
    /*! macro to override the default ::QMPoolCtr size [bytes].
    * Valid values 1U, 2U, or 4U; default 2U
    */
    #define QF_MPOOL_CTR_SIZE 2U
#endif
#if (QF_MPOOL_CTR_SIZE == 1U)

    /*! The data type to store the block-counter based on the macro
    * #QF_MPOOL_CTR_SIZE.
    * @details
    * The dynamic range of this data type determines the maximum number
    * of blocks that can be stored in the pool.
    */
    typedef uint8_t QMPoolCtr;
#elif (QF_MPOOL_CTR_SIZE == 2U)
    typedef uint16_t QMPoolCtr;
#elif (QF_MPOOL_CTR_SIZE == 4U)
    typedef uint32_t QMPoolCtr;
#else
    #error "QF_MPOOL_CTR_SIZE defined incorrectly, expected 1U, 2U, or 4U"
#endif

/*==========================================================================*/
/*! Native QF Memory Pool
* @details
* A fixed block-size memory pool is a very fast and efficient data
* structure for dynamic allocation of fixed block-size chunks of memory.
* A memory pool offers fast and deterministic allocation and recycling of
* memory blocks and is not subject to fragmenation.@n
* @n
* The ::QMPool class describes the native QF memory pool, which can be used as
* the event pool for dynamic event allocation, or as a fast, deterministic
* fixed block-size heap for any other objects in your application.
*
* @note
* ::QMPool contains only data members for managing a memory pool, but
* does not contain the pool storage, which must be provided externally
* during the pool initialization.
*
* @note
* The native QF event pool is configured by defining the macro
* #QF_EPOOL_TYPE_ as ::QMPool in the specific QF port header file.
*/
typedef struct {
    /*! The head of the linked list of free blocks */
    void * volatile free_head;

    /*! the original start this pool */
    void *start;

    /*! the last memory block managed by this memory pool */
    void *end;

    /*!  maximum block size (in bytes) */
    QMPoolSize blockSize;

    /*! total number of blocks */
    QMPoolCtr nTot;

    /*! number of free blocks remaining */
    QMPoolCtr volatile nFree;

    /*! minimum number of free blocks ever present in this pool
    * @details
    * this attribute remembers the low watermark of the pool, which
    * provides a valuable information for sizing event pools.
    * @sa QF_getPoolMin().
    */
    QMPoolCtr nMin;
} QMPool;

/* public functions: */

/*! Initializes the native QF memory pool */
void QMPool_init(QMPool * const me, void * const poolSto,
                 uint_fast32_t poolSize, uint_fast16_t blockSize);

/*! Obtains a memory block from a memory pool. */
void *QMPool_get(QMPool * const me, uint_fast16_t const margin,
                 uint_fast8_t const qs_id);

/*! Recycles a memory block back to a memory pool. */
void QMPool_put(QMPool * const me, void *b,
                uint_fast8_t const qs_id);

/*! Memory pool element to allocate correctly aligned storage
* for QMPool class.
* @param[in] evType_ event type (name of the subclass of QEvt)
*/
#define QF_MPOOL_EL(evType_) \
    struct { void *sto_[((sizeof(evType_) - 1U)/sizeof(void*)) + 1U]; }

#endif /* QMPOOL_H */
