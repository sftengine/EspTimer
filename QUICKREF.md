# Quick Reference Guide

## Essential Commands

### Build & Flash
```bash
# Configure project
idf.py menuconfig

# Build
idf.py build

# Flash to device
idf.py -p /dev/ttyUSB0 flash

# Monitor output
idf.py -p /dev/ttyUSB0 monitor

# All in one
idf.py -p /dev/ttyUSB0 flash monitor
```

### Common Port Names
- **Linux**: `/dev/ttyUSB0`, `/dev/ttyACM0`
- **macOS**: `/dev/cu.usbserial-*`
- **Windows**: `COM3`, `COM4`

## GPIO Pin Reference

### NodeMCU Pinout
```
        ┌─────────────┐
        │   NodeMCU   │
        │   ESP-12E   │
    D0  │ GPIO16      │  3V3
    D1  │ GPIO5  ●    │  GND  ← Connect relay GND
    D2  │ GPIO4       │  TX
    D3  │ GPIO0       │  RX
    D4  │ GPIO2       │  D8
    3V3 │             │  D7
    GND │             │  D6
        │  ┌───┐      │  D5
        │  │USB│      │  GND
        │  └───┘      │  3V3
        └─────────────┘

● = Default relay pin (GPIO5 / D1)
```

### Wemos D1 Mini Pinout
```
   ┌───────────┐
   │ D1 Mini   │
RST│           │TX
 A0│           │RX
 D0│ GPIO16    │D1  GPIO5  ● ← Default relay pin
 D5│ GPIO14    │D2  GPIO4
 D6│ GPIO12    │D3  GPIO0
 D7│ GPIO13    │D4  GPIO2
 D8│ GPIO15    │GND
3V3│           │5V
   └───────────┘
```

## Configuration Quick Reference

### menuconfig Navigation
- Arrow keys: Navigate
- Enter: Select/Edit
- Esc: Back
- Space: Toggle boolean
- `/`: Search
- `?`: Help
- `S`: Save and exit
- `Q`: Quit without saving

### Essential Settings

```
WiFi Configuration:
├─ WiFi SSID: "YourNetwork"
├─ WiFi Password: "YourPassword"
└─ Maximum retry: 5

NTP Configuration:
├─ NTP Server 1: "pool.ntp.org"
├─ NTP Server 2: "time.google.com"
├─ NTP Server 3: "time.cloudflare.com"
└─ Timezone offset: 0 (UTC)

Timer Configuration:
├─ Relay GPIO Pin: 5 (D1)
├─ HTTPS Server Port: 443
└─ Relay Active High: Yes
```

## Web Interface

### Access
```
URL: https://<ESP8266_IP>
Example: https://192.168.1.100
```

### API Endpoints

#### Get Status
```bash
curl -k https://192.168.1.100/api/status
```
Response:
```json
{
  "time": "2025-12-21 14:35:22",
  "relay_on": true,
  "on_hour": 8,
  "on_min": 0,
  "off_hour": 18,
  "off_min": 0
}
```

#### Set Timer
```bash
curl -k -X POST https://192.168.1.100/api/timer \
  -H "Content-Type: application/json" \
  -d '{"on_hour":8,"on_min":0,"off_hour":18,"off_min":30}'
```
Response:
```json
{
  "message": "Timer settings saved successfully",
  "success": true
}
```

## Timer Examples

### Regular Schedule (Same Day)
```
ON:  08:00 (8 AM)
OFF: 18:00 (6 PM)

Active from 8:00 to 18:00
```

### Overnight Schedule (Crosses Midnight)
```
ON:  20:00 (8 PM)
OFF: 06:00 (6 AM)

Active from 20:00 to 06:00 next day
```

### All Night Schedule
```
ON:  22:00 (10 PM)
OFF: 08:00 (8 AM)

Active from 22:00 to 08:00 next day
```

## Wiring Examples

### Basic Relay Connection
```
ESP8266          Relay Module
========         ============
GPIO5 (D1) ----> IN
GND        ----> GND
3V3        ----> VCC (if 3.3V relay)
5V         ----> VCC (if 5V relay)
```

### With Status LED
```
ESP8266          Components
========         ==========
GPIO5 (D1) ----> Relay IN
GPIO2 (D4) ----> LED + (built-in)
GND        ----> LED - (built-in)
```

### Safety with AC Loads
```
Relay           AC Load
=====           =======
COM  ---------> AC Hot (switched)
NO   ---------> Device Hot
              
AC Neutral ---> Device Neutral
AC Ground  ---> Device Ground

⚠️ WARNING: High voltage! Use proper enclosure!
```

## Serial Monitor Output

### Normal Startup
```
I (123) wifi_manager: WiFi initialization finished.
I (456) wifi_manager: Connected to AP SSID:MyNetwork
I (789) wifi_manager: Got IP address: 192.168.1.100
I (1234) ntp_sync: Time synchronized
I (1567) https_server: HTTPS server started successfully
I (1890) main: System Ready!
```

