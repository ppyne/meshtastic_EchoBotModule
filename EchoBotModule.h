#pragma once

#include "mesh/MeshModule.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

class EchoBotModule : public MeshModule {
  public:
    EchoBotModule();

    bool wantPacket(const meshtastic_MeshPacket *p) override;
    ProcessMessage handleReceived(const meshtastic_MeshPacket &p) override;

  private:
    struct BotJob {
        NodeNum from;
        ChannelIndex channel;
        char text[256];
        uint8_t hop_start;
        bool chan_fortune;
    };

    static void botTask(void *arg);
    static QueueHandle_t botQueue;
};
