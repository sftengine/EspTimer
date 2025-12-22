# Example Configurations

This directory contains example configurations for common use cases.

## Basic Configuration

### Kconfig Settings

#### WiFi Configuration
```
CONFIG_WIFI_SSID="MyHomeNetwork"
CONFIG_WIFI_PASSWORD="MySecurePassword123"
CONFIG_WIFI_MAXIMUM_RETRY=5
```

#### NTP Configuration
```
CONFIG_NTP_SERVER_1="pool.ntp.org"
CONFIG_NTP_SERVER_2="time.google.com"
CONFIG_NTP_SERVER_3="time.cloudflare.com"
CONFIG_TIMEZONE_OFFSET=0
```

#### Timer Configuration
```
CONFIG_RELAY_GPIO_PIN=5
CONFIG_HTTPS_SERVER_PORT=443
CONFIG_RELAY_ACTIVE_HIGH=y
```

## Use Case Examples

### 1. Garden Irrigation System

**Scenario**: Water garden daily from 6:00 AM to 6:30 AM

**Timer Settings** (via web interface):
- ON Time: 06:00
- OFF Time: 06:30
- Timezone: Adjust to your local time

**Hardware**:
- Relay controls 24V AC irrigation valve
- Use appropriate relay (5A minimum for valve)

### 2. Outdoor Lighting

**Scenario**: Turn on lights from sunset to sunrise (approximate)

**Timer Settings**:
- ON Time: 19:00 (7:00 PM)
- OFF Time: 06:00 (6:00 AM)
- Note: Crosses midnight, handled automatically

**Hardware**:
- Relay controls outdoor light switch
- Use solid-state relay for AC loads

### 3. Aquarium Light

**Scenario**: Aquarium light on from 8:00 AM to 8:00 PM

**Timer Settings**:
- ON Time: 08:00
- OFF Time: 20:00

**Hardware**:
- Relay controls 12V DC LED strip
- Use low-voltage relay or MOSFET

### 4. Pool Pump

**Scenario**: Run pool pump 6 hours daily (10:00 AM to 4:00 PM)

**Timer Settings**:
- ON Time: 10:00
- OFF Time: 16:00

**Hardware**:
- Use heavy-duty relay rated for pump motor
- Consider contactor for large motors
- Ensure proper electrical safety

### 5. Greenhouse Fan

**Scenario**: Ventilation fan runs overnight (8:00 PM to 6:00 AM)

**Timer Settings**:
- ON Time: 20:00
- OFF Time: 06:00

**Hardware**:
- Relay controls fan power supply
- Use relay rated for fan current

### 6. Security Camera Infrared

**Scenario**: IR illuminator on at night (sunset to sunrise)

**Timer Settings**:
- ON Time: 21:00 (9:00 PM)
- OFF Time: 07:00 (7:00 AM)

**Hardware**:
- Relay controls IR LED power
- Low voltage DC relay sufficient

### 7. Heater/Dehumidifier

**Scenario**: Run dehumidifier during off-peak hours (11:00 PM to 7:00 AM)

**Timer Settings**:
- ON Time: 23:00
- OFF Time: 07:00

**Hardware**:
- Use solid-state relay for AC appliances
- Ensure relay rated for appliance power

## Timezone Examples

### Common Timezones

```
# UTC (London in winter)
CONFIG_TIMEZONE_OFFSET=0

# CET - Central European Time (Berlin, Paris)
CONFIG_TIMEZONE_OFFSET=1

# CEST - Central European Summer Time
CONFIG_TIMEZONE_OFFSET=2

# EST - Eastern Standard Time (New York)
CONFIG_TIMEZONE_OFFSET=-5

# PST - Pacific Standard Time (Los Angeles)
CONFIG_TIMEZONE_OFFSET=-8

# CST - China Standard Time (Beijing)
CONFIG_TIMEZONE_OFFSET=8

# JST - Japan Standard Time (Tokyo)
CONFIG_TIMEZONE_OFFSET=9

# AEST - Australian Eastern Standard Time (Sydney)
CONFIG_TIMEZONE_OFFSET=10
```

## NTP Server Alternatives

### Regional NTP Servers

**North America**:
```
CONFIG_NTP_SERVER_1="time.nist.gov"
CONFIG_NTP_SERVER_2="time-a-g.nist.gov"
CONFIG_NTP_SERVER_3="pool.ntp.org"
```

