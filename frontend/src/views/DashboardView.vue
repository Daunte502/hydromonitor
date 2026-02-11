<template>
  <v-container fluid>
    <v-row justify="center" align="center">
      <v-col cols="12" md="4" align="center">
        <v-card class="pa-4">
          <v-card-title>Temperature</v-card-title>
          <v-card-text>
            <v-progress-circular
              :model-value="temperaturePercentage"
              :color="temperatureColor"
              size="150"
              width="10"
            >
              {{ payload.temperature }}°C
            </v-progress-circular>
          </v-card-text>
        </v-card>
      </v-col>
      <v-col cols="12" md="4" align="center">
        <v-card class="pa-4">
          <v-card-title>Humidity</v-card-title>
          <v-card-text>
            <v-progress-circular
              :model-value="humidityPercentage"
              :color="humidityColor"
              size="150"
              width="10"
            >
              {{ payload.humidity }}%
            </v-progress-circular>
          </v-card-text>
        </v-card>
      </v-col>
      <v-col cols="12" md="4" align="center">
        <v-card class="pa-4">
          <v-card-title>Heat Index</v-card-title>
          <v-card-text>
            <v-progress-circular
              :model-value="heatIndexPercentage"
              :color="heatIndexColor"
              size="150"
              width="10"
            >
              {{ payload.heatindex }}°C
            </v-progress-circular>
          </v-card-text>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
import { computed, onMounted, onBeforeUnmount } from 'vue';
import { storeToRefs } from 'pinia';
import { useMqttStore } from '@/store/mqttStore';

const mqttStore = useMqttStore();
const { payload } = storeToRefs(mqttStore);

// Computed properties for gauge percentages and colors
const temperaturePercentage = computed(() => {
  // Assuming temperature range 0-50°C for 0-100%
  return Math.min((payload.value.temperature / 50) * 100, 100);
});

const humidityPercentage = computed(() => {
  // Humidity is already 0-100%
  return payload.value.humidity;
});

const heatIndexPercentage = computed(() => {
  // Assuming heat index range 0-50°C for 0-100%
  return Math.min((payload.value.heatindex / 50) * 100, 100);
});

const temperatureColor = computed(() => {
  const temp = payload.value.temperature;
  if (temp < 20) return 'blue';
  if (temp < 30) return 'green';
  return 'red';
});

const humidityColor = computed(() => {
  const hum = payload.value.humidity;
  if (hum < 30) return 'red';
  if (hum < 60) return 'yellow';
  return 'green';
});

const heatIndexColor = computed(() => {
  const hi = payload.value.heatindex;
  if (hi < 25) return 'blue';
  if (hi < 35) return 'orange';
  return 'red';
});

onMounted(() => {
  // Subscribe to the pub topic to receive ESP32 data
  mqttStore.subscribe('620170239_pub');
});

onBeforeUnmount(() => {
  // Unsubscribe to save on network traffic
  mqttStore.unsubcribe('620170239_pub');
});
</script>
