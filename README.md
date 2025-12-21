# ESP8266 Web Timer

A web-based timer application for ESP8266 using ESP-IDF that controls a relay based on scheduled on/off times. Features a secure HTTPS web interface for configuration and real-time clock synchronization via NTP.

## Features

- **WiFi Connectivity**:  Automatic connection with fixed credentials
- **NTP Time Synchronization**: Accurate timekeeping from internet time servers
- **HTTPS Web Interface**: Secure web server with SSL/TLS encryption
- **Relay Timer Control**: Schedule relay on/off times through web interface
- **Persistent Storage**: Timer settings saved in non-volatile memory
- **Real-time Status**: View current time and relay state
- **Mobile Friendly**: Responsive web design

## Hardware Requirements

- ESP8266 development board (NodeMCU, Wemos D1 Mini, etc.)
- Relay module (1-channel, 5V or 3.3V compatible)
- Power supply (5V USB or appropriate voltage for your setup)
- Optional: LED for status indication

## Pin Configuration

Default GPIO assignments (configurable in code):

| Function | GPIO Pin | Notes |
|----------|----------|-------|
| Relay Control | GPIO 5 (D1) | Can be changed in menuconfig |
| Status LED | GPIO 2 (D4) | Built-in LED on most boards |

## Software Requirements

- ESP-IDF v4.4 or later (ESP8266 RTOS SDK)
- Python 3.6 or later
- Git

## Installation & Setup

### 1. Install ESP-IDF for ESP8266

```bash
# Clone ESP8266 RTOS SDK
git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK.git
cd ESP8266_RTOS_SDK
./install. sh
.  ./export.sh
```

### 2. Clone This Repository

```bash
git clone https://github.com/sftengine/EspTimer.git
cd EspTimer
```

### 3. Configure the Project

```bash
idf.py menuconfig
```

Configure the following settings: 

#### WiFi Configuration
Navigate to:  `Component config → WiFi Configuration`
- **WiFi SSID**: Your WiFi network name
- **WiFi Password**: Your WiFi password

#### Timer Configuration
Navigate to: `Component config → Timer Configuration`
- **Relay GPIO Pin**: GPIO pin connected to relay (default: 5)
- **Default Timezone**: Your timezone offset in hours (e.g., +1 for CET)

#### NTP Configuration
Navigate to: `Component config → NTP Configuration`
- **NTP Server 1**: Primary NTP server (default: pool.ntp.org)
- **NTP Server 2**: Secondary NTP server (default: time.google.com)

### 4. Build and Flash

```bash
# Build the project
idf.py build

# Flash to ESP8266 (replace PORT with your serial port)
idf.py -p /dev/ttyUSB0 flash

# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor
```

Common serial ports:
- Linux: `/dev/ttyUSB0` or `/dev/ttyACM0`
- macOS: `/dev/cu.usbserial-*`
- Windows: `COM3`, `COM4`, etc.

## Usage

### First Boot

1. Power on your ESP8266
2. Wait for WiFi connection (check serial monitor)
3. Note the IP address displayed in serial output
4. The device will automatically sync time from NTP servers

### Accessing the Web Interface

1. Open a web browser
2. Navigate to: `https://<ESP8266_IP_ADDRESS>`
   - Example: `https://192.168.1.100`
3. Accept the security warning (self-signed certificate)
4. You'll see the Web Timer interface

### Setting Timer

1. **View Current Time**:  Displayed at the top of the page
2. **Set ON Time**: 
   - Enter hour (0-23) and minute (0-59)
   - This is when the relay will turn ON
3. **Set OFF Time**:
   - Enter hour (0-23) and minute (0-59)
   - This is when the relay will turn OFF
4. **Click "Save Timer"**:  Settings are saved to non-volatile memory
5. **Relay Status**: Shows current relay state (ON/OFF)

### Timer Behavior

- Timer checks every minute if relay should be on or off
- Settings persist across reboots
- Supports timers spanning midnight (e.g., ON at 23:00, OFF at 06:00)
- Manual override available through web interface

## Web Interface Example

```
╔═══════════════════════════════════╗
║      ESP8266 Web Timer           ║
╠═══════════════════════════════════╣
║ Current Time: 14:35: 22            ║
║                                   ║
║ Timer Settings                    ║
║ ┌─────────────────────────────┐  ║
║ │ ON Time:   [08]:[00]         │  ║
║ │ OFF Time: [18]:[30]         │  ║
║ │         [Save Timer]         │  ║
║ └─────────────────────────────┘  ║
║                                   ║
║ Relay Status: ● ON               ║
╚═══════════════════════════════════╝
```

## Configuration Files

### WiFi Credentials

Edit in `main/Kconfig. projbuild` or via menuconfig:

