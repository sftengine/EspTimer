# ESP8266 Web Timer - Architecture

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     ESP8266 Web Timer                        │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                    main.c (App Core)                    │ │
│  │  • System Initialization                                │ │
│  │  • Timer Check Task (runs every 60 seconds)            │ │
│  │  • System Status Monitoring                             │ │
│  └──────┬──────────┬──────────┬──────────┬─────────┬──────┘ │
│         │          │          │          │         │        │
│  ┌──────▼──────┐┌──▼──────┐┌──▼─────┐┌──▼──────┐┌─▼──────┐│
│  │   WiFi      ││   NTP    ││ Timer  ││ Relay   ││ HTTPS  ││
│  │  Manager    ││   Sync   ││Storage ││ Control ││ Server ││
│  └──────┬──────┘└──┬───────┘└──┬─────┘└──┬──────┘└─┬──────┘│
│         │          │            │         │         │        │
│  ┌──────▼──────────▼────────────▼─────────▼─────────▼─────┐ │
│  │              ESP-IDF Framework (RTOS)                   │ │
│  │  • FreeRTOS • LWIP • NVS • GPIO • HTTP Server          │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                              │
└──────────────────┬───────────────────────────────────────────┘
                   │
        ┌──────────┴───────────┐
        │                      │
   ┌────▼─────┐         ┌─────▼─────┐
   │  WiFi    │         │  Relay    │
   │ Network  │         │  Module   │
   └──────────┘         └───────────┘
        │                     │
   ┌────▼─────┐         ┌─────▼─────┐
   │ Internet │         │  Load     │
   │   NTP    │         │ (Lights,  │
   │          │         │  Pump,    │
   └──────────┘         │  etc.)    │
                        └───────────┘
```

## Component Details

### 1. WiFi Manager (`wifi_manager.c/h`)
**Purpose**: Handle WiFi connectivity

**Responsibilities**:
- Initialize WiFi in station mode
- Connect to configured access point
- Handle disconnection events
- Auto-reconnection logic
- Provide connection status

**Key Functions**:
- `wifi_init_sta()` - Initialize and connect
- `wifi_is_connected()` - Check connection status
- `wifi_get_ip_address()` - Get assigned IP

**Events Handled**:
- `WIFI_EVENT_STA_START`
- `WIFI_EVENT_STA_DISCONNECTED`
- `IP_EVENT_STA_GOT_IP`

### 2. NTP Sync (`ntp_sync.c/h`)
**Purpose**: Synchronize system time with internet time servers

**Responsibilities**:
- Initialize SNTP client
- Configure multiple NTP servers
- Synchronize time on startup
- Handle timezone configuration
- Provide time formatting functions

**Key Functions**:
- `ntp_sync_time()` - Sync with NTP servers
- `get_current_time_str()` - Format time as string
- `get_current_time()` - Get time structure
- `is_time_synced()` - Check sync status

**NTP Servers**:
1. Primary: pool.ntp.org
2. Secondary: time.google.com
3. Tertiary: time.cloudflare.com

### 3. Timer Storage (`timer_storage.c/h`)
**Purpose**: Persist timer settings in non-volatile memory

**Responsibilities**:
- Initialize NVS partition
- Save timer settings (ON/OFF times)
- Load timer settings on boot
- Provide default values if none saved

**Key Functions**:
- `timer_storage_init()` - Initialize NVS
- `timer_save_settings()` - Save to flash
- `timer_load_settings()` - Load from flash

**Storage Format**:
```
Namespace: "timer"
Keys:
  - on_hour  (uint8_t)
  - on_min   (uint8_t)
  - off_hour (uint8_t)
  - off_min  (uint8_t)
