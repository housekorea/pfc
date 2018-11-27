
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

PJONMaster has been created by Giovanni Blu Mitolo with the support
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

/* Reference to device */
struct Device_reference {
  uint8_t  packet_index = 0;
  uint32_t registration = 0;
  uint32_t rid          = 0;
  bool     state        = 0;
};

template<typename Strategy = SoftwareBitBang>
class PJONMaster : public PJON<Strategy> {
  public:
    bool debug = false;
    Device_reference ids[PJON_MAX_DEVICES];
    uint8_t required_config =
      PJON_TX_INFO_BIT | PJON_CRC_BIT | PJON_ACK_REQ_BIT;

    /* PJONMaster bus default initialization:
       State: Local (bus_id: 0.0.0.0)
       Acknowledge: true (Acknowledge is requested)
       device id: MASTER (254)
       Mode: PJON_HALF_DUPLEX
       Sender info: true (Sender info are included in the packet)
       Strategy: SoftwareBitBang */

    PJONMaster() : PJON<Strategy>(PJON_MASTER_ID) {
      set_default();
    };

    /* PJONMaster initialization passing bus and device id:
       uint8_t my_bus = {1, 1, 1, 1};
       PJONMaster master(my_bys); */

    PJONMaster(const uint8_t *b_id) : PJON<Strategy>(b_id, PJON_MASTER_ID) {
      set_default();
    };

    /* Add a device reference: */

    bool add_id(uint8_t id, uint32_t rid, bool state) {
      if(unique_rid(rid) && !ids[id - 1].state && !ids[id - 1].rid) {
        ids[id - 1].rid = rid;
        ids[id - 1].state = state;
        return true;
      }
      return false;
    };

    /* Confirm a device id sending a repeated broadcast containing:
    PJON_ID_REQUEST - RID (4 byte random id) - DEVICE ID (the new assigned) */

    void approve_id(uint8_t *b_id, uint32_t rid) {
      char response[6];
      uint16_t state = reserve_id(rid);
      if(state == PJON_DEVICES_BUFFER_FULL) return;
      if(state == PJON_FAIL) return negate_id(PJON_NOT_ASSIGNED, b_id, rid);

      response[0] = PJON_ID_REQUEST;
      response[1] = (uint8_t)((uint32_t)(rid) >> 24);
      response[2] = (uint8_t)((uint32_t)(rid) >> 16);
      response[3] = (uint8_t)((uint32_t)(rid) >>  8);
      response[4] = (uint8_t)((uint32_t)(rid));
      response[5] = (uint8_t)(state);

      ids[response[5] - 1].packet_index = PJON<Strategy>::send_repeatedly(
        PJON_BROADCAST,
        b_id,
        response,
        6,
        PJON_ID_REQUEST_INTERVAL,
        PJON<Strategy>::config | required_config | PJON_PORT_BIT,
        0,
        PJON_DYNAMIC_ADDRESSING_PORT
      );
    };

    /* Master begin function: */

    void begin() {
      PJON<Strategy>::begin();
      list_ids();
    };

    /* Confirm device ID insertion in list: */

    bool confirm_id(uint32_t rid, uint8_t id) {
      if(ids[id - 1].rid == rid && !ids[id - 1].state) {
        if(
          (uint32_t)(PJON_MICROS() - ids[id - 1].registration) <
          PJON_ADDRESSING_TIMEOUT
        ) {
          ids[id - 1].state = true;
          PJON<Strategy>::remove(ids[id - 1].packet_index);
          return true;
        }
      }
      return false;
    };

    /* Count active devices: */

    uint8_t count_active_ids() {
      uint8_t result = 0;
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(ids[i].state) result++;
      return result;
    };

    /* Empty a single element or the whole buffer: */