### Timer Trigger
```
I (60123) main: Timer triggered: Relay turned ON at 08:00
I (600123) main: Timer triggered: Relay turned OFF at 18:00
```

### Status Monitoring
```
I (10000) main: Status - Time: 14:35:22, Relay: ON, Heap: 45234 bytes
```

## Troubleshooting Commands

### Check Flash
```bash
esptool.py --port /dev/ttyUSB0 flash_id
```

### Erase Flash
```bash
idf.py -p /dev/ttyUSB0 erase_flash
```

### Check Partitions
```bash
idf.py partition_table
```

### Full Clean Build
```bash
idf.py fullclean
idf.py build
```

## Timezone Quick Reference

| Location | Offset | Example City |
|----------|--------|--------------|
| UTC | 0 | London (winter) |
| CET | +1 | Berlin, Paris |
| CEST | +2 | Berlin (summer) |
| EET | +2 | Athens, Cairo |
| MSK | +3 | Moscow |
| GST | +4 | Dubai |
| IST | +5:30 | India |
| CST | +8 | Beijing, Singapore |
| JST | +9 | Tokyo |
| AEST | +10 | Sydney |
| EST | -5 | New York |
| CST | -6 | Chicago |
| MST | -7 | Denver |
| PST | -8 | Los Angeles |

**Note**: Set timezone in menuconfig as hours from UTC.

## Memory Management

### Check Available Memory
```bash
# In monitor, look for:
Free heap: XXXXX bytes
```

### Typical Memory Usage
- **Good**: >30 KB free
- **Warning**: 10-30 KB free
- **Critical**: <10 KB free

### Reduce Memory Usage
1. Decrease `CONFIG_MBEDTLS_SSL_MAX_CONTENT_LEN`
2. Reduce `CONFIG_HTTPD_MAX_REQ_HDR_LEN`
3. Minify HTML in `https_server.c`

## Common Error Codes

| Error | Meaning | Solution |
|-------|---------|----------|
| ESP_ERR_NVS_NOT_FOUND | Timer not set | Normal on first boot |
| ESP_ERR_WIFI_CONN | WiFi failed | Check SSID/password |
| ESP_ERR_TIMEOUT | Operation timeout | Retry or check network |
| ESP_ERR_NO_MEM | Out of memory | Reduce buffer sizes |

## LED Indicators

Most ESP8266 boards have built-in LED on GPIO2:

- **Fast blink**: Connecting to WiFi
- **Slow blink**: Normal operation
- **Solid on**: Error state
- **Off**: (depends on implementation)

## Maintenance

### Daily
- None required (automatic operation)

### Weekly
- Check logs for errors
- Verify time accuracy

### Monthly
- Check relay operation
- Clean dust from enclosure
- Verify timer still synchronized

### Yearly
- Update firmware if needed
- Check relay contacts
- Verify wiring integrity

## Backup Configuration

Save your settings:
```bash
# Copy sdkconfig
cp sdkconfig sdkconfig.backup

# Note timer settings from web interface
# (stored in NVS, preserved across reflashes)
```

## Default Values

If you need to reset to defaults:

**WiFi**: Set in menuconfig
**NTP**: pool.ntp.org, time.google.com, time.cloudflare.com
**Timezone**: UTC (0)
**GPIO Pin**: 5 (D1)
**HTTPS Port**: 443
**Default Timer**: ON=08:00, OFF=18:00

## Performance Tips

1. **Use local NTP servers** for faster sync
2. **Place ESP8266 near WiFi AP** for better signal
3. **Use quality power supply** (minimum 500mA)
4. **Keep firmware updated** for best performance
5. **Monitor heap memory** to prevent crashes

## Safety Checklist

Before deploying:
- [ ] WiFi credentials configured
- [ ] Timezone set correctly
- [ ] GPIO pin tested with LED first
- [ ] Relay rated for load
- [ ] Proper electrical isolation
- [ ] Enclosed in safety box
- [ ] AC wiring by qualified person
- [ ] Fuse protection in place
- [ ] Emergency stop accessible
- [ ] Documentation nearby

## Quick Debug

Problem solving in 60 seconds:
```bash
# 1. Connect serial
idf.py -p /dev/ttyUSB0 monitor

# 2. Look for these lines:
"WiFi Connected" → WiFi OK
"Got IP address" → Network OK
"Time synchronized" → NTP OK
"HTTPS server started" → Web OK
"System Ready" → All OK

# 3. If error, check that component
```

## Useful Links

- **ESP-IDF Docs**: https://docs.espressif.com/
- **This Project**: https://github.com/sftengine/EspTimer
- **ESP8266 Pinout**: Search "ESP8266 pinout" + your board model
- **NTP Servers**: https://www.ntppool.org/

## Version Info

Check your version:
```bash
grep "v[0-9]" CHANGELOG.md | head -1
```

Latest: v1.0.0 (2025-12-21)
