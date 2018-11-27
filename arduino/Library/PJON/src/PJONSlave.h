
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| v11.1
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2018 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

PJONSlave has been created by Giovanni Blu Mitolo with the support
of Fred Larsen and is inspired by the work of Thomas Snaidero:
 "Modular components for eye tracking, in the interest of helping persons with
  severely impaired motor skills."
Master Thesis, IT University of Copenhagen, Denmark, September 2016

PJON® Dynamic addressing specification:
- v1.0 specification/PJON-dynamic-addressing-specification-v1.0.md

The PJON project is entirely financed by contributions of people like you and
its resources are solely invested to cover the development and maintenance
costs, consider to make donation:
- Paypal:   https://www.paypal.me/PJON
- Bitcoin:  1FupxAyDTuAMGz33PtwnhwBm4ppc7VLwpD
- Ethereum: 0xf34AEAF3B149454522019781668F9a2d1762559b
Thank you and happy tinkering!
 _____________________________________________________________________________

This software is experimental and it is distributed "AS IS" without any
warranty, use it at your own risk.

Copyright 2010-2018 by Giovanni Blu Mitolo gioscarab@gmail.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once
#include "PJON.h"

template<typename Strategy = SoftwareBitBang>
class PJONSlave : public PJON<Strategy> {
  public:
    uint8_t required_config =
      PJON_ACK_REQ_BIT | PJON_TX_INFO_BIT | PJON_CRC_BIT;

    /* PJONSlave bus default initialization:
       State: Local (bus_id: 0.0.0.0)
       Acknowledge: true (Acknowledge is requested)
       device id: PJON_NOT_ASSIGNED (255)
       Mode: PJON_HALF_DUPLEX
       Sender info: true (Sender info are included in the packet)
       Strategy: SoftwareBitBang */

    PJONSlave() : PJON<Strategy>() {
      set_default();
    };

    /* PJONSlave initialization passing device id:
       PJONSlave bus(1); */

    PJONSlave(uint8_t device_id) : PJON<Strategy>(device_id) {
      set_default();
    };

    /* PJONSlave initialization passing bus and device id:
       uint8_t my_bus = {1, 1, 1, 1};
       PJONSlave bus(my_bys, 1); */

    PJONSlave(
      const uint8_t *b_id,
      uint8_t device_id
    ) : PJON<Strategy>(b_id, device_id) {
      set_default();
    };

    /* Acquire an id in multi-master configuration: */

    void acquire_id_multi_master(uint8_t limit = 0) {
      if(limit >= PJON_MAX_ACQUIRE_ID_COLLISIONS)
        return
          error(PJON_ID_ACQUISITION_FAIL, PJON_ID_ACQUIRE);

      PJON_DELAY_MICROSECONDS(PJON_RANDOM(PJON_ACQUIRE_ID_DELAY));
      char msg = PJON_ID_ACQUIRE;
      char head =
        this->config | required_config | PJON_PORT_BIT;
      this->_device_id = PJON_NOT_ASSIGNED;
      uint8_t id = PJON_RANDOM(1, PJON_MAX_DEVICES);

      if(
        id == PJON_NOT_ASSIGNED ||
        id == PJON_MASTER_ID ||
        id == PJON_BROADCAST
      ) acquire_id_multi_master(limit);

      if(
        this->send_packet_blocking(
          id,
          this->bus_id,
          &msg,
          1,
          head,
          0,
          PJON_DYNAMIC_ADDRESSING_PORT
        ) == PJON_ACK
      ) acquire_id_multi_master(limit++);

      this->_device_id = id;
      receive(PJON_RANDOM(PJON_ACQUIRE_ID_DELAY) * 1000);

      if(
        this->send_packet_blocking(
          this->_device_id,
          this->bus_id,
          &msg,
          1,
          head,
          0,
          PJON_DYNAMIC_ADDRESSING_PORT
        ) == PJON_ACK
      ) acquire_id_multi_master(limit++);
    };

    /* Acquire id in master-slave configuration: */

