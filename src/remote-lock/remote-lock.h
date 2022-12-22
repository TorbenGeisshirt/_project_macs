
#ifndef REMOTE_LOCK_H_
#define REMOTE_LOCK_H_

#include <stdio.h>

// Contiki
#include "contiki.h"
#include "coap-engine.h"

// LWM
#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
#include "lwm2m-object.h"
#include "lwm2m-engine.h"
#include "lwm2m-security.h"
#endif

// Logging
#include "sys/log.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

extern coap_resource_t
  //res_auth,
    res_reed,
    res_lock,
    res_buzzer;

PROCESS(remote_lock, "Re-Mote: Lock");

AUTOSTART_PROCESSES(
    &remote_lock
    );

#endif//REMOTE_LOCK_H_
