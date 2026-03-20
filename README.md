# ✈️ RC Airplane Control System

### Real-Time Embedded Control • Arduino Nano • NRF24L01

<p align="center">
  <b>Wireless Control System for RC Aircraft with Safety-Critical Features</b><br>
  <i>From pilot input to actuator control — fully embedded pipeline</i>
</p>

---

## 🏆 Key Highlights

* 📡 **Reliable RF Communication** (NRF24L01 – SPI)
* ⚡ **Real-Time Control Loop (~50 Hz)**
* 🔐 **Safety-Critical Design**

  * Motor Kill Switch
  * Fail-Safe (Signal Loss Detection)
* 🎯 **Precise Servo Control (Ailerons + Elevator)**
* 🧠 Structured embedded architecture (TX/RX separation)

---

## 🧠 System Architecture

```id="k96qdl"
        🎮 Pilot Input
   (Joysticks + Buttons)
              │
              ▼
     🔵 Transmitter (TX)
        Arduino Nano
              │
              ▼
     📡 NRF24L01 (SPI)
     Wireless Link (2.4 GHz)
              │
              ▼
     🔴 Receiver (RX)
        Arduino Nano
              │
              ▼
   ✈️ Actuators & Propulsion
   Servos + ESC + Brushless Motor
```

---

## ⚙️ Hardware Configuration

### Core Components

* Arduino Nano ×2
* NRF24L01 ×2
* Analog Joysticks ×2
* Push Buttons ×2
* Servo Motors ×3
* ESC + Brushless Motor

---

### 🔌 NRF24L01 Wiring

| NRF24L01 | Arduino Nano |
| -------- | ------------ |
| VCC      | 3.3V ⚠️      |
| GND      | GND          |
| CE       | D9           |
| CSN      | D10          |
| SCK      | D13          |
| MOSI     | D11          |
| MISO     | D12          |

⚠️ **Critical:**

* NEVER use 5V
* Add **10–100 µF capacitor** for stability

---

## 📦 Data Communication Model

```cpp id="qlt34x"
struct DataPacket {
  int joyGaucheY;   // Throttle
  int joyDroitX;    // Ailerons
  int joyDroitY;    // Elevator
  bool secuMoteur;  // Motor safety
  bool reserve;     // Future feature
};
```

### ✔ Design Choices

* Single struct → **deterministic communication**
* Fixed size → **low latency**
* Shared definition TX/RX → **no mismatch risk**

---

## 🎮 Control Mapping

| Input            | Output              |
| ---------------- | ------------------- |
| Left joystick Y  | ESC (Throttle)      |
| Right joystick X | Ailerons (mirrored) |
| Right joystick Y | Elevator            |
| Button 1         | Motor Kill Switch   |
| Button 2         | Reserved            |

---

## ⚡ Control Pipeline

```id="2q2r6o"
Analog Read → Data Struct → RF Transmission → Reception → Mapping → Actuation
```

* Sampling: `analogRead()`
* Transmission: `radio.write()`
* Reception: `radio.read()`
* Actuation: `Servo.write()` / `writeMicroseconds()`

---

## 🔴 Receiver Logic

### 🎯 Signal Mapping

| Input Range | Output             |
| ----------- | ------------------ |
| 300–700     | ESC → 1000–2000 µs |
| 300–700     | Servo → 0–180°     |

### ✈️ Aileron Behavior

```id="07rtud"
Left  = X  
Right = 180 - X
```

➡️ Ensures **aerodynamic symmetry**

---

## 🔐 Safety System (Critical Section)

### 🛑 Motor Kill Switch

* Immediate override
* Forces throttle → **1000 µs**
* Priority over pilot input

---

### ⚠️ Fail-Safe System

```id="9t4o3u"
if (no signal > 500 ms) {
    servos → 90°
    ESC → 1000 µs
}
```

✔ Prevents uncontrolled behavior
✔ Auto-recovery when signal returns

---

## 🔥 ESC Arming Sequence

```id="5d9pxb"
ESC = 1000 µs → wait 3 seconds
```

✔ Required for safe initialization

⚠️ **Always remove propeller during testing**

---

## 🧪 Validation Strategy

### 🔬 Unit Testing

* Joystick calibration (Serial output)
* Servo sweep tests
* ESC initialization

### 🔗 Integration Testing

* RF communication validation
* Full control loop verification
* Latency measurement

### 🔐 Safety Testing

* Kill switch validation
* Fail-safe trigger (TX shutdown)

---

## 📈 Performance Metrics

| Metric      | Value                 |
| ----------- | --------------------- |
| Frequency   | ~50 Hz                |
| Latency     | Low                   |
| Reliability | High (short-range RF) |

---

## ⚠️ Engineering Challenges

### 🔋 Power Stability

NRF24L01 is sensitive:

* Requires **stable 3.3V**
* Needs decoupling capacitor

### 🎯 Signal Noise

* Analog drift handled via mapping
* Future filtering possible

---

## 🚀 Future Enhancements

* 🧠 Flight stabilization (PID control)
* 📡 Telemetry (RX → TX feedback)
* 🔁 ACK-based communication
* 🎮 Input smoothing (filters)
* ✈️ Advanced flight modes

---

## 💼 Engineering Skills Demonstrated

* Embedded C++ (Arduino)
* Real-time systems design
* RF communication (SPI / NRF24)
* Safety-critical logic
* Hardware/software co-design

---

## 👨‍💻 Author

Lucas CALLOVA & Christian HOYEK

---

## 🛩️ Safety Disclaimer

This project controls a **real propulsion system**.

Always:

* ❌ Remove propeller during testing
* ✔ Validate fail-safe
* ✔ Test in controlled environment

---

## ⭐ Why This Project Matters

This project demonstrates a **complete embedded control chain**:

> **Input → Processing → Wireless Communication → Actuation → Safety**

Making it highly relevant for:

* Embedded Systems Engineering
* Robotics & UAV Systems
* Safety-Critical Applications

---

<p align="center">
  🚀 <b>Ready for real-world embedded challenges</b>
</p>
