
# EchoBotModule for Meshtastic

## Overview

**EchoBotModule** is a lightweight, optional Meshtastic firmware module that adds a simple interactive bot to a LoRa mesh network.

It listens for **direct messages** sent to the node and responds with useful or playful commands (uptime, signal quality, routing info, fortune messages, etc.).  
Optionally, it can also **listen passively on a specific channel** and broadcast a response when a trigger word is detected.

This project is inspired by the early Internet / IRC spirit: small, friendly, autonomous services living inside the network.

---

## What is Meshtastic?

[Meshtastic](https://meshtastic.org/) is an open-source project that enables **off-grid, decentralized communication** using inexpensive LoRa radios.  
Nodes form a mesh network and exchange encrypted messages over long distances with very low power consumption.

---

## What is LoRa?

**LoRa** (Long Range) is a low-power radio modulation technology designed for long-distance communication at low data rates.  
It is well suited for sensor networks, off-grid messaging, and mesh communication.

---

## Features

- Direct-message command handling (via RTOS queue)
- Optional passive listening on a configurable channel
- Safe packet filtering (only relevant packets are processed)
- No blocking inside `handleReceived()`
- Example commands:
  - `HELP`
  - `UPTIME`
  - `DATE`
  - `PING`
  - `SIGNAL`
  - `ROUTEINFO`
  - `FORTUNE`

---

## Channel Configuration

At the top of the module, you can configure the passive listening channel:

```c
#define CHAN 2
```

Channels are zero-based (`0 = channel 1` in the UI).

---

## Files

This repository contains only:

- `EchoBotModule.h`
- `EchoBotModule.cpp`

They are meant to be dropped directly into the Meshtastic firmware tree.

---

## Integration Instructions

### 1. Copy files

Copy `EchoBotModule.h` and `EchoBotModule.cpp` into:

```
firmware/src/modules/
```

---

### 2. Include the module

Edit:

```
firmware/src/modules/modules.cpp
```

Add the include near the top:

```cpp
#include "modules/EchoBotModule.h"
```

---

### 3. Instantiate the module

Edit again:

```
firmware/src/modules/modules.cpp
```

In the module initialization (near the end of the function `void setupModules()`) section (where other modules are created), add:

```cpp
void setupModules() {
...
new EchoBotModule();
...
}
```

That’s it. The module will register itself automatically.

---

## Design Notes

- All message processing happens in a **FreeRTOS task**
- `handleReceived()` is kept minimal and non-blocking
- Broadcast replies never request ACKs
- Payload sizes are always bounded and null-terminated
- The module avoids interfering with routing or core services

---

## Compatibility

- Tested with Meshtastic firmware **v2.7.16** and hardware Heltec V3 or Heltec WSL V3.
- Newer development versions may contain regressions affecting MQTT + broadcast packets

---

## License

BSD 3-Clause License

Copyright (c) 2025 Alexandre Vialle
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the conditions of the BSD 3-Clause License are met.

See `LICENSE` for details.

---

## Final Note

This module is intentionally small, transparent, and optional.  
If you don’t like it — don’t load it.  
If you do — enjoy, modify, and extend it.

