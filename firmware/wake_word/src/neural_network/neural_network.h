#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "config.h"

// Arduino.h defines DEFAULT and it's used in some enum inside tflite
#define DEFAULT_OLD DEFAULT
#undef DEFAULT
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#define DEFAULT DEFAULT_OLD

#include <memory>

using tflite_resolver_ptr = std::unique_ptr<tflite::MicroMutableOpResolver<10>>;
using tflite_interpreter_ptr = std::unique_ptr<tflite::MicroInterpreter>;
using tflite_error_reporter_ptr = std::unique_ptr<tflite::ErrorReporter>;
template <typename T, size_t N> struct alignas(16) aligned_array : public std::array<T,N> {};
using tflite_tensor_arena_arr = aligned_array<uint8_t, TENSOR_ARENA_SIZE>;
using tflite_tensor_arena_ptr = std::unique_ptr<tflite_tensor_arena_arr>;
using tflite_model_ptr = tflite::Model*;
using tflite_tensor_ptr = TfLiteTensor*;

struct neural_network
{
  neural_network();
  float* get_input_buffer();
  float* get_output_buffer();
  void predict();
  void add_ops();

  tflite_error_reporter_ptr m_error_reporter;
  tflite_tensor_arena_ptr m_tensor_arena;
  tflite_interpreter_ptr m_interpreter;
  tflite_resolver_ptr m_resolver;
  tflite_model_ptr m_model;
  tflite_tensor_ptr m_input;
  tflite_tensor_ptr m_output;
};

using neural_network_ptr = std::unique_ptr<neural_network>;

#endif
