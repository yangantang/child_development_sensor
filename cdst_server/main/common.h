/*
 * common global declarations
 */
#ifndef COMMON_H
#define COMMON_H

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <AWS.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
// The remote service we wish to connect to.
static BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("0d563a58-196a-48ce-ace2-dfec78acc814");

/* AP and AWS configs */
#define EAP_USERNAME	"ece445"
#define EAP_PASSWORD 	"iLuv4ples"

extern AWS *aws;

#endif	//COMMON_H