    void delete_id_reference(uint8_t id = 0) {
      if(!id) {
        for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
          if(!ids[i].state && ids[i].rid)
            this->remove(ids[i].packet_index);
          ids[i].packet_index = 0;
          ids[i].registration = 0;
          ids[i].rid = 0;
          ids[i].state = false;
        }
      } else if(id > 0 && id < PJON_MAX_DEVICES) {
        if(!ids[id - 1].state && ids[id - 1].rid)
          this->remove(ids[id - 1].packet_index);
        ids[id - 1].packet_index = 0;
        ids[id - 1].registration = 0;
        ids[id - 1].rid   = 0;
        ids[id - 1].state = false;
      }
    };

    /* Check slaves presence contacting all known devices and waiting for
       a synchronous acknowledge. If no answer is received the slave is
       removed from the list (bandwidth consuming, use rarely): */

    void check_slaves_presence() {
      char request = PJON_ID_ACQUIRE;
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
        if(ids[i].state && ids[i].rid)
          if(
            PJON<Strategy>::send_packet_blocking(
              i + 1,
              this->bus_id,
              &request,
              1,
              PJON<Strategy>::config | required_config | PJON_PORT_BIT,
              0,
              PJON_DYNAMIC_ADDRESSING_PORT
            ) != PJON_ACK
          ) {
            delete_id_reference(i + 1);
            error(PJON_ID_ACQUISITION_FAIL, i + 1);
          }
      }
    };

    /* Master error handler: */

    void error(uint8_t code, uint16_t data) {
      _master_error(code, data, _custom_pointer);
      if(code == PJON_CONNECTION_LOST)
        delete_id_reference(PJON<Strategy>::packets[data].content[0]);
    };

    static void static_error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
      ((PJONMaster<Strategy>*)custom_pointer)->error(code, data);
    };

    /* Filter addressing packets from receive callback: */

    void filter(
      uint8_t *payload,
      uint16_t length,
      const PJON_Packet_Info &packet_info
    ) {
      PJON_Packet_Info p_i;
      memcpy(&p_i, &packet_info, sizeof(PJON_Packet_Info));
      p_i.custom_pointer = _custom_pointer;
      if(!handle_addressing() || debug)
        _master_receiver(payload, length, p_i);
    };

    /* Handle addressing procedure if related: */

    bool handle_addressing() {
      bool filter = false;
      uint8_t overhead = PJON<Strategy>::packet_overhead(this->data[1]);
      uint8_t CRC_overhead = (this->data[1] & PJON_CRC_BIT) ? 4 : 1;

      if(
        (this->last_packet_info.header & PJON_PORT_BIT) &&
        (this->last_packet_info.header & PJON_TX_INFO_BIT) &&
        (this->last_packet_info.header & PJON_CRC_BIT) &&
        (this->last_packet_info.port == PJON_DYNAMIC_ADDRESSING_PORT)
      ) {
        filter = true;
        uint8_t request = this->data[overhead - CRC_overhead];
        uint32_t rid =
          (uint32_t)(this->data[(overhead - CRC_overhead) + 1]) << 24 |
          (uint32_t)(this->data[(overhead - CRC_overhead) + 2]) << 16 |
          (uint32_t)(this->data[(overhead - CRC_overhead) + 3]) <<  8 |
          (uint32_t)(this->data[(overhead - CRC_overhead) + 4]);

        if(request == PJON_ID_REQUEST)
          approve_id(this->last_packet_info.sender_bus_id, rid);

        if(request == PJON_ID_CONFIRM)
          if(!confirm_id(rid, this->data[(overhead - CRC_overhead) + 5]))
            negate_id(
              this->last_packet_info.sender_id,
              this->last_packet_info.sender_bus_id,
              rid
            );

        if(request == PJON_ID_REFRESH)
          if(!add_id(this->data[(overhead - CRC_overhead) + 5], rid, 1))
            negate_id(
              this->last_packet_info.sender_id,
              this->last_packet_info.sender_bus_id,
              rid
            );

        if(request == PJON_ID_NEGATE)
          if(
            this->data[(overhead - CRC_overhead) + 5] ==
            this->last_packet_info.sender_id
          )
            if(rid == ids[this->last_packet_info.sender_id - 1].rid)
              if(
                PJONTools::bus_id_equality(
                  this->last_packet_info.sender_bus_id,
                  this->bus_id
                )
              ) delete_id_reference(this->last_packet_info.sender_id);
      }
      return filter;
    };

    /* Remove reserved id which expired (Remove never confirmed ids): */

    void free_reserved_ids_expired() {
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(!ids[i].state && ids[i].rid) {
          if(
            (uint32_t)(PJON_MICROS() - ids[i].registration) <
            PJON_ADDRESSING_TIMEOUT
          ) continue;
          else delete_id_reference(i + 1);
        }
    };

    /* Get device id from RID: */

    uint8_t get_id_from_rid(uint32_t rid) {
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(rid == ids[i].rid) return i + 1;
      return PJON_NOT_ASSIGNED;
    };

    /* Check for device rid uniqueness in the reference buffer: */

    bool unique_rid(uint32_t rid) {
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(ids[i].rid == rid) return false;
      return true;
    };

    /* Broadcast a PJON_ID_LIST request to all devices: */

    void list_ids() {
      uint32_t time = PJON_MICROS();
      char request = PJON_ID_LIST;
      while((uint32_t)(PJON_MICROS() - time) < PJON_ADDRESSING_TIMEOUT) {
        PJON<Strategy>::send_packet(
          PJON_BROADCAST,
          this->bus_id,
          &request,
          1,
          PJON<Strategy>::config | required_config | PJON_PORT_BIT,
          0,
          PJON_DYNAMIC_ADDRESSING_PORT
        );
        receive(PJON_LIST_IDS_TIME);
      }
    };

    /* Negate a device id request sending a packet to the device containing
       ID_NEGATE forcing the slave to make a new request. */

    void negate_id(uint8_t id, uint8_t *b_id, uint32_t rid) {
      uint8_t response[5] = {
        (uint8_t)PJON_ID_NEGATE,
        (uint8_t)((uint32_t)(rid) >> 24),
        (uint8_t)((uint32_t)(rid) >> 16),
        (uint8_t)((uint32_t)(rid) >>  8),
        (uint8_t)((uint32_t)(rid))
      };

      PJON<Strategy>::send_packet_blocking(
        id,
        b_id,
        (char *)response,
        5,
        PJON<Strategy>::config | required_config | PJON_PORT_BIT,
        0,
        PJON_DYNAMIC_ADDRESSING_PORT
      );
    };

    /* Reserve a device id and wait for its confirmation: */

    uint16_t reserve_id(uint32_t rid) {
      if(!unique_rid(rid)) return PJON_FAIL;
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(!ids[i].state && !ids[i].rid) {
          ids[i].registration = PJON_MICROS();
          ids[i].rid = rid;
          ids[i].state = false;
          return i + 1;
        }
      error(PJON_DEVICES_BUFFER_FULL, PJON_MAX_DEVICES);
      return PJON_DEVICES_BUFFER_FULL;
    };

/* Master receive function: */

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

    /* Static receiver hander: */

    static void static_receiver_handler(
      uint8_t *payload,
      uint16_t length,
      const PJON_Packet_Info &packet_info
    ) {
      (
        (PJONMaster<Strategy>*)packet_info.custom_pointer
      )->filter(payload, length, packet_info);
    };

    /* Set custom pointer: */

    void set_custom_pointer(void *p) {
      _custom_pointer = p;
    };

    /* Set default configuration: */

    void set_default() {
      PJON<Strategy>::set_default();
      PJON<Strategy>::set_custom_pointer(this);
      PJON<Strategy>::set_error(static_error_handler);
      PJON<Strategy>::set_receiver(static_receiver_handler);
      delete_id_reference();
      this->include_port(true);
    };

    /* Master receiver function setter: */

    void set_receiver(PJON_Receiver r) {
      _master_receiver = r;
    };

    /* Master error receiver function: */

    void set_error(PJON_Error e) {
      _master_error = e;
    };

    /* Master packet handling update: */

    uint8_t update() {
      free_reserved_ids_expired();
      return PJON<Strategy>::update();
    };

  private:
    void           *_custom_pointer;
    PJON_Receiver   _master_receiver;
    PJON_Error      _master_error;
};
