#include "config_restart.h"
#include "common_def.h"

#include "esp_log.h"
#include "esp_err.h"

#include <sys/unistd.h>
#include <sys/stat.h>
#include <string.h>

#define TAG "CONFIG_RESTART"

/*esp_err_t cfg_RestartGetString(RestartConfig* theConfig, char* theBuffer)
{
    if( theConfig->m_isTmpCfg ){
        theBuffer[0] = 0;
        return ESP_OK;
    }
    if( theConfig->m_isAP ){
        theBuffer[0] = AP_SYMBOL;
    }
    else{
        theBuffer[0] = ST_SYMBOL;
    }
    if( theConfig->m_isMqtt ){
        theBuffer[1] = MQTT_SYMBOL;
    }
    else {
        theBuffer[1] = HTTP_SYMBOL;
    }
    theBuffer[2] = 0;
    return ESP_OK;
}*/

void CFG_RestartInit(RestartConfig* theConfig)
{
    theConfig->m_isTmpCfg = false;
    theConfig->m_isAP = false;
    theConfig->m_isHttp = false;
}

void CFG_RestartSaveToFile(RestartConfig* theConfig, uint8_t theFlag)
{
    if( theConfig->m_isTmpCfg ){
//If already restart with specific mode - just restart in normal        
        return;
    }
    FILE* aCfgFile = fopen(RESTART_CFG_FILE_PATH, "w");
    if( aCfgFile == NULL ){
        ESP_LOGE(TAG, "Can't open config file %s for write", RESTART_CFG_FILE_PATH);
        return;
    }
    int aWrSize = fwrite(&theFlag, sizeof(char), 1, aCfgFile);
    fflush(aCfgFile);
    fclose(aCfgFile);
    ESP_LOGI(TAG,"Write to restart file done %d, %d", theFlag, aWrSize);
}

void CFG_RestartReadFromFile(RestartConfig* theConfig)
{
    theConfig->m_isTmpCfg = false;
    ESP_LOGI(TAG, "========== Read restart file ===============");
    FILE* aFile = fopen(RESTART_CFG_FILE_PATH, "r");
    if( aFile == NULL ){
        ESP_LOGE(TAG, "Can't open temporary restart config file %s", "/int/tmpres");
        return;
    }
    uint8_t aFlag;
    int aReadSize = fread(&aFlag, sizeof(char), 1, aFile);
    fclose(aFile);
    ESP_LOGI(TAG, "Read from restart=%d, %d", aFlag, aReadSize);
    unlink(RESTART_CFG_FILE_PATH);
    if( aReadSize == 0 ){
        return;
    }

    theConfig->m_isTmpCfg = true;
    if( aFlag & RESTART_AP_FLAG ){
        theConfig->m_isAP = true;
    }
    if( aFlag & RESTART_HTTP_FLAG ){
        theConfig->m_isHttp = true;
    }
}

