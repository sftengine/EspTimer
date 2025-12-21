# Build and Setup Guide

## Quick Start

This document provides step-by-step instructions for building and deploying the ESP8266 Web Timer application.

## Prerequisites

### Hardware
- ESP8266 development board (NodeMCU, Wemos D1 Mini, etc.)
- USB cable for programming
- Relay module (3.3V or 5V compatible)
- Jumper wires

### Software
- ESP-IDF for ESP8266 (ESP8266_RTOS_SDK) v3.4 or later
- Python 3.6 or later
- Git
- Serial terminal (screen, minicom, or use idf.py monitor)

## Installation Steps

### 1. Install ESP8266 RTOS SDK

```bash
# Clone the ESP8266 RTOS SDK
cd ~/esp
git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK.git
cd ESP8266_RTOS_SDK

# Install required packages (Ubuntu/Debian)
sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python3 python3-pip python3-serial

# Run the install script
./install.sh

# Set up environment variables (add to ~/.bashrc for persistence)
. ./export.sh
```

### 2. Clone and Configure Project

```bash
# Clone this repository
git clone https://github.com/sftengine/EspTimer.git
cd EspTimer

# Configure the project
idf.py menuconfig
```

### 3. Configure Settings in menuconfig

Navigate through these menus and set your values:

#### WiFi Configuration
- Path: `Component config → WiFi Configuration`
- Set **WiFi SSID**: Your WiFi network name
- Set **WiFi Password**: Your WiFi password
- Set **Maximum retry**: Number of connection retries (default: 5)

#### NTP Configuration
- Path: `Component config → NTP Configuration`
- Set **NTP Server 1**: Primary server (default: pool.ntp.org)
- Set **NTP Server 2**: Secondary server (default: time.google.com)
- Set **NTP Server 3**: Tertiary server (default: time.cloudflare.com)
- Set **Timezone offset**: Your timezone in hours from UTC

#### Timer Configuration
- Path: `Component config → Timer Configuration`
- Set **Relay GPIO Pin**: GPIO pin for relay (default: 5 / D1)
- Set **HTTPS Server Port**: Port for web interface (default: 443)
- Set **Relay Active High**: Enable if relay turns ON with HIGH signal

### 4. Build the Project

```bash
# Clean build (if rebuilding)
idf.py fullclean

# Build the project
idf.py build
```

Expected output:
```
Project build complete. To flash, run this command:
python -m esptool --chip esp8266 --port /dev/ttyUSB0 --baud 460800 ...
```

### 5. Flash to ESP8266

```bash
# Flash the firmware (replace /dev/ttyUSB0 with your port)
idf.py -p /dev/ttyUSB0 flash

# For faster flashing
idf.py -p /dev/ttyUSB0 -b 460800 flash
```

Common ports:
- **Linux**: `/dev/ttyUSB0`, `/dev/ttyACM0`
- **macOS**: `/dev/cu.usbserial-*`, `/dev/cu.SLAB_USBtoUART`
- **Windows**: `COM3`, `COM4`, etc.

### 6. Monitor Serial Output

```bash
idf.py -p /dev/ttyUSB0 monitor

# To exit monitor: Ctrl+]
```

## Wiring Diagram

### ESP8266 to Relay Module

```
ESP8266 (NodeMCU)     Relay Module
==================    ============
GPIO 5 (D1)    -----> IN
3.3V           -----> VCC
GND            -----> GND
```

**Important Notes:**
- Use appropriate power supply for relay (5V for 5V relays)
- Some relays may need a level shifter for 3.3V logic
- For AC loads, ensure proper isolation and safety measures
- Never connect AC mains directly to ESP8266

## First Boot

After flashing, the ESP8266 will:

1. **Connect to WiFi** (10-30 seconds)
   - Watch serial output for connection status
   - Note the IP address assigned

2. **Sync time with NTP** (2-5 seconds)
   - Confirms internet connectivity

3. **Start HTTPS server** (1-2 seconds)
   - Server runs on port 443 (or configured port)

4. **Display access information**:
   ```
   =====================================
   WiFi Connected!
   IP Address: 192.168.1.100
   Web Interface: https://192.168.1.100
   =====================================
   System Ready!
   =====================================
   ```

## Accessing the Web Interface

