# Contributing to ESP8266 Web Timer

Thank you for considering contributing to ESP8266 Web Timer! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help others learn and grow
- Maintain a positive community

## How to Contribute

### Reporting Bugs

Before creating a bug report:
1. Check existing issues to avoid duplicates
2. Test with the latest version
3. Verify it's not a configuration issue

When reporting a bug, include:
- ESP8266 board type and specifications
- ESP-IDF version
- Steps to reproduce
- Expected vs. actual behavior
- Serial monitor output (logs)
- Configuration settings (WiFi credentials removed)

### Suggesting Enhancements

Enhancement suggestions are welcome! Include:
- Clear description of the feature
- Use cases and benefits
- Potential implementation approach
- Any related examples or references

### Pull Requests

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make your changes**
   - Follow existing code style
   - Add comments for complex logic
   - Update documentation if needed

4. **Test your changes**
   - Build successfully
   - Test on actual hardware if possible
   - Verify no memory leaks
   - Check all existing features still work

5. **Commit your changes**
   ```bash
   git commit -m "Add: Brief description of changes"
   ```
   
   Commit message format:
   - `Add: New feature or file`
   - `Fix: Bug fix`
   - `Update: Modify existing feature`
   - `Docs: Documentation changes`
   - `Refactor: Code restructuring`

6. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```

7. **Open a Pull Request**
   - Describe what changes you made
   - Reference any related issues
   - Include screenshots if UI changed
   - Note any breaking changes

## Development Setup

### Prerequisites

```bash
# Install ESP8266 RTOS SDK
cd ~/esp
git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK.git
cd ESP8266_RTOS_SDK
./install.sh
. ./export.sh
```

### Local Development

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/EspTimer.git
cd EspTimer

# Create development branch
git checkout -b dev/my-changes

# Configure for development
idf.py menuconfig

# Build and test
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Coding Standards

### C Code Style

- **Indentation**: 4 spaces (no tabs)
- **Line length**: Maximum 100 characters
- **Braces**: K&R style (opening brace on same line)
- **Naming**:
  - Functions: `snake_case` (e.g., `relay_set_state`)
  - Variables: `snake_case` (e.g., `wifi_connected`)
  - Constants/Defines: `UPPER_CASE` (e.g., `CONFIG_RELAY_GPIO_PIN`)
  - Structs/Types: `snake_case_t` (e.g., `timer_config_t`)

Example:
```c
void relay_set_state(bool state)
{
    relay_state = state;
    
    if (state) {
        gpio_set_level(CONFIG_RELAY_GPIO_PIN, 1);
    } else {
        gpio_set_level(CONFIG_RELAY_GPIO_PIN, 0);
    }
    
    ESP_LOGI(TAG, "Relay set to %s", state ? "ON" : "OFF");
}
```

### Comments

- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Add function header comments:
  ```c
  /**
   * @brief Short description
   * 
   * @param param1 Description of param1
   * @return Description of return value
   */
  ```

### Logging

Use ESP-IDF logging macros:
```c
ESP_LOGI(TAG, "Informational message");
ESP_LOGW(TAG, "Warning message");
ESP_LOGE(TAG, "Error message");
ESP_LOGD(TAG, "Debug message");
```

### Error Handling

Always check return values:
```c
esp_err_t ret = function_call();
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Function failed: %s", esp_err_to_name(ret));
    return ret;
}
```

## Testing

### Manual Testing Checklist

Before submitting a PR, test:

- [ ] Build completes without warnings
- [ ] WiFi connects successfully
- [ ] NTP time syncs correctly
- [ ] Web interface loads
- [ ] Timer settings save/load
- [ ] Relay toggles as expected
- [ ] Settings persist after reboot
- [ ] Memory usage is reasonable
- [ ] No crashes or exceptions

### Hardware Testing

If possible, test on:
- NodeMCU (ESP-12E)
- Wemos D1 Mini
- Generic ESP8266 module

## Component Guidelines

### Adding New Components

1. Create header file (`component_name.h`)
2. Create implementation (`component_name.c`)
3. Add to `main/CMakeLists.txt`
4. Document in code and README
5. Add configuration to `Kconfig.projbuild` if needed

### Modifying Existing Components

- Maintain backward compatibility
- Update documentation
- Test thoroughly
- Note any breaking changes

## Documentation

### Code Documentation

- Comment complex algorithms
- Explain non-obvious decisions
- Document hardware dependencies
- Include usage examples

### README Updates

Update README.md if you:
- Add new features
- Change configuration options
- Modify pin assignments
- Update dependencies

### BUILD.md Updates

Update BUILD.md if you:
- Change build process
- Add new dependencies
- Modify menuconfig options

## Version Control

### Branch Naming

- `feature/description` - New features
- `fix/description` - Bug fixes
- `docs/description` - Documentation
- `refactor/description` - Code refactoring

### Commit Messages

Write clear, concise commit messages:

Good:
```
Add: MQTT support for remote monitoring
Fix: WiFi reconnection after connection loss
Update: Improve timer precision to 10 seconds
```

Bad:
```
Fixed stuff
Updated files
Changes
```

## Release Process

Releases follow [Semantic Versioning](https://semver.org/):

- **MAJOR**: Breaking changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes

## Areas for Contribution

We especially welcome contributions in:

### High Priority
- Multiple relay support
- Weekly scheduling
- User authentication
- MQTT integration
- OTA updates

### Medium Priority
- Mobile app
- Temperature sensor integration
- Email/push notifications
- Web interface themes
- Historical logging

### Low Priority
- Additional languages
- Advanced scheduling rules
- Cloud integration
- Voice assistant support

## Questions?

- Open an issue for questions
- Check existing issues and PRs
- Read documentation thoroughly
- Join discussions

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Recognition

Contributors will be:
- Listed in CHANGELOG.md
- Credited in release notes
- Appreciated by the community!

Thank you for contributing to ESP8266 Web Timer! 🎉
