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
	static QEvt const evt = { PISCAR_LED_SIG, 0 };
	QF_PUBLISH(&evt, 0);
}
