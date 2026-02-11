<template>
  <v-container fluid>
    <v-row justify="center" align="center" style="max-width: 1200px;">
      <v-col align="center">
        <v-sheet class="pa-4 mb-4">
          <h3>LED Controls</h3>
        </v-sheet>
        <v-sheet class="pa-4 mb-4">
          <v-slider
            v-model="led.brightness"
            label="Brightness"
            min="0"
            max="250"
            step="10"
            thumb-label="always"
          ></v-slider>
        </v-sheet>
        <v-sheet class="pa-4 mb-4">
          <v-slider
            v-model="led.leds"
            label="LED Nodes"
            min="1"
            max="7"
            step="1"
            thumb-label="always"
          ></v-slider>
        </v-sheet>
        <v-sheet class="pa-4">
          <v-progress-circular
            :model-value="led.leds * 15"
            :color="indicatorColor"
            size="100"
          ></v-progress-circular>
        </v-sheet>
      </v-col>
      <v-col align="center">
        <v-color-picker
          v-model="led.color"
          mode="rgb"
        ></v-color-picker>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
  import { storeToRefs } from 'pinia';
  import { useMqttStore } from '@/store/mqttStore';
  import { ref, reactive, watch, computed, onMounted, onBeforeUnmount } from 'vue';

  // Initialize reactive object for LED controls
  const led = reactive({
    brightness: 0,
    leds: 1,
    color: { r: 255, g: 255, b: 255 } // Initialize as RGB object
  });

  // Timer for debouncing MQTT publishes
  const publishTimer = ref(null);

  // Computed property for ProgressCircular color (convert RGB object to CSS color string)
  const indicatorColor = computed(() => `rgb(${led.color.r}, ${led.color.g}, ${led.color.b})`);

  // MQTT Store
  const mqttStore = useMqttStore();

  // Watcher for led object changes with debouncing
  watch(
    led,
    (newLed) => {
      if (publishTimer.value) {
        clearTimeout(publishTimer.value);
      }
      publishTimer.value = setTimeout(() => {
        // Publish JSON message with "type": "controls" to hardware topic (replace YOUR_STUDENT_ID with actual ID, e.g., '620012345_sub')
        mqttStore.publish('620170239_sub', JSON.stringify({ type: "controls", ...newLed }));
      }, 1000); // 1000ms debounce delay as per lab manual
    },
    { deep: true }
  );

  onMounted(() => {
    // Any initialization if needed
  });

  onBeforeUnmount(() => {
    // Clear timer on unmount
    if (publishTimer.value) {
      clearTimeout(publishTimer.value);
    }
  });
</script>
```