#ifndef CONFIG_H
#define CONFIG_H

#include <sys/_stdint.h>
#include <stddef.h>
#include <math.h>

// todo: determine the value empirically
const unsigned long SERIAL_CONNECTION_TIMEOUT = 1000;

// Audio sample rate for the whole project
const uint32_t SAMPLE_RATE = 16000;

// Audio input pins
// todo: test and select better fitting pins
const uint8_t AUDIO_IN_SCK_PIN = 4;
const uint8_t AUDIO_IN_WS_PIN = 5;
const uint8_t AUDIO_IN_DATA_PIN = 18;

// Audio output pins
// todo: set these pins
const uint8_t AUDIO_OUT_SCK_PIN = 1;
const uint8_t AUDIO_OUT_WS_PIN = 2;
const uint8_t AUDIO_OUT_DATA_PIN = 15;

// Ring buffer settings
const size_t AUDIO_BUFFER_LENGTH = SAMPLE_RATE * 1; // 1s
const size_t NO_I2S_BUFFERS = 11;
const size_t RING_BUFFER_TOTAL_SIZE = AUDIO_BUFFER_LENGTH * NO_I2S_BUFFERS;

// STFT settings
const size_t WINDOW_SIZE = 320;
const size_t STEP_SIZE = 160;
const size_t POOLING_SIZE = 6;
const size_t FFT_SIZE = 512;
const size_t ENERGY_SIZE = FFT_SIZE / 2 + 1;
const size_t ENERGY_POOLED_SIZE = (size_t) ceil((float)ENERGY_SIZE / (float)POOLING_SIZE);

// Neural network settings
// todo: create my own tool for calculating area size
const size_t TENSOR_ARENA_SIZE = 297184; // calculated by https://github.com/edgeimpulse/tflite-find-arena-size
// todo: retrain network and change to 2 or even 1 (binary crossentropy should be better (would be able to adjust threshold) and faster (smaller network))
const size_t NO_COMMANDS = 3;
const size_t TARGET_WORD_IDX = 0;
const unsigned long MIN_TIME_BETWEEN_DETECTIONS = 1000; // ms

// Audio processor settings
const float EPSILON_F = 1e-6f;

// Google API settings
const unsigned long TOKEN_LIFETIME = 60 * 60; // seconds
const size_t SHA256_HASH_LEN = 32;
const size_t SIGNATURE_LEN = 256;

// Speech to Text settings
const size_t MAX_RECORDING_LENGTH = SAMPLE_RATE * 10; // 10s
const int16_t SILENCE_THRESHOLD = 327; // 0.01f * 32767
const size_t SILENCE_CUT_RECORDING_LENGTH = SAMPLE_RATE * 2; // 1s
const int16_t NOISE_TO_VOICE_RATIO = 7; // todo: empirically set the best value

// SSL timeout settings
const unsigned long RECV_RESPONSE_TIMEOUT = 10000;
const unsigned long CHATGPT_RECV_RESPONSE_TIMEOUT = 120000; // todo: maybe even longer?

// Speech synthesis settings
const float SPEECH_SYNTHESIS_PITCH = 0.9f;
const float SPEECH_SYNTHESIS_SPEED = 1.1f;
const float SPEECH_SYNTHESIS_VOLUME = 1.f;
const float SPEECH_SYNTHESIS_GAIN = 0.3f;
const int SPEECH_SYNTHESIS_CONTRAST = 0;

// LED settings
const size_t NO_LEDS = 1;
const uint8_t LED_PIN = 48;
const size_t NO_INIT_STEPS = 12;


#endif
