# Frequently Asked Questions (FAQ)

## General Questions

### What is ESP8266 Web Timer?
ESP8266 Web Timer is a complete firmware solution for ESP8266 that allows you to control a relay based on scheduled times. It features a secure web interface for easy configuration.

### What can I control with this?
Anything that can be controlled by a relay:
- Lights (indoor/outdoor)
- Irrigation systems
- Pumps
- Fans
- Heaters
- Appliances
- Motors (with appropriate relay rating)

### Do I need programming experience?
No! Once you flash the firmware, everything is configured through:
1. menuconfig (one-time setup for WiFi)
2. Web interface (for timer settings)

### Is it safe to use with AC power?
If you follow proper electrical safety practices:
- Use properly rated relays
- Ensure electrical isolation
- Have qualified electrician do AC wiring
- Use proper enclosures
- Follow local electrical codes

## Hardware Questions

### Which ESP8266 boards are compatible?
- NodeMCU (ESP-12E/ESP-12F)
- Wemos D1 Mini
- ESP-01 (limited pins)
- Any ESP8266 module with sufficient flash (2MB+)

### What relay module should I use?
Recommended:
- 5V relay modules (common and cheap)
- Solid-state relays (for AC loads, quieter)
- Choose based on your load requirements

Specifications:
- Voltage rating: Higher than your load
- Current rating: 2x your load current (safety margin)
- Optical isolation (preferred)

### Can I use multiple relays?
Not out of the box, but the code can be extended to support multiple relays. See CONTRIBUTING.md for guidance.

### What about ESP32?
This project is specifically for ESP8266. ESP32 has different SDK and would require modifications.

## Software Questions

### Which ESP-IDF version do I need?
ESP8266 RTOS SDK v3.4 or later. This is different from ESP32's ESP-IDF.

### Can I use Arduino IDE?
No, this project uses ESP-IDF (ESP8266 RTOS SDK). It's not compatible with Arduino framework without significant modifications.

### How do I update the firmware?
Currently via USB cable using `idf.py flash`. OTA updates are planned for future versions.

### Can I customize the web interface?
Yes! Edit the `html_page` variable in `main/https_server.c`. You can modify HTML, CSS, and JavaScript.

## Configuration Questions

### How do I change WiFi credentials?
Run `idf.py menuconfig` and navigate to "WiFi Configuration". Set your SSID and password, then rebuild and flash.

### Can it connect to 5GHz WiFi?
No, ESP8266 only supports 2.4GHz WiFi networks.

### How do I set my timezone?
In `idf.py menuconfig`, go to "NTP Configuration" and set your timezone offset in hours from UTC.

Examples:
- UTC: 0
- New York (EST): -5
- Los Angeles (PST): -8
- Berlin (CET): +1
- Tokyo (JST): +9

### What if my timezone uses half-hour offsets?
Current implementation only supports full-hour offsets. You can modify `ntp_sync.c` to support :30 offsets (e.g., India +5:30).

## Network Questions

### Does it need internet access?
Yes, initially for NTP time sync. After sync, it can work offline, but time will drift. Reconnects to NTP periodically (on reboot).

### Can I access it from outside my home network?
Not by default. You would need to:
1. Set up port forwarding on your router
2. Use a dynamic DNS service
3. Consider security implications

**Security warning**: Not recommended without adding authentication!

### What happens if WiFi disconnects?
The device automatically attempts to reconnect. Timer continues to work based on last synchronized time, though accuracy will degrade without periodic NTP sync.

### Can I use a static IP address?
Currently uses DHCP. You can:
- Set static IP in your router (DHCP reservation)
- Modify `wifi_manager.c` to use static IP

## Timer Questions

### How accurate is the timer?
±1 minute accuracy. Timer checks every 60 seconds.

### Can I set different times for different days?
Not in the current version. It runs the same schedule every day. Weekly scheduling is planned for future releases.

### What happens at midnight?
The timer logic correctly handles midnight crossing. For example:
- ON: 22:00 (10 PM)
- OFF: 06:00 (6 AM)
This will keep the relay ON from 22:00 until 06:00 the next day.

### Can I have multiple ON/OFF cycles per day?
Not in the current version. Only one ON time and one OFF time per day. Multiple cycles is a planned enhancement.

### What if ON time equals OFF time?
The relay will remain OFF. It's recommended to have different ON and OFF times.

## Operation Questions

### What happens after a power outage?
1. ESP8266 reboots
2. Reconnects to WiFi
3. Syncs time with NTP
4. Loads timer settings from NVS (they persist!)
5. Resumes normal operation

Timer settings are NOT lost!

### How do I manually override the timer?
Currently not supported via button. You can:
- Set very short timer ranges
- Modify code to add manual control
- Access via API (future feature)

### Can I see historical logs?
No built-in logging. The serial console shows real-time logs. For historical data, you'd need to:
- Log serial output to file
- Add SD card logging (code modification)
- Send logs to external server (future MQTT feature)

### Does it work without internet?
After initial setup:
- ✅ Timer continues to work
- ✅ Web interface accessible
- ✅ Settings persist
- ⚠️ Time will slowly drift (no NTP sync)
- ❌ Cannot update time

## Web Interface Questions

### Why does my browser show a security warning?
The ESP8266 uses a self-signed SSL certificate. This is normal. Click "Advanced" and "Proceed" to access the interface.

### Can I use HTTP instead of HTTPS?
The current implementation only supports HTTPS for security. Modifying code for HTTP is possible but not recommended.

### Why is the web page slow?
Possible reasons:
- Weak WiFi signal
- ESP8266 overloaded (low memory)
- Large HTML page

Try:
- Move ESP8266 closer to router
- Reduce HTML/CSS in code
- Check free heap memory

