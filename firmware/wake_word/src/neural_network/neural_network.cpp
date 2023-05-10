#include "neural_network.h"

#include <HardwareSerial.h>

extern unsigned char __model[];
extern unsigned int __model_len;

neural_network::neural_network()
{
  m_error_reporter = std::make_unique<tflite::MicroErrorReporter>();
  m_tensor_arena = std::make_unique<tflite_tensor_arena_arr>();
  m_model = const_cast<tflite_model_ptr>(tflite::GetModel(__model));
  m_resolver = std::make_unique<tflite::MicroMutableOpResolver<10>>();
  add_ops();
  m_interpreter = std::make_unique<tflite::MicroInterpreter>(m_model, *m_resolver, m_tensor_arena->data(), TENSOR_ARENA_SIZE, m_error_reporter.get());
  m_interpreter->AllocateTensors();
  m_input = m_interpreter->input(0);
  m_output = m_interpreter->output(0);

  // auto used_bytes = m_interpreter->arena_used_bytes();
  // Serial.printf("Used bytes: %zu\n", used_bytes);
  // Serial.printf("Output size %d [%d %d]\n", m_interpreter->output_tensor(0)->dims->size, m_interpreter->output_tensor(0)->dims->data[0], m_interpreter->output_tensor(0)->dims->data[1]);
}

float* neural_network::get_input_buffer()
{
  return m_input->data.f;
}

float* neural_network::get_output_buffer()
{
  return m_output->data.f;
}

void neural_network::predict()
{
  m_interpreter->Invoke();
}

void neural_network::add_ops()
{
  m_resolver->AddConv2D();
  m_resolver->AddMaxPool2D();
  m_resolver->AddFullyConnected();
  m_resolver->AddMul();
  m_resolver->AddAdd();
  m_resolver->AddLogistic();
  m_resolver->AddReshape();
  m_resolver->AddQuantize();
  m_resolver->AddDequantize();
  m_resolver->AddSoftmax();
}
