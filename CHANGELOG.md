# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-12-21

### Added
- Initial release of ESP8266 Web Timer
- WiFi connectivity with automatic connection and reconnection
- NTP time synchronization with multiple server support
- HTTPS web server with self-signed certificate
- Responsive web interface for timer configuration
- Real-time clock display on web page
- Relay control via GPIO with configurable active level
- Timer scheduling with ON/OFF times
- Midnight-spanning timer support (e.g., 23:00 ON to 06:00 OFF)
- Persistent storage of timer settings in NVS
- Automatic relay control based on scheduled times
- System status monitoring and logging
- JSON API endpoints for status and configuration
- Configurable parameters via Kconfig/menuconfig:
  - WiFi SSID and password
  - NTP server addresses (3 servers)
  - Timezone offset
  - Relay GPIO pin
  - HTTPS server port
  - Relay active level (HIGH/LOW)
- Comprehensive documentation (README.md, BUILD.md)
- MIT License

### Technical Details
- Built with ESP-IDF (ESP8266 RTOS SDK)
- Memory optimized for ESP8266 constraints (~80KB RAM)
- FreeRTOS task-based architecture
- Secure HTTPS communication
- Event-driven WiFi management
- Efficient timer checking (1-minute intervals)
- Clean separation of concerns (modular components)

### Components
- `wifi_manager` - WiFi connection handling
- `ntp_sync` - NTP time synchronization
- `relay_control` - GPIO relay control
- `timer_storage` - NVS persistent storage
- `https_server` - HTTPS web server with embedded HTML

### Security
- SSL/TLS encryption for web interface
- Self-signed certificate included
- Input validation on API endpoints
- Safe defaults for all configuration options

## [Unreleased]

### Planned Features
- Multiple relay support
- Weekly schedule (different times per day of week)
- User authentication for web interface
- MQTT integration
- Mobile application
- Temperature/humidity sensor integration
- OTA (Over-The-Air) firmware updates
- Manual relay override button
- Web interface themes
- Email/push notifications
- Backup power monitoring
- Historical logging