    bool acquire_id_master_slave() {
      generate_rid();
      char response[5];
      response[0] = PJON_ID_REQUEST;
      response[1] = (uint8_t)((uint32_t)(_rid) >> 24);
      response[2] = (uint8_t)((uint32_t)(_rid) >> 16);
      response[3] = (uint8_t)((uint32_t)(_rid) >>  8);
      response[4] = (uint8_t)((uint32_t)(_rid));

      if(this->send_packet_blocking(
        PJON_MASTER_ID,
        this->bus_id,
        response,
        5,
        this->config | PJON_PORT_BIT | required_config,
        0,
        PJON_DYNAMIC_ADDRESSING_PORT
      ) == PJON_ACK) return true;

      error(PJON_ID_ACQUISITION_FAIL, PJON_ID_REQUEST);
      return false;
    };

    /* Begin function to be called in setup: */

    void begin() {
      PJON<Strategy>::begin();
    };

    /* Release device id (Master-slave only): */

    bool discard_device_id() {
      char request[6] = {
        PJON_ID_NEGATE,
        (uint8_t)((uint32_t)(_rid) >> 24),
        (uint8_t)((uint32_t)(_rid) >> 16),
        (uint8_t)((uint32_t)(_rid) >>  8),
        (uint8_t)((uint32_t)(_rid)),
        this->_device_id
      };

      if(this->send_packet_blocking(
        PJON_MASTER_ID,
        this->bus_id,
        request,
        6,
        this->config | PJON_PORT_BIT | required_config,
        0,
        PJON_DYNAMIC_ADDRESSING_PORT
      ) == PJON_ACK) {
        this->_device_id = PJON_NOT_ASSIGNED;
        return true;
      }
      error(PJON_ID_ACQUISITION_FAIL, PJON_ID_NEGATE);
      return false;
    };

    /* Error callback: */

    void error(uint8_t code, uint16_t data) {
      _slave_error(code, data, _custom_pointer);
    };

    /* Filter incoming addressing packets callback: */

    void filter(
      uint8_t *payload,
      uint16_t length,
      const PJON_Packet_Info &packet_info
    ) {
      if(!handle_addressing()) {
         PJON_Packet_Info p_i;
         memcpy(&p_i, &packet_info, sizeof(PJON_Packet_Info));
         p_i.custom_pointer = _custom_pointer;
        _slave_receiver(payload, length, p_i);
      }
    };

    /* Generate a new device rid: */

    void generate_rid() {
      _rid = (
        (uint32_t)(PJON_RANDOM(2147483646)) ^
        (uint32_t)(PJON_ANALOG_READ(this->random_seed)) ^
        (uint32_t)(PJON_MICROS())
      ) ^ _rid ^ _last_request_time;
    };

    /* Get device rid: */

    uint32_t get_rid() {
      return _rid;
    };

    /* Handle dynamic addressing requests and responses: */

