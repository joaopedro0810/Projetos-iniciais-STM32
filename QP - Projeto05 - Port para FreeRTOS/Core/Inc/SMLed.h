/*.$file${.\Core\Inc::SMLed.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: Port para FreeRTOS.qm
* File:  ${.\Core\Inc::SMLed.h}
*
* This code has been generated by QM 5.1.0 <www.state-machine.com/qm/>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This code is covered by the following QP license:
* License #   : QP-EVAL-201231
* Issued to   : Institution or an individual evaluating the QP frameworks
* Framework(s): qpc qpcpp qpn
* Support ends: 2020-12-31
* Product(s)  :
* This license is available only for evaluation purposes and
* the generated code is still licensed under the terms of GPL.
* Please submit request for extension of the evaluaion period at:
* https://www.state-machine.com/licensing/#RequestForm
*/
/*.$endhead${.\Core\Inc::SMLed.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#ifndef SMLED_H
#define SMLED_H

#include "qf_port.h"
#include "SharedData.h"

/*.$declare${AOs::SMLed} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${AOs::SMLed} ...........................................................*/
typedef struct {
/* protected: */
    QMActive super;
} SMLed;

/* protected: */
QState SMLed_initial(SMLed * const me, void const * const par);
QState SMLed_Desligado  (SMLed * const me, QEvt const * const e);
QState SMLed_Desligado_e(SMLed * const me);
extern QMState const SMLed_Desligado_s;
QState SMLed_Ligado  (SMLed * const me, QEvt const * const e);
QState SMLed_Ligado_e(SMLed * const me);
extern QMState const SMLed_Ligado_s;
/*.$enddecl${AOs::SMLed} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

void SMLed_ctor(void);

extern QMActive * const AO_SMLed;

#endif