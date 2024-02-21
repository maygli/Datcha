#pragma once

#include <esp_err.h>
#include <esp_http_server.h>

#ifdef __cplusplus
extern "C" {
#endif
#ifdef MQTT_ENABLED
esp_err_t HTTP_SetMqttSettings(httpd_req_t *req);
esp_err_t HTTP_GetMqttSettings(httpd_req_t *req);
#endif

#ifdef __cplusplus
}
#endif
