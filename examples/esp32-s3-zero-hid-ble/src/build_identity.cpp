#include "build_identity.h"

#include <Arduino.h>
#include "esp_app_desc.h"

void printBuildIdentity() {
  const esp_app_desc_t* desc = esp_app_get_description();

  char sha[65];
  for (int i = 0; i < 32; ++i) {
    sprintf(sha + (i * 2), "%02x", desc->app_elf_sha256[i]);
  }
  sha[64] = '\0';

  Serial.printf("[build] project=%s version=%s idf=%s\n",
                desc->project_name, desc->version, desc->idf_ver);
  Serial.printf("[build] app_elf_sha256=%s\n", sha);
}
