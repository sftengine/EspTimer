#include <string.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_http_server.h"
#include "esp_https_server.h"
#include "cJSON.h"

#include "https_server.h"
#include "relay_control.h"
#include "timer_storage.h"
#include "ntp_sync.h"

static const char *TAG = "https_server";
static httpd_handle_t server = NULL;

// External references to embedded certificate and key
extern const unsigned char server_cert_pem_start[] asm("_binary_server_cert_pem_start");
extern const unsigned char server_cert_pem_end[]   asm("_binary_server_cert_pem_end");
extern const unsigned char server_key_pem_start[]  asm("_binary_server_key_pem_start");
extern const unsigned char server_key_pem_end[]    asm("_binary_server_key_pem_end");

// HTML page with embedded CSS
static const char *html_page = 
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='UTF-8'>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"<title>ESP8266 Web Timer</title>"
"<style>"
"body{font-family:Arial,sans-serif;max-width:600px;margin:50px auto;padding:20px;background:#f0f0f0;}"
"h1{color:#333;text-align:center;margin-bottom:30px;}"
".container{background:white;padding:30px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}"
".time-display{font-size:24px;text-align:center;margin-bottom:30px;padding:15px;background:#f8f8f8;border-radius:5px;}"
".form-group{margin-bottom:20px;}"
"label{display:block;margin-bottom:5px;font-weight:bold;color:#555;}"
".time-input{display:flex;gap:10px;align-items:center;}"
".time-input input{width:80px;padding:10px;font-size:16px;border:1px solid #ddd;border-radius:5px;}"
".time-input span{font-size:20px;color:#333;}"
"button{width:100%;padding:15px;font-size:16px;background:#4CAF50;color:white;border:none;border-radius:5px;cursor:pointer;}"
"button:hover{background:#45a049;}"
".status{margin-top:30px;padding:15px;border-radius:5px;text-align:center;font-size:18px;}"
".status-on{background:#d4edda;color:#155724;border:1px solid #c3e6cb;}"
".status-off{background:#f8d7da;color:#721c24;border:1px solid #f5c6cb;}"
".indicator{display:inline-block;width:15px;height:15px;border-radius:50%;margin-right:10px;}"
".indicator-on{background:#28a745;}"
".indicator-off{background:#dc3545;}"
"</style>"
"</head>"
"<body>"
"<h1>ESP8266 Web Timer</h1>"
"<div class='container'>"
"<div class='time-display'>Current Time: <span id='current-time'>--:--:--</span></div>"
"<form id='timer-form'>"
"<div class='form-group'>"
"<label>ON Time</label>"
"<div class='time-input'>"
"<input type='number' id='on-hour' min='0' max='23' placeholder='HH' required>"
"<span>:</span>"
"<input type='number' id='on-min' min='0' max='59' placeholder='MM' required>"
"</div>"
"</div>"
"<div class='form-group'>"
"<label>OFF Time</label>"
"<div class='time-input'>"
"<input type='number' id='off-hour' min='0' max='23' placeholder='HH' required>"
"<span>:</span>"
"<input type='number' id='off-min' min='0' max='59' placeholder='MM' required>"
"</div>"
"</div>"
"<button type='submit'>Save Timer</button>"
"</form>"
"<div id='status' class='status'></div>"
"</div>"
"<script>"
"function updateStatus(){"
"fetch('/api/status')"
".then(r=>r.json())"
".then(d=>{"
"document.getElementById('current-time').textContent=d.time;"
"const s=document.getElementById('status');"
"const isOn=d.relay_on;"
"s.className='status '+(isOn?'status-on':'status-off');"
"s.innerHTML='<span class=\"indicator '+(isOn?'indicator-on':'indicator-off')+'\"></span>Relay: '+(isOn?'ON':'OFF');"
"document.getElementById('on-hour').value=d.on_hour;"
"document.getElementById('on-min').value=d.on_min;"
"document.getElementById('off-hour').value=d.off_hour;"
"document.getElementById('off-min').value=d.off_min;"
"})"
".catch(e=>{console.error('Status update error:',e);});"
"}"
"document.getElementById('timer-form').addEventListener('submit',function(e){"
"e.preventDefault();"
"const data={"
"on_hour:parseInt(document.getElementById('on-hour').value),"
"on_min:parseInt(document.getElementById('on-min').value),"
"off_hour:parseInt(document.getElementById('off-hour').value),"
"off_min:parseInt(document.getElementById('off-min').value)"
"};"
"fetch('/api/timer',{"
"method:'POST',"
"headers:{'Content-Type':'application/json'},"
"body:JSON.stringify(data)"
"})"
".then(r=>r.json())"
".then(d=>{"
"alert(d.message);"
"updateStatus();"
"})"
".catch(e=>{console.error('Save error:',e);alert('Error saving timer. Please try again.');});"
"});"
"updateStatus();"
"setInterval(updateStatus,5000);"
"</script>"
"</body>"
"</html>";

