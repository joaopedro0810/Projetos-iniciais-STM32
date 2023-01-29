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
* @brief QP natvie, platform-independent, thread-safe event queue interface
* @details
* This header file must be included in all QF ports that use native QF
* event queue for active objects. Also, this file needs to be included
* in the QP/C library when the application uses QActive_defer()/
* QActive_recall(). Finally, this file is also needed when the "raw"
* thread-safe queues are used for communication between active objects
* and non-framework entities, such as ISRs, device drivers, or legacy
* code.
*/
#ifndef QEQUEUE_H
#define QEQUEUE_H

#ifndef QF_EQUEUE_CTR_SIZE

    /*! The size [bytes] of the ring-buffer counters used in the
    * native QF event queue implementation. Valid values: 1U, 2U, or 4U;
    * default 1U.
    * @details
    * This macro can be defined in the QF port file (qf_port.h) to
    * configure the ::QEQueueCtr type. Here the macro is not defined so the
    * default of 1 byte is chosen.
    */
    #define QF_EQUEUE_CTR_SIZE 1U
#endif
#if (QF_EQUEUE_CTR_SIZE == 1U)

    /*! The data type to store the ring-buffer counters based on
    * the macro #QF_EQUEUE_CTR_SIZE.
    * @details
    * The dynamic range of this data type determines the maximum length
    * of the ring buffer managed by the native QF event queue.
    */
    typedef uint8_t QEQueueCtr;
#elif (QF_EQUEUE_CTR_SIZE == 2U)
    typedef uint16_t QEQueueCtr;
#elif (QF_EQUEUE_CTR_SIZE == 4U)
    typedef uint32_t QEQueueCtr;
#else
    #error "QF_EQUEUE_CTR_SIZE defined incorrectly, expected 1U, 2U, or 4U"
#endif

/*==========================================================================*/

/*! Native QF Event Queue
* @details
* This class describes the native QF event queue, which can be used as
* the event queue for active objects, or as a simple "raw" event queue for
* thread-safe event passing among non-framework entities, such as ISRs,
* device drivers, or other third-party components.@n
* @n
* The native QF event queue is configured by defining the macro
* #QF_EQUEUE_TYPE as ::QEQueue in the specific QF port header file.@n
* @n
* The ::QEQueue structure contains only data members for managing an event
* queue, but does not contain the storage for the queue buffer, which must
* be provided externally during the queue initialization.@n
* @n
* The event queue can store only event pointers, not the whole events. The
* internal implementation uses the standard ring-buffer plus one external
* location that optimizes the queue operation for the most frequent case
* of empty queue.@n
* @n
* The ::QEQueue structure is used with two sets of functions. One set is for
* the active object event queue, which might need to block the active object
* task when the event queue is empty and might need to unblock it when
* events are posted to the queue. The interface for the native active object
* event queue consists of the following functions: QActive_post(),
* QActive_postLIFO(), and QActive_get_(). Additionally the function
* QEQueue_init() is used to initialize the queue.@n
* @n
* The other set of functions, uses ::QEQueue as a simple "raw" event
* queue to pass events between entities other than active objects, such as
* ISRs. The "raw" event queue is not capable of blocking on the get()
* operation, but is still thread-safe because it uses QF critical section
* to protect its integrity. The interface for the "raw" thread-safe queue
* consists of the following functions: QEQueue_post(),
* QEQueue_postLIFO(), and QEQueue_get(). Additionally the function
* QEQueue_init() is used to initialize the queue.
*
* @note Most event queue operations (both the active object queues and
* the "raw" queues) internally use  the QF critical section. You should be
* careful not to invoke those operations from other critical sections when
* nesting of critical sections is not supported.
*
* @sa ::QEQueue for the description of the data members
*/
typedef struct QEQueue {
    /*! pointer to event at the front of the queue.
    * @details
    * All incoming and outgoing events pass through the frontEvt location.
    * When the queue is empty (which is most of the time), the extra
    * frontEvt location allows to bypass the ring buffer altogether,
    * greatly optimizing the performance of the queue. Only bursts of events
    * engage the ring buffer.
    *
    * @note The additional role of this attribute is to indicate the empty
    * status of the queue. The queue is empty when frontEvt is NULL.
    */
    QEvt const * volatile frontEvt;

    /*! pointer to the start of the ring buffer. */
    QEvt const **ring;

    /*! offset of the end of the ring buffer from the start of the buffer. */
    QEQueueCtr end;

    /*! offset to where next event will be inserted into the buffer. */
    QEQueueCtr volatile head;

    /*! offset of where next event will be extracted from the buffer. */
    QEQueueCtr volatile tail;

    /*! number of free events in the ring buffer. */
    QEQueueCtr volatile nFree;

    /*! minimum number of free events ever in the ring buffer.
    * @details
    * this attribute remembers the low-watermark of the ring buffer,
    * which provides a valuable information for sizing event queues.
    * @sa QF_getQueueMargin().
    */
    QEQueueCtr nMin;
} QEQueue;

/* public class operations */

/*! Initialize the native QF event queue */
void QEQueue_init(QEQueue * const me,
                  QEvt const * * const qSto, uint_fast16_t const qLen);

/*! Post an event to the "raw" thread-safe event queue (FIFO). */
bool QEQueue_post(QEQueue * const me, QEvt const * const e,
                  uint_fast16_t const margin, uint_fast8_t const qs_id);

/*! Post an event to the "raw" thread-safe event queue (LIFO). */
void QEQueue_postLIFO(QEQueue * const me, QEvt const * const e,
                      uint_fast8_t const qs_id);

/*! Obtain an event from the "raw" thread-safe queue. */
QEvt const *QEQueue_get(QEQueue * const me, uint_fast8_t const qs_id);

/*! "raw" thread-safe QF event queue operation for obtaining the number
* of free entries still available in the queue.
* @details
* This operation needs to be used with caution because the number of free
* entries can change unexpectedly. The main intent for using this operation
* is in conjunction with event deferral. In this case the queue is accessed
* only from a single thread (by a single AO),  so the number of free
* entries cannot change unexpectedly.
*
* @param[in] me pointer (see @ref oop)
*
* @returns the current number of free slots in the queue.
*/
static inline QEQueueCtr QEQueue_getNFree(QEQueue * const me) {
    return me->nFree;
}

/*! "raw" thread-safe QF event queue operation for obtaining the minimum
* number of free entries ever in the queue (a.k.a. "low-watermark").
* @details
* This operation needs to be used with caution because the "low-watermark"
* can change unexpectedly. The main intent for using this operation is to
* get an idea of queue usage to size the queue adequately.
*
* @param[in] me  pointer (see @ref oop)
*
* @returns the minimum number of free entries ever in the queue since init.
*/
static inline QEQueueCtr QEQueue_getNMin(QEQueue * const me) {
    return me->nMin;
}

/*! "raw" thread-safe QF event queue operation to find out if the queue
* is empty.
* @details
* This operation needs to be used with caution because the queue status
* can change unexpectedly. The main intent for using this operation is in
* conjunction with event deferral. In this case the queue is accessed only
* from a single thread (by a single AO), so no other entity can post
* events to the queue.
*
* @param[in] me_  pointer (see @ref oop)
*
* @returns 'true' if the queue is current empty and 'false' otherwise.
*/
static inline bool QEQueue_isEmpty(QEQueue * const me) {
    return me->frontEvt == (QEvt *)0;
}

#endif /* QEQUEUE_H */
