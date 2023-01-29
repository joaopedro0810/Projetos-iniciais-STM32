/*
 * SMLedActions.c
 *
 *  Created on: Jun 19, 2022
 *      Author: joaop
 */


#include "main.h"
#include "qf_port.h"
#include "SharedData.h"

void PublicarEvento(void)
{
	EvtPiscaLED *evt = Q_NEW(EvtPiscaLED, PISCAR_LED_SIG);
	evt->quantidade = 2;

	QACTIVE_POST((QActive *const) AO_SMConsumer, (QEvt *)evt, 0);
}