### Can I access it from my phone?
Yes! The web interface is responsive and works on mobile browsers. Just accept the security certificate warning.

### The web page doesn't update automatically
The page refreshes status every 5 seconds automatically. If it's not updating:
- Check browser console for errors
- Ensure ESP8266 is connected
- Try refreshing the page manually

## Security Questions

### Is it secure?
Moderately secure for home use:
- ✅ HTTPS encryption
- ✅ WiFi password protection
- ❌ No web interface authentication
- ❌ Self-signed certificate

### Should I expose it to the internet?
**Not recommended** without adding:
- User authentication
- Proper SSL certificate
- Rate limiting
- Security monitoring

### Can someone hack my relay?
If they:
- Are on your WiFi network
- Know the ESP8266's IP address
Then yes, they can access it. Add authentication for secure environments.

### How do I add password protection?
This requires code modification. You would need to:
1. Add authentication logic in `https_server.c`
2. Store credentials in NVS
3. Add login page to HTML
4. Verify credentials on each request

## Troubleshooting Questions

### ESP8266 won't connect to WiFi
Check:
- ✓ Correct SSID (case-sensitive)
- ✓ Correct password
- ✓ 2.4GHz network (not 5GHz)
- ✓ Network not hidden
- ✓ Good signal strength
- ✓ Router allows new connections

### Time is wrong
Check:
- ✓ Internet connection working
- ✓ NTP servers accessible
- ✓ Timezone offset correct
- ✓ No firewall blocking UDP port 123

### Relay clicks but load doesn't turn on
Check:
- ✓ Relay rated for your load
- ✓ Proper wiring (NO vs NC terminals)
- ✓ Relay getting enough power
- ✓ Load has power supply
- ✓ Fuse/breaker not tripped

### Can't access web interface
Check:
- ✓ ESP8266 has IP address (check serial)
- ✓ Computer on same network
- ✓ Using `https://` not `http://`
- ✓ Accepting certificate warning
- ✓ Firewall not blocking

### Memory errors / crashes
Solutions:
- Reduce `CONFIG_MBEDTLS_SSL_MAX_CONTENT_LEN`
- Simplify HTML page
- Check for memory leaks (monitor heap)
- Ensure good power supply

### Flash verification failed
Try:
- Slower baud rate: `-b 115200`
- Different USB cable
- Different USB port
- Hold BOOT button during flash
- Erase flash first: `idf.py erase_flash`

## Performance Questions

### How much power does it use?
- Active (WiFi on): ~80mA @ 3.3V = ~264mW
- Can be powered by USB (500mA available)
- Use 1A power supply for relay + ESP8266

### Does it get hot?
ESP8266 should stay cool (<40°C). If hot:
- Check for short circuit
- Ensure adequate ventilation
- Verify proper voltage (3.3V for ESP8266)
- Don't overdrive GPIO pins

### Battery operation?
Possible but not ideal:
- High power consumption (WiFi)
- Would need large battery
- Consider deep sleep modifications
- Or use separate timer (DS3231) with ESP8266

### Can it handle continuous operation?
Yes! Designed for 24/7 operation. Consider:
- Good power supply
- Adequate cooling
- Quality components
- Regular monitoring (first week)

## Development Questions

### How can I contribute?
See CONTRIBUTING.md! We welcome:
- Bug reports
- Feature requests
- Code contributions
- Documentation improvements
- Testing and feedback

### Can I sell products using this?
Yes! MIT License allows commercial use. Please:
- Keep license notice
- Credit original author
- Consider contributing improvements back

### How do I add new features?
1. Fork the repository
2. Create feature branch
3. Make changes following code style
4. Test thoroughly
5. Submit pull request

See CONTRIBUTING.md for details.

### Where is the roadmap?
Check:
- CHANGELOG.md (planned features)
- GitHub Issues (feature requests)
- GitHub Projects (if available)

## Compatibility Questions

### Works with Home Assistant?
Not directly, but you could:
- Use REST API integration
- Add MQTT support (planned feature)
- Create custom integration

### Works with Alexa/Google Home?
Not currently. Would require:
- Cloud integration
- OAuth authentication
- Device discovery protocol
- Significant development

### Can it integrate with MQTT?
Not yet, but it's on the roadmap! MQTT would enable:
- Home automation integration
- Remote monitoring
- Cloud logging
- Multiple device coordination

## Support Questions

### Where do I get help?
1. Read documentation (README, BUILD, FAQ)
2. Check existing GitHub issues
3. Open new GitHub issue
4. ESP8266 community forums

### How do I report a bug?
Open a GitHub issue with:
- ESP8266 board model
- ESP-IDF version
- Steps to reproduce
- Expected vs actual behavior
- Serial monitor output
- Configuration settings

### Is there a community?
- GitHub Discussions (if enabled)
- GitHub Issues for support
- Link to your implementation (tag #ESP8266WebTimer)

### How do I stay updated?
- Watch GitHub repository
- Check CHANGELOG.md
- Follow release announcements
- Subscribe to repository

## Future Questions

### What features are coming?
See CHANGELOG.md under [Unreleased]. Top priorities:
- Multiple relay support
- Weekly scheduling
- MQTT integration
- User authentication
- OTA updates

### When is the next release?
No fixed schedule. Follow GitHub for updates.

### Can I request features?
Yes! Open a GitHub issue with:
- Feature description
- Use case
- Why it's important
- Potential implementation ideas

### Will there be a mobile app?
It's on the long-term roadmap, but no timeline yet. The web interface works well on mobile browsers for now.

---

## Still have questions?

- **Check Documentation**: README.md, BUILD.md, ARCHITECTURE.md
- **Search Issues**: github.com/sftengine/EspTimer/issues
- **Ask Community**: Open a new issue with your question
- **Improve this FAQ**: Submit PR to add your Q&A!
