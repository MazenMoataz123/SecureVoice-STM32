# SecureVoice-STM32

SecureVoice is a real-time encrypted voice communication system built on STM32 microcontrollers. The project demonstrates secure wireless voice transmission over Bluetooth using audio compression, packet framing, encryption, and integrity verification.

## Features

- Real-time voice communication
- G.711 μ-law audio compression
- Bluetooth communication using HC-05
- Payload encryption with a 128-bit key
- CRC-16 packet integrity verification
- 20 ms deterministic frame scheduler
- Dual-role firmware (Transmitter / Receiver)
- Modular architecture for easy upgrades
- Future-ready design with Ascon-128 upgrade path

---

## System Architecture

The SecureVoice pipeline consists of five stages:

```text
Microphone
    ↓
Audio Capture
    ↓
G.711 μ-law Encoding
    ↓
Payload Encryption
    ↓
Packet Framing + CRC
    ↓
Bluetooth Transmission
    ↓
Bluetooth Reception
    ↓
CRC Verification
    ↓
Payload Decryption
    ↓
G.711 μ-law Decoding
    ↓
Speaker Playback
```

---

## Hardware Components

| Component | Description |
|------------|------------|
| STM32 Board | Main processing unit |
| MAX9814 | Electret microphone amplifier |
| HC-05 | Bluetooth Serial Module |
| Speaker | Audio output |
| 2N2222 | Speaker driver transistor |

---

## Technical Specifications

### Audio Parameters

| Parameter | Value |
|------------|------------|
| Sample Rate | 8000 Hz |
| Frame Duration | 20 ms |
| Frame Size | 160 Samples |
| ADC Resolution | 12-bit |
| PWM Resolution | 8-bit |

### Communication Parameters

| Parameter | Value |
|------------|------------|
| Packet Size | 166 Bytes |
| UART Baudrate | 115200 bps |
| CRC | CRC-16 (Modbus) |
| Key Length | 128-bit |
| Sync Byte | 0xAA |

---

## Packet Format

Each Bluetooth packet contains:

| Field | Size |
|---------|---------|
| Sync Byte | 1 Byte |
| Packet ID | 2 Bytes |
| Flags | 1 Byte |
| Payload | 160 Bytes |
| CRC16 | 2 Bytes |

```text
+---------+-----------+--------+-------------+--------+
| Sync    | Packet ID | Flags  | Payload     | CRC16  |
| 0xAA    | 2 Bytes   | 1 Byte | 160 Bytes   | 2 Bytes|
+---------+-----------+--------+-------------+--------+
```

Total packet size:

```text
166 Bytes
```

---

## Encryption

The system uses a lightweight XOR stream cipher for real-time operation on resource-constrained STM32 hardware.

### Encryption Process

```text
Ciphertext = Plaintext XOR Keystream
```

The keystream is generated using:

- 128-bit session key
- Packet ID (nonce)
- Byte position index

Advantages:

- Low computational overhead
- Unique keystream per packet
- Simple decryption (same XOR operation)

### Future Upgrade

The architecture is designed to support:

- Ascon-128 Authenticated Encryption
- Message authentication
- Tamper detection

Only the keystream generation function needs to be replaced.

---

## CRC Protection

To detect transmission errors, every packet includes a CRC-16 checksum using the Modbus polynomial:

```text
0xA001
```

The receiver:

1. Receives the packet
2. Recomputes CRC
3. Compares values
4. Drops corrupted packets

---

## System States

```text
BOOT
  ↓
IDLE
  ↓
ACTIVE_COMM
  ↓
ERROR_HALT
```

### BOOT

- Initialize peripherals
- Calibrate microphone ADC
- Perform Bluetooth handshake

### IDLE

- Wait for connection

### ACTIVE_COMM

- Capture audio
- Encode
- Encrypt
- Transmit / Receive
- Decrypt
- Playback

### ERROR_HALT

- Stop communication
- Output diagnostic message

---

## Project Structure

```text
SecureVoice-STM32/
│
├── app.c
├── app.h
│
├── audio.c
├── audio.h
│
├── crypto.c
├── crypto.h
│
├── transport.c
├── transport.h
│
├── bluetooth.cpp
├── bluetooth.h
│
├── hal_audio.cpp
├── hal_audio.h
│
├── sys_health.c
├── sys_health.h
│
├── debug.cpp
├── debug.h
│
├── project_defs.h
└── system_config.h
```

### Module Responsibilities

#### Audio Module

- ADC microphone capture
- G.711 μ-law encoding
- G.711 μ-law decoding
- PWM speaker output

#### Crypto Module

- XOR stream cipher
- Key management
- Encryption validation

#### Transport Module

- Packet framing
- Sync byte detection
- CRC generation and verification

#### Bluetooth Module

- HC-05 UART communication

#### System Health Module

- Task timing monitor
- Error detection
- Runtime diagnostics

---

## Building

### Transmitter

Set:

```c
#define DEVICE_ROLE ROLE_TRANSMITTER
```

Compile and flash to STM32.

### Receiver

Set:

```c
#define DEVICE_ROLE ROLE_RECEIVER
```

Compile and flash to STM32.

---

## Results

The project successfully achieved:

- End-to-end encrypted voice communication
- Real-time operation within a 20 ms frame budget
- Modular software architecture
- Shared codebase for TX and RX devices
- CRC-based packet validation
- Hardware abstraction for portability
- Future-ready authenticated encryption support

---

## Team

| Name | Student ID |
|--------|------------|
| Abdelrhman Yasser | 221004349 |
| Omar Ahmed Abdulaziz | 221005041 |
| Mazen Moataz | 221005723 |
| Youssef Hani | 221005893 |

---

## Future Improvements

- Ascon-128 authenticated encryption
- DMA-based audio streaming
- Full-duplex communication
- Dynamic key exchange
- Higher audio quality codecs
- Battery optimization
- Mobile application integration

---

## License

This project is released under the MIT License.

---

## Repository

GitHub Repository:

https://github.com/MazenMoataz123/SecureVoice-STM32