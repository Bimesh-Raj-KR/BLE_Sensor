# BLE_SensorLinphone Custom RTP Transfer

A C-based implementation using the Linphone SDK and oRTP to facilitate automated audio transmission. This project bypasses standard Linphone media handling to manually inject and extract RTP packets for custom audio processing.

## Features

### Caller

Initiates a SIP call, monitors for a connection, and streams a PCMU-encoded audio file directly into the RTP session using manual timestamping and sequence numbering.

### Receiver

Listens for incoming SIP calls, "hijacks" the established RTP session to capture raw payload data into a WAV file, and implements an Echo Phase to stream the received data back to the caller.

Utilizes **PCMU (G.711u)** at **8000Hz**, ensuring low-latency audio transmission with manual RTP header construction.

Also two clean wav files are provided for testing purposes

## Prerequisites

1. **Linphone SDK:** Ensure the Linphone SDK is installed

2. **Audio Format:** Input files must be raw PCMU or WAV files formatted for 8000Hz mono.

3. **Network:** Both applications are configured to communicate over UDP port **5070**. Ensure this port is not blocked by a firewall.

4. **Execution Order:** Start the Receiver before the Caller to ensure the SIP INVITE is captured correctly.

5. **Makefile:** Set the correct path to Linphone SDK

## Building with Make

The project uses a structured Makefile that handles the complex Linphone library dependencies and includes the custom helper functions.

### make call

Compiles the Caller application (integrating main.c and callerFunctions.c).
Output: call

### make receive
Compiles the Receiver application (the refactored code following Trenser standards).
Output: receiver