    bool handle_addressing() {
      if( // Detect mult-master dynamic addressing
        (this->last_packet_info.header & PJON_PORT_BIT) &&
        (this->last_packet_info.header & PJON_TX_INFO_BIT) &&
        (this->last_packet_info.header & PJON_CRC_BIT) &&
        (this->last_packet_info.port == PJON_DYNAMIC_ADDRESSING_PORT) &&
        (
          (this->last_packet_info.sender_id == PJON_NOT_ASSIGNED) ||
          (this->last_packet_info.sender_id == this->_device_id)
        )
      ) return true;

      if( // Handle master-slave dynamic addressing
        (this->last_packet_info.header & PJON_PORT_BIT) &&
        (this->last_packet_info.header & PJON_TX_INFO_BIT) &&
        (this->last_packet_info.header & PJON_CRC_BIT) &&
        (this->last_packet_info.port == PJON_DYNAMIC_ADDRESSING_PORT) &&
        (this->_device_id != PJON_MASTER_ID) &&
        (this->last_packet_info.sender_id == PJON_MASTER_ID)
      ) {

        uint8_t overhead =
          this->packet_overhead(this->last_packet_info.header);
        uint8_t CRC_overhead =
          (this->last_packet_info.header & PJON_CRC_BIT) ? 4 : 1;
        uint8_t rid[4] = {
          (uint8_t)((uint32_t)(_rid) >> 24),
          (uint8_t)((uint32_t)(_rid) >> 16),
          (uint8_t)((uint32_t)(_rid) >>  8),
          (uint8_t)((uint32_t)(_rid))
        };
        char response[6];
        response[1] = rid[0];
        response[2] = rid[1];
        response[3] = rid[2];
        response[4] = rid[3];

        if(this->data[overhead - CRC_overhead] == PJON_ID_REQUEST)
          if(
            PJONTools::bus_id_equality(
              this->data + ((overhead - CRC_overhead) + 1),
              rid
            )
          ) {
            response[0] = PJON_ID_CONFIRM;
            response[5] = this->data[(overhead - CRC_overhead) + 5];
            this->set_id(response[5]);
            if(this->send_packet_blocking(
              PJON_MASTER_ID,
              this->bus_id,
              response,
              6,
              this->config | PJON_PORT_BIT | required_config,
              0,
              PJON_DYNAMIC_ADDRESSING_PORT
            ) != PJON_ACK) {
              this->set_id(PJON_NOT_ASSIGNED);
              error(PJON_ID_ACQUISITION_FAIL, PJON_ID_CONFIRM);
            }
          }

        if(this->data[overhead - CRC_overhead] == PJON_ID_NEGATE)
          if(
            PJONTools::bus_id_equality(
              this->data + ((overhead - CRC_overhead) + 1),
              rid
            ) && this->_device_id == this->data[0]
          ) acquire_id_master_slave();

        if(this->data[overhead - CRC_overhead] == PJON_ID_LIST) {
          if(this->_device_id != PJON_NOT_ASSIGNED) {
            if(
              (uint32_t)(PJON_MICROS() - _last_request_time) >
              (PJON_ADDRESSING_TIMEOUT * 1.125)
            ) {
              _last_request_time = PJON_MICROS();
              response[0] = PJON_ID_REFRESH;
              response[5] = this->_device_id;
              this->send(
                PJON_MASTER_ID,
                this->bus_id,
                response,
                6,
                this->config | PJON_PORT_BIT | required_config,
                0,
                PJON_DYNAMIC_ADDRESSING_PORT
              );
            }
          } else if(
            (uint32_t)(PJON_MICROS() - _last_request_time) >
            (PJON_ADDRESSING_TIMEOUT * 1.125)
          ) {
            _last_request_time = PJON_MICROS();
            acquire_id_master_slave();
          }
        }
        return true;
      }
      return false;
    };

    /* Slave receive function: */

    uint16_t receive() {
      return PJON<Strategy>::receive();
    };

    /* Try to receive a packet repeatedly with a maximum duration: */

    uint16_t receive(uint32_t duration) {
      uint32_t time = PJON_MICROS();
      while((uint32_t)(PJON_MICROS() - time) <= duration)
        if(receive() == PJON_ACK) return PJON_ACK;
      return PJON_FAIL;
    };

    /* Set custom pointer: */

    void set_custom_pointer(void *p) {
      _custom_pointer = p;
    }

    /* Set default configuration: */

    void set_default() {
      PJON<Strategy>::set_default();
      PJON<Strategy>::set_custom_pointer(this);
      PJON<Strategy>::set_receiver(static_receiver_handler);
      PJON<Strategy>::set_error(static_error_handler);
      this->include_port(true);
    };

    /* Slave receiver function setter: */

    void set_receiver(PJON_Receiver r) {
      _slave_receiver = r;
    };

    /* Master error receiver function: */

    void set_error(PJON_Error e) {
      _slave_error = e;
    };

    /* Static receiver hander: */

    static void static_receiver_handler(
      uint8_t *payload,
      uint16_t length,
      const PJON_Packet_Info &packet_info
    ) {
      (
        (PJONSlave<Strategy>*)packet_info.custom_pointer
      )->filter(payload, length, packet_info);
    };

    /* Static error hander: */

    static void static_error_handler(
      uint8_t code,
      uint16_t data,
      void *custom_pointer
    ) {
      ((PJONSlave<Strategy>*)custom_pointer)->error(code, data);
    };

    /* Slave packet handling update: */

    uint8_t update() {
      return PJON<Strategy>::update();
    };

  private:
    void          *_custom_pointer;
    uint32_t      _last_request_time;
    uint32_t      _rid;
    PJON_Error    _slave_error;
    PJON_Receiver _slave_receiver;
};