```

### 4. Relay Control (`relay_control.c/h`)
**Purpose**: Control relay via GPIO

**Responsibilities**:
- Initialize relay GPIO pin
- Set relay ON/OFF state
- Get current relay state
- Handle active HIGH/LOW logic

**Key Functions**:
- `relay_init()` - Configure GPIO
- `relay_set_state()` - Control relay
- `relay_get_state()` - Read state
- `relay_toggle()` - Toggle state

**GPIO Configuration**:
- Default: GPIO 5 (NodeMCU D1)
- Mode: Output
- Active: HIGH (configurable)

### 5. HTTPS Server (`https_server.c/h`)
**Purpose**: Serve web interface and handle API requests

**Responsibilities**:
- Start HTTPS server with SSL/TLS
- Serve embedded HTML page
- Handle API endpoints
- Process timer configuration updates
- Return system status as JSON

**Key Functions**:
- `start_https_server()` - Start server
- `stop_https_server()` - Stop server

**Endpoints**:
1. `GET /` - Main HTML interface
2. `GET /api/status` - System status JSON
3. `POST /api/timer` - Save timer settings

**SSL/TLS**:
- Self-signed certificate (PEM format)
- 2048-bit RSA key
- Certificates embedded in binary

### 6. Main Application (`main.c`)
**Purpose**: Application entry point and coordination

**Responsibilities**:
- Initialize all components
- Create timer check task
- Monitor system health
- Coordinate component interactions

**Key Functions**:
- `app_main()` - Entry point
- `timer_check_task()` - Periodic timer check
- `is_time_in_range()` - Timer logic

**Task Structure**:
```
Main Task (priority: default)
  ├── Initialize NVS
  ├── Connect WiFi
  ├── Sync NTP
  ├── Load timer settings
  ├── Start HTTPS server
  └── Monitor system (loop)

Timer Check Task (priority: 5)
  └── Check timer every 60 seconds
      ├── Reload settings
      ├── Compare current time
      └── Update relay state
```

## Data Flow

### Startup Sequence
```
1. Power On
   ↓
2. NVS Init → Load saved timer settings
   ↓
3. WiFi Init → Connect to AP
   ↓
4. NTP Sync → Get current time
   ↓
5. Relay Init → Set initial state
   ↓
6. HTTPS Server Start → Listen for requests
   ↓
7. Timer Task Start → Begin monitoring
   ↓
8. Ready (Main loop: system monitoring)
```

### Timer Check Flow
```
Every 60 seconds:
1. Get current time (HH:MM)
   ↓
2. Load timer settings from memory
   ↓
3. Check if current time is in ON range
   ↓
4. Compare with current relay state
   ↓
5. If different: Update relay
   ↓
6. Log action
```

### Web Interface Flow
```
User Browser → HTTPS Request
   ↓
ESP8266 HTTPS Server
   ├── GET / → Return HTML page
   ├── GET /api/status → Return JSON
   │   ├── Current time
   │   ├── Relay state
   │   └── Timer settings
   └── POST /api/timer → Save settings
       ├── Parse JSON
       ├── Validate values
       ├── Save to NVS
       └── Return success/error
```

## Memory Layout

### Flash Memory (~2MB)
```
┌─────────────────────────────┐
│ Bootloader      (32 KB)     │
├─────────────────────────────┤
│ Partition Table (4 KB)      │
├─────────────────────────────┤
│ NVS            (16 KB)      │ ← Timer settings stored here
├─────────────────────────────┤
│ PHY Init       (4 KB)       │
├─────────────────────────────┤
│ Application    (~1.5 MB)    │ ← Code + SSL certs + HTML
├─────────────────────────────┤
│ (Unused)                    │
└─────────────────────────────┘
```

### RAM Usage (~80 KB available)
```
Heap Memory Usage:
- WiFi/LWIP Stack:   ~25 KB
- HTTP Server:       ~15 KB
- SSL/TLS:          ~10 KB
- Application:       ~5 KB
- FreeRTOS:         ~10 KB
- Available:        ~15 KB (buffer)
```

## Configuration System

### Build-Time Configuration (Kconfig)
```
menuconfig
├── WiFi Configuration
│   ├── SSID
│   ├── Password
│   └── Max Retry
├── NTP Configuration
│   ├── Server 1, 2, 3
│   └── Timezone Offset
└── Timer Configuration
    ├── Relay GPIO Pin
    ├── HTTPS Port
    └── Relay Active Level
