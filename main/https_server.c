#include <string.h>
#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server = NULL;

// HTML page with form for timer settings
static const char* html_page = 
"<! DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='UTF-8'>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"<title>ESP8266 Timer Control</title>"
"<style>"
"body { font-family:  Arial, sans-serif; max-width: 600px; margin: 50px auto; padding: 20px; background-color: #f0f0f0; }"
"h1 { color: #333; text-align: center; }"
". container { background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
". form-group { margin-bottom:  20px; }"
"label { display: block; margin-bottom: 5px; font-weight: bold; color: #555; }"
"input[type='time'] { width: 100%; padding:  10px; border: 1px solid #ddd; border-radius: 5px; font-size: 16px; }"
"button { width: 100%; padding:  12px; background-color: #4CAF50; color: white; border: none; border-radius:  5px; font-size:  16px; cursor: pointer; }"
"button:hover { background-color: #45a049; }"
". status { margin-top: 20px; padding: 15px; background-color: #e7f3fe; border-left: 4px solid #2196F3; border-radius: 5px; }"
".info { font-size: 14px; color: #666; margin-top: 10px; }"
"</style>"
"</head>"
"<body>"
"<div class='container'>"
"<h1>⏱️ ESP8266 Timer Control</h1>"
"<form id='timerForm'>"
"<div class='form-group'>"
"<label for='startTime'>Start Time: </label>"
"<input type='time' id='startTime' name='startTime' required>"
"</div>"
"<div class='form-group'>"
"<label for='stopTime'>Stop Time:</label>"
"<input type='time' id='stopTime' name='stopTime' required>"
"</div>"
"<button type='submit'>Set Timer</button>"
"</form>"
"<div class='status' id='status' style='display: none;'></div>"
"<div class='info'>Current time will be synced via NTP</div>"
"</div>"
"<script>"
"document.getElementById('timerForm').addEventListener('submit', function(e) {"
"  e.preventDefault();"
"  var startTime = document.getElementById('startTime').value;"
"  var stopTime = document.getElementById('stopTime').value;"
"  fetch('/api/timer', {"
"    method: 'POST',"
"    headers: { 'Content-Type': 'application/json' },"
"    body: JSON. stringify({ start: startTime, stop: stopTime })"
"  })"
"  .then(response => response.json())"
"  .then(data => {"
"    var status = document.getElementById('status');"
"    status.style.display = 'block';"
"    status.innerHTML = '✅ Timer set successfully! <br>Start:  ' + startTime + '<br>Stop: ' + stopTime;"
"    status.style.backgroundColor = '#d4edda';"
"    status.style.borderColor = '#28a745';"
"  })"
"  .catch(error => {"
"    var status = document.getElementById('status');"
"    status.style.display = 'block';"
"    status.innerHTML = '❌ Error setting timer: ' + error;"
"    status.style.backgroundColor = '#f8d7da';"
"    status.style. borderColor = '#dc3545';"
"  });"
"});"
"</script>"
"</body>"
"</html>";

// Root handler - serve HTML page
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, html_page, strlen(html_page));
    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

// Status API handler
static esp_err_t status_get_handler(httpd_req_t *req)
{
    const char* resp_str = "{\"status\": \"ok\",\"device\": \"esp8266_timer\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

static const httpd_uri_t status = {
    .uri       = "/status",
    .method    = HTTP_GET,
    .handler   = status_get_handler,
    .user_ctx  = NULL
};

// Timer API POST handler
static esp_err_t timer_post_handler(httpd_req_t *req)
{
    char content[200];
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    content[ret] = '\0';

    ESP_LOGI(TAG, "Received timer data: %s", content);

    // Parse JSON
    cJSON *json = cJSON_Parse(content);
    if (json == NULL) {
        const char* error_resp = "{\"error\": \"Invalid JSON\"}";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, error_resp, strlen(error_resp));
        return ESP_FAIL;
    }

    cJSON *start = cJSON_GetObjectItem(json, "start");
    cJSON *stop = cJSON_GetObjectItem(json, "stop");

    if (cJSON_IsString(start) && cJSON_IsString(stop)) {
        ESP_LOGI(TAG, "Start time: %s, Stop time:  %s", start->valuestring, stop->valuestring);
        
        // TODO: Save to NVS and apply timer settings
        // Call your timer_storage functions here
        
        const char* success_resp = "{\"status\":  \"success\", \"message\": \"Timer set\"}";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, success_resp, strlen(success_resp));
    } else {
        const char* error_resp = "{\"error\": \"Missing start or stop time\"}";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, error_resp, strlen(error_resp));
    }

    cJSON_Delete(json);
    return ESP_OK;
}

static const httpd_uri_t timer_post = {
    .uri       = "/api/timer",
    .method    = HTTP_POST,
    .handler   = timer_post_handler,
    .user_ctx  = NULL
};

esp_err_t start_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.max_uri_handlers = 8;
    config.stack_size = 8192;  // Increased for JSON parsing

    ESP_LOGI(TAG, "Starting HTTP server on port: '%d'", config.server_port);
    
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &status);
        httpd_register_uri_handler(server, &timer_post);
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Error starting HTTP server!");
    return ESP_FAIL;
}

void stop_http_server(void)
{
    if (server) {
        httpd_stop(server);
        server = NULL;
    }
}