**Europe**:
```
CONFIG_NTP_SERVER_1="ptbtime1.ptb.de"
CONFIG_NTP_SERVER_2="europe.pool.ntp.org"
CONFIG_NTP_SERVER_3="time.cloudflare.com"
```

**Asia**:
```
CONFIG_NTP_SERVER_1="ntp.nict.jp"
CONFIG_NTP_SERVER_2="asia.pool.ntp.org"
CONFIG_NTP_SERVER_3="time.google.com"
```

**Australia**:
```
CONFIG_NTP_SERVER_1="ntp.nict.jp"
CONFIG_NTP_SERVER_2="oceania.pool.ntp.org"
CONFIG_NTP_SERVER_3="time.cloudflare.com"
```

## GPIO Pin Mappings

### NodeMCU Pin Labels to GPIO

| NodeMCU | GPIO | Notes |
|---------|------|-------|
| D0 | GPIO 16 | No PWM/interrupt, use for relay OK |
| D1 | GPIO 5 | **Default relay pin** |
| D2 | GPIO 4 | Good for relay |
| D3 | GPIO 0 | Boot mode pin, avoid if possible |
| D4 | GPIO 2 | Built-in LED, boot mode pin |
| D5 | GPIO 14 | Good for relay |
| D6 | GPIO 12 | Good for relay |
| D7 | GPIO 13 | Good for relay |
| D8 | GPIO 15 | Boot mode pin, avoid if possible |

**Recommended pins for relay**: D1 (GPIO 5), D2 (GPIO 4), D5-D7 (GPIO 12-14)

### Wemos D1 Mini Pin Labels

| D1 Mini | GPIO | Notes |
|---------|------|-------|
| D0 | GPIO 16 | No PWM/interrupt |
| D1 | GPIO 5 | **Default relay pin** |
| D2 | GPIO 4 | Good for relay |
| D3 | GPIO 0 | Boot mode pin |
| D4 | GPIO 2 | Built-in LED |
| D5 | GPIO 14 | Good for relay |
| D6 | GPIO 12 | Good for relay |
| D7 | GPIO 13 | Good for relay |
| D8 | GPIO 15 | Boot mode pin |

## Advanced Configuration

### High-Frequency Timer Checks

For more precise timing, modify `main.c`:

```c
// Check every 10 seconds instead of 60
vTaskDelay(10000 / portTICK_PERIOD_MS);
```

### Multiple Relays

To control multiple relays, duplicate relay control logic:

1. Add more GPIO configs in `Kconfig.projbuild`
2. Modify `relay_control.c` to handle array of relays
3. Update web interface for multiple timer sets

### Custom Web Interface

Edit `https_server.c` to customize HTML/CSS:
- Modify `html_page` string
- Add custom styling
- Add more features (manual override button, etc.)

## Testing Configurations

### Development/Testing Setup

```
# Use open test WiFi or hotspot
CONFIG_WIFI_SSID="TestNetwork"
CONFIG_WIFI_PASSWORD="test1234"

# Use default NTP servers
CONFIG_NTP_SERVER_1="pool.ntp.org"

# Use built-in LED for testing (GPIO 2)
CONFIG_RELAY_GPIO_PIN=2
CONFIG_RELAY_ACTIVE_HIGH=n  # Built-in LED is active LOW

# Test with short intervals (5 min ON, 5 min OFF)
# Set via web interface: ON=00:00, OFF=00:05
```

## Troubleshooting Configurations

### Debug Logging

Enable verbose logging in `sdkconfig`:
```
CONFIG_LOG_DEFAULT_LEVEL_DEBUG=y
CONFIG_LOG_DEFAULT_LEVEL=4
```

### Memory Monitoring

Add to `sdkconfig.defaults`:
```
CONFIG_HEAP_POISONING_LIGHT=y
CONFIG_HEAP_TRACING_STANDALONE=y
```

## Security Hardening

### Change Default Port

```
CONFIG_HTTPS_SERVER_PORT=8443
```

### Disable HTTP Fallback

Ensure only HTTPS is used (already default in this implementation).

## Production Recommendations

1. **Change WiFi credentials** before deployment
2. **Set correct timezone** for your location
3. **Use local NTP servers** for better reliability
4. **Test timer logic** before connecting critical loads
5. **Use appropriate power supplies** for relays
6. **Follow electrical safety codes** for AC wiring
7. **Consider backup power** (UPS) for critical applications
8. **Monitor system logs** initially to catch issues
9. **Use proper relay ratings** for your load
10. **Add physical switches** for manual override
