#ifndef CONFIG_H
#define CONFIG_H

#include <sys/_stdint.h>
#include <stddef.h>
#include <math.h>

const uint32_t SAMPLE_RATE = 16000;

// todo: test and select better fitting pins
const uint8_t I2S_SCK_PIN = 4;
const uint8_t I2S_WS_PIN = 5;
const uint8_t I2S_SD_PIN = 18;

const size_t AUDIO_BUFFER_LENGTH = 16000;
const size_t NO_I2S_BUFFERS = 11;
const size_t RING_BUFFER_TOTAL_SIZE = AUDIO_BUFFER_LENGTH * NO_I2S_BUFFERS;

const size_t WINDOW_SIZE = 320;
const size_t STEP_SIZE = 160;
const size_t POOLING_SIZE = 6;
const size_t FFT_SIZE = 512;
const size_t ENERGY_SIZE = FFT_SIZE / 2 + 1;
const size_t ENERGY_POOLED_SIZE = (size_t) ceil((float)ENERGY_SIZE / (float)POOLING_SIZE);

const size_t TENSOR_ARENA_SIZE = 77776; // calculated by some tool // todo: find it's name and check how it works

const size_t NO_COMMANDS = 3; // todo: retrain network and change to 2
const size_t TARGET_WORD_IDX = 0;
const unsigned long MIN_TIME_BETWEEN_DETECTIONS = 1000; // ms

const float EPSILON_F = 1e-6f;

const unsigned long TOKEN_LIFETIME = 60 * 60; // seconds
const size_t SHA256_HASH_LEN = 32;
const size_t SIGNATURE_LEN = 256;

const size_t MAX_RECORDING_LENGTH = SAMPLE_RATE * 10; // 10s
const uint16_t SILENCE_THRESHOLD = 6553; // 0.1f * 65535
const size_t SILENCE_CUT_RECORDING_LENGTH = SAMPLE_RATE; // 1s

#endif
