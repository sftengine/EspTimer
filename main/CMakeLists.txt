idf_component_register(
    SRCS 
        "main.c"
        "wifi_manager.c"
        "ntp_sync.c"
        "relay_control.c"
        "https_server.c"
        "timer_storage.c"
    INCLUDE_DIRS "."
    REQUIRES 
        esp_http_server esp_http_client nvs_flash pthread json
    EMBED_FILES
        "${CMAKE_SOURCE_DIR}/components/certs/server_cert.pem"
        "${CMAKE_SOURCE_DIR}/components/certs/server_key.pem"
)
