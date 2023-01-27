#ifndef VITIS_SYSTEM_PROTOCOL_H
#define VITIS_SYSTEM_PROTOCOL_H
#define __USE_MSI__
#ifdef __USE_MSI__
#include "msi_protocol.h"
#elif __USE_MSI_ACK__
// This is the protocol used with FwdAck on unordered interconnect
#include "msi_ack_protocol.h"
#elif __USE_MESI__
#error "MESI not implemented"
#endif

#endif //VITIS_SYSTEM_PROTOCOL_H
