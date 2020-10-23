#include "UHH2/HighPtSingleTop/include/DNNApplication.h"

#include <fstream>

using namespace std;
using namespace uhh2;


DNNApplication::DNNApplication(Context & ctx, const string & object_name) {

  cout << "Hello World from DNNApplication!" << endl;
  cout << "Object: " << object_name << endl;

  string config_file_string = "NeuralNetFile_";
  if (object_name == "Top") { config_file_string += "tTag"; }
  else if(object_name == "W") { config_file_string += "WTag"; }
  else throw invalid_argument("DNNApplication: Please choose 'Top' or 'W' as object_name argument.");
  string neural_net_filepath = ctx.get(config_file_string);

  ifstream neural_net_file(neural_net_filepath);
  auto dnn_config = lwt::parse_json(neural_net_file);
  m_NeuralNetwork.reset(new lwt::LightweightNeuralNetwork(dnn_config.inputs, dnn_config.layers, dnn_config.outputs));

  cout << "Number of used DNN inputs: " << dnn_config.inputs.size() << endl;
  for(uint i = 0; i < dnn_config.inputs.size(); i++) {
    const auto & input = dnn_config.inputs.at(i);
    // cout << "input.name: " << input.name << endl;
    m_dnn_config_inputNames.push_back(input.name);
  }

  if(dnn_config.outputs.size() > 1) {
    cout << "Multi-class DNN with " << dnn_config.outputs.size() << " output nodes" << endl;
  }
  else if(dnn_config.outputs.size() == 1) {
    cout << "Binary DNN with 1 output node" << endl;
  }
  else {
    throw runtime_error("DNNApplication: Number of outputs read-in from DNN json file non-sensical (<= 0). Please check.");
  }
  for(uint i = 0; i < dnn_config.outputs.size(); i++) {
    const auto & output = dnn_config.outputs.at(i);
    // cout << "output: " << output << endl;
    m_dnn_config_outputNames.push_back(output);
  }

  for(uint i = 0; i < m_dnn_config_inputNames.size(); i++) {
    m_h_inputs.push_back(ctx.get_handle<double>(m_dnn_config_inputNames.at(i)));
    cout << m_dnn_config_inputNames.at(i) << endl;
  }
  for(uint i = 0; i < m_dnn_config_outputNames.size(); i++) {
    string handle_name = "DNNoutput_";
    if(object_name == "Top") { handle_name += "ttag__"; }
    else if(object_name == "W") { handle_name += "wtag__"; }
    handle_name += m_dnn_config_outputNames.at(i);
    cout << handle_name << endl;
    m_h_outputs.push_back(ctx.declare_event_output<double>(handle_name));
    m_output_names.push_back(handle_name);
  }
}

bool DNNApplication::process(Event & event) {

  map<string, double> inputs_map;
  for(uint i = 0; i < m_dnn_config_inputNames.size(); i++) {
    inputs_map[m_dnn_config_inputNames.at(i)] = (double)event.get(m_h_inputs.at(i));
  }

  auto dnn_output_values = m_NeuralNetwork->compute(inputs_map);
  for(uint i = 0; i < m_dnn_config_outputNames.size(); i++) {
    event.set(m_h_outputs.at(i), (double)dnn_output_values[m_dnn_config_outputNames.at(i)]);
  }

  return true;
}

bool DNNApplication::process_dummy(Event & event) {

  for(uint i = 0; i < m_dnn_config_outputNames.size(); i++) {
    event.set(m_h_outputs.at(i), (double)(-1.));
  }

  return true;
}