```c
#define WIFI_SSID      "YourNetworkName"
#define WIFI_PASSWORD  "YourPassword"
```

### Timezone

Set your timezone offset in hours:

```c
#define TIMEZONE_OFFSET 1  // +1 for CET, -5 for EST, etc.
```

### NTP Servers

Configure NTP servers for time synchronization:

```c
#define NTP_SERVER_1 "pool.ntp. org"
#define NTP_SERVER_2 "time.google.com"
#define NTP_SERVER_3 "time.cloudflare.com"
```

## Troubleshooting

### WiFi Connection Issues

- Verify SSID and password in menuconfig
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP8266 doesn't support 5GHz)
- Monitor serial output for error messages

### Cannot Access Web Interface

- Check IP address in serial monitor
- Verify ESP8266 and computer are on same network
- Accept browser security warning (self-signed certificate)
- Try `http://` instead of `https://` if HTTPS fails
- Disable firewall temporarily for testing

### Time Not Syncing

- Verify internet connectivity
- Check NTP server addresses
- Ensure router allows NTP traffic (UDP port 123)
- Check serial monitor for NTP errors

### Relay Not Switching

- Verify GPIO pin configuration
- Check relay module power supply
- Test relay with manual override
- Verify timer settings are saved correctly
- Check relay module type (active HIGH or LOW)

### Memory Issues

- ESP8266 has limited RAM (~80KB available)
- If experiencing crashes, reduce buffer sizes
- Monitor heap memory in serial output
- Simplify HTML if needed

## Project Structure

```
EspTimer/
├── CMakeLists.txt
├── README.md
├── sdkconfig.defaults
├── main/
│   ├── CMakeLists.txt
│   ├── Kconfig.projbuild
│   ├── main.c
│   ├── wifi_manager.c
│   ├── wifi_manager.h
│   ├── ntp_sync.c
│   ├── ntp_sync.h
│   ├── https_server.c
│   ├── https_server.h
│   ├── relay_control.c
│   ├── relay_control.h
│   ├── timer_storage.c
│   └── timer_storage.h
└── components/
    └── certs/
        ├── server_cert.pem
        └── server_key.pem
```

## API Reference

### Relay Control

```c
// Initialize relay GPIO
void relay_init(void);

// Set relay state
void relay_set_state(bool state);

// Get current relay state
bool relay_get_state(void);
```

### Timer Storage

```c
// Save timer settings to NVS
esp_err_t timer_save_settings(uint8_t on_hour, uint8_t on_min, 
                               uint8_t off_hour, uint8_t off_min);

// Load timer settings from NVS
esp_err_t timer_load_settings(uint8_t *on_hour, uint8_t *on_min,
                               uint8_t *off_hour, uint8_t *off_min);
```

### NTP Sync

```c
// Initialize and sync NTP time
esp_err_t ntp_sync_time(void);

// Get current time string
void get_current_time_str(char *buffer, size_t buffer_size);
```

## Security Considerations

- **Self-Signed Certificate**: The HTTPS server uses a self-signed certificate.  For production, consider using a proper certificate. 
- **No Authentication**: The web interface has no password protection.  Add authentication for secure environments.
- **Network Security**: Ensure your WiFi network is secured with WPA2/WPA3.
- **Firmware Updates**: Keep ESP-IDF updated for security patches. 

## Performance

- **Boot Time**: ~5-10 seconds to WiFi connection
- **NTP Sync**: 1-3 seconds
- **Web Response**: <500ms
- **Timer Accuracy**: ±1 minute
- **Memory Usage**: ~40KB RAM, ~300KB Flash

## Known Limitations

- ESP8266 does not have hardware RTC (requires periodic NTP sync or external RTC)
- Self-signed HTTPS certificate will show browser warnings
- Single relay control (can be extended for multiple relays)
- No user authentication on web interface
- Limited to 2.4GHz WiFi networks

## Future Enhancements

- [ ] Multiple relay support
- [ ] Weekly schedule (different times per day)
- [ ] User authentication
- [ ] MQTT integration
- [ ] Mobile app
- [ ] Temperature/humidity sensor integration
- [ ] OTA (Over-The-Air) firmware updates
- [ ] Backup power monitoring

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Espressif Systems for ESP-IDF framework
- ESP8266 community for examples and support
- NTP Pool Project for time synchronization

## Support

For issues, questions, or suggestions: 
- Open an issue on GitHub
- Check existing issues for solutions
- Review ESP-IDF documentation

## Author

**sftengine**
- GitHub: [@sftengine](https://github.com/sftengine)

## Version History

- **v1.0.0** (Initial Release)
  - WiFi connectivity with fixed credentials
  - NTP time synchronization
  - HTTPS web interface
  - Relay timer control
  - Persistent storage

---

**Last Updated**: 2025-12-21
```