1. Open a web browser
2. Navigate to `https://<ESP_IP_ADDRESS>`
3. **Accept the security warning** (self-signed certificate)
4. You'll see the ESP8266 Web Timer interface

### Browser-Specific Instructions

**Chrome/Edge:**
- Click "Advanced" → "Proceed to [IP] (unsafe)"

**Firefox:**
- Click "Advanced" → "Accept the Risk and Continue"

**Safari:**
- Click "Show Details" → "visit this website"

## Troubleshooting

### Build Errors

**Error: ESP-IDF not found**
```bash
# Source the environment
cd ~/esp/ESP8266_RTOS_SDK
. ./export.sh
```

**Error: sdkconfig not found**
```bash
# Reconfigure
idf.py menuconfig
```

### Flash Errors

**Error: Serial port not found**
```bash
# Check available ports
ls /dev/tty* | grep USB

# Add user to dialout group (Linux)
sudo usermod -a -G dialout $USER
# Log out and back in
```

**Error: Timed out waiting for packet header**
```bash
# Try slower baud rate
idf.py -p /dev/ttyUSB0 -b 115200 flash

# Hold BOOT/FLASH button during upload
# Reset ESP8266 before flashing
```

### Runtime Errors

**WiFi Connection Failed**
- Verify SSID and password in menuconfig
- Check 2.4GHz WiFi (ESP8266 doesn't support 5GHz)
- Check WiFi signal strength
- Review serial output for error codes

**NTP Sync Failed**
- Verify internet connectivity
- Check router firewall (allow UDP port 123)
- Try different NTP servers
- Check timezone offset

**Cannot Access Web Interface**
- Verify ESP8266 IP address from serial monitor
- Ensure computer and ESP8266 on same network
- Try `http://` instead of `https://`
- Disable firewall temporarily
- Check HTTPS port configuration

**Relay Not Working**
- Verify GPIO pin number in menuconfig
- Check wiring connections
- Test with LED first
- Verify relay power supply
- Check relay active level (HIGH/LOW)

## Monitoring and Debugging

### View Logs

```bash
# Real-time monitoring
idf.py monitor

# Filter by log level
idf.py monitor | grep -i error
idf.py monitor | grep -i wifi
```

### Check Memory Usage

Logs show heap memory:
```
Free heap: 45234 bytes
```

If memory is low (<10KB):
- Reduce HTML page size
- Decrease HTTPD buffer sizes in sdkconfig
- Optimize code

### Debug Mode

Enable verbose logging in menuconfig:
```
Component config → Log output → Default log verbosity → Verbose
```

## Updating Firmware

### Over Serial (USB)

```bash
# Just reflash
idf.py -p /dev/ttyUSB0 flash
```

### Settings Preservation

Timer settings are stored in NVS and persist across reflashes unless you:
```bash
# Erase entire flash (including NVS)
idf.py -p /dev/ttyUSB0 erase_flash

# Then reflash
idf.py -p /dev/ttyUSB0 flash
```

## Performance Optimization

### Reduce Memory Usage

1. In `sdkconfig.defaults`, adjust:
```
CONFIG_MBEDTLS_SSL_MAX_CONTENT_LEN=4096
CONFIG_HTTPD_MAX_REQ_HDR_LEN=256
```

2. Minify HTML in `https_server.c`

### Improve Response Time

1. Increase CPU frequency (menuconfig):
```
Component config → ESP8266-specific → CPU frequency → 160 MHz
```

2. Optimize task priorities in `main.c`

## Production Deployment

### Security Enhancements

1. **Use proper SSL certificate** (Let's Encrypt, etc.)
2. **Add authentication** to web interface
3. **Change default timer values**
4. **Use secure WiFi (WPA3)**

### Reliability

1. **Add watchdog timer** monitoring
2. **Implement OTA updates**
3. **Add status LED** for visual feedback
4. **External RTC** for time backup
5. **Power supply stability** with capacitors

## Additional Resources

- [ESP8266 RTOS SDK Documentation](https://docs.espressif.com/projects/esp8266-rtos-sdk/)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [ESP8266 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)

## Support

For issues and questions:
- GitHub Issues: https://github.com/sftengine/EspTimer/issues
- ESP8266 Community Forum
- Stack Overflow (tag: esp8266)
