#include "upgrade.hpp"

#include "esp_log.h"

using driver::UpgradeHandler;
using data::GincoMessage;

const char * TAG = "Upgrade";

bool UpgradeHandler::init(const GincoMessage& message)
{
	ota_partition_ = esp_ota_get_next_update_partition(NULL);
	if (ota_partition_ == NULL){
		ESP_LOGE(TAG, "Passive OTA partition not found");
		return false;
	}
    image_size_ = message.data;
	if (image_size_ > ota_partition_->size){
		ESP_LOGE(TAG, "Image size too large");
		return false;
	}

	ESP_ERROR_CHECK(esp_ota_begin(ota_partition_, image_size_, &update_handle_));
	ESP_LOGI(TAG, "esp_ota_begin succeeded");
	return true;
};

bool UpgradeHandler::handle(const GincoMessage& message)
{
	image_size_ -=  message.data_length;
	ESP_ERROR_CHECK(esp_ota_write(update_handle_, &message.data, message.data_length));
	if (image_size_ == 0)
	{
		complete();
	}
	return true;
};

void UpgradeHandler::complete(){
	ESP_ERROR_CHECK(esp_ota_end(update_handle_));
	if (!partitionValid())
		return;

	ESP_ERROR_CHECK(esp_ota_set_boot_partition(ota_partition_));
	ESP_LOGI(TAG, "esp_ota_set_boot_partition succeeded");
	esp_restart();
};

bool UpgradeHandler::partitionValid()
{
	esp_app_desc_t new_description;
	const esp_app_desc_t * current_description = esp_app_get_description();
	esp_err_t err = esp_ota_get_partition_description(ota_partition_, &new_description);
	if (err != ESP_OK) {
		ESP_LOGW(TAG, "ota get description returned %d ", err);
		return false;
	}
	if (strcmp(current_description->project_name, new_description.project_name)) {
		ESP_LOGW(TAG, "Invalid image name %s, should be %s", new_description.project_name, current_description->project_name);
		return false;
	}
	return true;
};

void UpgradeHandler::fail(){
	if(update_handle_)
		esp_ota_end(update_handle_);
};
