idf_component_register(
    SRCS 
        "main.c"
        "wifi_manager.c"
        "ntp_sync.c"
        "relay_control.c"
        "timer_storage.c"
    INCLUDE_DIRS "."
    EMBED_FILES
        "${CMAKE_SOURCE_DIR}/components/certs/server_cert.pem"
        "${CMAKE_SOURCE_DIR}/components/certs/server_key.pem"
)
