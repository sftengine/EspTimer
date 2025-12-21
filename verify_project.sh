#!/bin/bash
# Project Verification Script

echo "=================================="
echo "ESP8266 Web Timer - Project Verification"
echo "=================================="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1"
        return 0
    else
        echo -e "${RED}✗${NC} $1 - MISSING!"
        return 1
    fi
}

check_dir() {
    if [ -d "$1" ]; then
        echo -e "${GREEN}✓${NC} $1/"
        return 0
    else
        echo -e "${RED}✗${NC} $1/ - MISSING!"
        return 1
    fi
}

ERRORS=0

echo "Checking Directory Structure..."
check_dir "main" || ((ERRORS++))
check_dir "components/certs" || ((ERRORS++))
check_dir "examples" || ((ERRORS++))
echo ""

echo "Checking Build Configuration..."
check_file "CMakeLists.txt" || ((ERRORS++))
check_file "sdkconfig.defaults" || ((ERRORS++))
check_file "main/CMakeLists.txt" || ((ERRORS++))
check_file "main/Kconfig.projbuild" || ((ERRORS++))
check_file ".gitignore" || ((ERRORS++))
echo ""

echo "Checking Source Files..."
check_file "main/main.c" || ((ERRORS++))
check_file "main/wifi_manager.c" || ((ERRORS++))
check_file "main/wifi_manager.h" || ((ERRORS++))
check_file "main/ntp_sync.c" || ((ERRORS++))
check_file "main/ntp_sync.h" || ((ERRORS++))
check_file "main/relay_control.c" || ((ERRORS++))
check_file "main/relay_control.h" || ((ERRORS++))
check_file "main/timer_storage.c" || ((ERRORS++))
check_file "main/timer_storage.h" || ((ERRORS++))
check_file "main/https_server.c" || ((ERRORS++))
check_file "main/https_server.h" || ((ERRORS++))
echo ""

echo "Checking SSL Certificates..."
check_file "components/certs/server_cert.pem" || ((ERRORS++))
check_file "components/certs/server_key.pem" || ((ERRORS++))
echo ""

echo "Checking Documentation..."
check_file "README.md" || ((ERRORS++))
check_file "BUILD.md" || ((ERRORS++))
check_file "ARCHITECTURE.md" || ((ERRORS++))
check_file "QUICKREF.md" || ((ERRORS++))
check_file "FAQ.md" || ((ERRORS++))
check_file "CHANGELOG.md" || ((ERRORS++))
check_file "CONTRIBUTING.md" || ((ERRORS++))
check_file "LICENSE" || ((ERRORS++))
check_file "PROJECT_SUMMARY.md" || ((ERRORS++))
check_file "examples/EXAMPLES.md" || ((ERRORS++))
echo ""

echo "Checking Code Statistics..."
C_FILES=$(find main -name "*.c" | wc -l)
H_FILES=$(find main -name "*.h" | wc -l)
TOTAL_LOC=$(find main -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $1}')
DOC_FILES=$(find . -name "*.md" ! -path "./.git/*" | wc -l)

echo "  Source files: ${C_FILES} .c files, ${H_FILES} .h files"
echo "  Lines of code: ${TOTAL_LOC}"
echo "  Documentation: ${DOC_FILES} markdown files"
echo ""

echo "Summary:"
if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}✓ All checks passed! Project is complete.${NC}"
    echo ""
    echo "Next steps:"
    echo "  1. Run 'idf.py menuconfig' to configure WiFi settings"
    echo "  2. Run 'idf.py build' to build the project"
    echo "  3. Run 'idf.py -p PORT flash monitor' to flash and monitor"
    exit 0
else
    echo -e "${RED}✗ Found $ERRORS errors. Please fix them.${NC}"
    exit 1
fi