```

### Runtime Configuration (Web Interface)
- ON Time (hour, minute)
- OFF Time (hour, minute)
- Manual override (future)

### Persistent Storage (NVS)
- Timer ON/OFF times
- Survives power cycles
- Flash lifetime: 100,000+ writes

## Security Architecture

### HTTPS/TLS
```
Browser ←→ HTTPS (TLS 1.2) ←→ ESP8266
         Self-signed cert
         2048-bit RSA
         Port 443
```

### Threat Model
- **Protected**: WiFi password, timer settings
- **Not Protected**: Web interface (no authentication)
- **Recommendation**: Use on trusted network only

## Performance Characteristics

### Timing
- WiFi connect: 5-10 seconds
- NTP sync: 1-3 seconds
- Web page load: <500ms
- API response: <100ms
- Timer check: Every 60 seconds
- Timer accuracy: ±1 minute

### Resource Usage
- CPU: <5% average (ESP8266 @ 80MHz)
- Flash: ~300 KB (code + data)
- RAM: ~40 KB used, ~40 KB free
- Power: ~80mA @ 3.3V (WiFi active)

## Extension Points

### Easy to Add
1. **Multiple relays**: Duplicate relay control
2. **More timers**: Array of timer settings
3. **Weekly schedule**: Add day-of-week logic
4. **Manual override**: Add button/web control

### Moderate Effort
1. **User authentication**: Add login system
2. **MQTT**: Add MQTT client
3. **Sensors**: Add I2C/SPI sensor support
4. **OTA updates**: Add HTTP OTA handler

### Significant Effort
1. **Mobile app**: Native iOS/Android app
2. **Cloud integration**: AWS/Azure IoT
3. **Voice control**: Alexa/Google Home
4. **Machine learning**: Predictive scheduling

## Reliability Features

### Implemented
- WiFi auto-reconnection
- Multiple NTP servers
- Persistent storage (NVS)
- Watchdog timer (ESP-IDF default)
- Error logging

### Recommended Additions
- External RTC for time backup
- Power loss detection
- Status LED indicators
- Hardware reset button
- Backup configuration

## Development Tools

### Required
- ESP-IDF (ESP8266 RTOS SDK)
- Python 3.6+
- Git

### Optional
- VS Code with ESP-IDF extension
- Serial monitor (PuTTY, screen, minicom)
- Logic analyzer for debugging
- Network analyzer (Wireshark)

## Testing Strategy

### Unit Testing
- Component initialization
- Timer logic (midnight crossing)
- NVS read/write
- GPIO control

### Integration Testing
- WiFi connection flow
- NTP synchronization
- Web interface interaction
- Relay control via timer

### System Testing
- 24-hour operation
- Power cycle recovery
- Network disconnection
- Timer accuracy
- Memory leak detection

## Troubleshooting Decision Tree

```
Problem: Device not working
  ├── No WiFi?
  │   ├── Check SSID/password
  │   ├── Check 2.4GHz network
  │   └── Check serial output
  ├── No time sync?
  │   ├── Check internet access
  │   ├── Check NTP servers
  │   └── Check firewall (UDP 123)
  ├── Relay not switching?
  │   ├── Check GPIO config
  │   ├── Check wiring
  │   ├── Check relay power
  │   └── Check active level
  └── Can't access web?
      ├── Check IP address
      ├── Check same network
      ├── Accept SSL warning
      └── Check port config
```

## Future Architecture Improvements

1. **State machine** for system states
2. **Event-driven** architecture
3. **Configuration manager** module
4. **Hardware abstraction layer**
5. **Modular component system**
6. **Plugin architecture**