/* Handler for root URI */
static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Handler for status API */
static esp_err_t status_handler(httpd_req_t *req)
{
    char time_str[64];
    uint8_t on_hour, on_min, off_hour, off_min;
    
    get_current_time_str(time_str, sizeof(time_str));
    timer_load_settings(&on_hour, &on_min, &off_hour, &off_min);
    
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "time", time_str);
    cJSON_AddBoolToObject(root, "relay_on", relay_get_state());
    cJSON_AddNumberToObject(root, "on_hour", on_hour);
    cJSON_AddNumberToObject(root, "on_min", on_min);
    cJSON_AddNumberToObject(root, "off_hour", off_hour);
    cJSON_AddNumberToObject(root, "off_min", off_min);
    
    const char *json_str = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));
    
    free((void *)json_str);
    cJSON_Delete(root);
    
    return ESP_OK;
}

/* Handler for timer API */
static esp_err_t timer_handler(httpd_req_t *req)
{
    char content[200];
    int ret, remaining = req->content_len;

    if (remaining >= sizeof(content)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content too long");
        return ESP_FAIL;
    }

    ret = httpd_req_recv(req, content, remaining);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    content[ret] = '\0';

    cJSON *root = cJSON_Parse(content);
    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    cJSON *on_hour_json = cJSON_GetObjectItem(root, "on_hour");
    cJSON *on_min_json = cJSON_GetObjectItem(root, "on_min");
    cJSON *off_hour_json = cJSON_GetObjectItem(root, "off_hour");
    cJSON *off_min_json = cJSON_GetObjectItem(root, "off_min");

    if (!cJSON_IsNumber(on_hour_json) || !cJSON_IsNumber(on_min_json) ||
        !cJSON_IsNumber(off_hour_json) || !cJSON_IsNumber(off_min_json)) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing or invalid fields");
        return ESP_FAIL;
    }

    // Validate time ranges before casting
    int on_hour_int = on_hour_json->valueint;
    int on_min_int = on_min_json->valueint;
    int off_hour_int = off_hour_json->valueint;
    int off_min_int = off_min_json->valueint;

    if (on_hour_int < 0 || on_hour_int > 23 || on_min_int < 0 || on_min_int > 59 ||
        off_hour_int < 0 || off_hour_int > 23 || off_min_int < 0 || off_min_int > 59) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Time values out of range (hour: 0-23, minute: 0-59)");
        return ESP_FAIL;
    }

    uint8_t on_hour = (uint8_t)on_hour_int;
    uint8_t on_min = (uint8_t)on_min_int;
    uint8_t off_hour = (uint8_t)off_hour_int;
    uint8_t off_min = (uint8_t)off_min_int;

    cJSON_Delete(root);

    esp_err_t err = timer_save_settings(on_hour, on_min, off_hour, off_min);
    
    cJSON *response = cJSON_CreateObject();
    if (err == ESP_OK) {
        cJSON_AddStringToObject(response, "message", "Timer settings saved successfully");
        cJSON_AddBoolToObject(response, "success", true);
    } else {
        cJSON_AddStringToObject(response, "message", "Failed to save timer settings");
        cJSON_AddBoolToObject(response, "success", false);
    }

    const char *json_str = cJSON_Print(response);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));
    
    free((void *)json_str);
    cJSON_Delete(response);

    return ESP_OK;
}

static const httpd_uri_t root_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t status_uri = {
    .uri       = "/api/status",
    .method    = HTTP_GET,
    .handler   = status_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t timer_uri = {
    .uri       = "/api/timer",
    .method    = HTTP_POST,
    .handler   = timer_handler,
    .user_ctx  = NULL
};

esp_err_t start_https_server(void)
{
    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
    conf.port_secure = CONFIG_HTTPS_SERVER_PORT;
    
    conf.cacert_pem = server_cert_pem_start;
    conf.cacert_len = server_cert_pem_end - server_cert_pem_start;
    
    conf.prvtkey_pem = server_key_pem_start;
    conf.prvtkey_len = server_key_pem_end - server_key_pem_start;

    ESP_LOGI(TAG, "Starting HTTPS server on port %d", conf.port_secure);
    
    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTPS server: %s", esp_err_to_name(ret));
        return ret;
    }

    httpd_register_uri_handler(server, &root_uri);
    httpd_register_uri_handler(server, &status_uri);
    httpd_register_uri_handler(server, &timer_uri);

    ESP_LOGI(TAG, "HTTPS server started successfully");
    return ESP_OK;
}

void stop_https_server(void)
{
    if (server) {
        httpd_ssl_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "HTTPS server stopped");
    }
}
