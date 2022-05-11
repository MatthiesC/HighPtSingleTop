#include "UHH2/HighPtSingleTop/include/DNNApplication.h"

#include "lwtnn/parse_json.hh"

#include <fstream>

using namespace std;
using namespace uhh2;
using namespace uhh2::btw;

namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
DNNApplication::DNNApplication(Context & ctx, const ERegion_heavyTags & region_heavyTags):
  fHandle_Region_heavyTags(ctx.get_handle<ERegion_heavyTags>(kHandleName_Region_heavyTags)),
  fRegion_heavyTags(region_heavyTags) // region for which DNN has been trained
{
  cout << "Hello World from DNNApplication!" << endl;
  cout << "Region_heavyTags: " << kRegions_heavyTags.at(fRegion_heavyTags).name << endl;

  const string xmlConfig_nnFile = "NeuralNetFile_"+kRegions_heavyTags.at(fRegion_heavyTags).name;
  bApplyDNN = ctx.has(xmlConfig_nnFile) && string2bool(ctx.get("apply_DNNs"));
  if(!bApplyDNN) {
    cout << "Application of DNN switched off or no neural net file given. No outputs will be computed." << endl;
    return;
  }

  const string neural_net_filepath = ctx.get(xmlConfig_nnFile);
  ifstream neural_net_file(neural_net_filepath);
  const lwt::JSONConfig dnn_config = lwt::parse_json(neural_net_file);
  fNeuralNetwork.reset(new lwt::LightweightNeuralNetwork(dnn_config.inputs, dnn_config.layers, dnn_config.outputs));

  cout << "Number of used DNN inputs: " << dnn_config.inputs.size() << endl;
  for(uint i = 0; i < dnn_config.inputs.size(); i++) {
    const lwt::Input & input = dnn_config.inputs.at(i);
    // cout << "input.name: " << input.name << endl;
    fInputNames.insert(input.name);
  }

  if(dnn_config.outputs.size() > 1) {
    cout << "Multi-class DNN with " << dnn_config.outputs.size() << " output nodes" << endl;
  }
  else if(dnn_config.outputs.size() == 1) {
    cout << "Binary DNN with 1 output node" << endl;
  }
  else {
    throw runtime_error("DNNApplication: Number of outputs read-in from DNN json file is non-sensical (<= 0). Please check.");
  }
  for(uint i = 0; i < dnn_config.outputs.size(); i++) {
    const string & output = dnn_config.outputs.at(i);
    // cout << "output: " << output << endl;
    fOutputNames.insert(output);
  }

  // for(uint i = 0; i < fInputNames.size(); i++) {
  for(const string & input_name : fInputNames) {
    fInputHandles[input_name] = ctx.get_handle<float>(input_name);
    // cout << fInputNames.at(i) << endl;
  }
  // for(uint i = 0; i < fOutputNames.size(); i++) {
  for(const string & output_name : fOutputNames) {
    const string handle_name = "dnnOutput__"+kRegions_heavyTags.at(fRegion_heavyTags).name+"__"+output_name;
    cout << "Output branch: " << handle_name << endl;
    fOutputHandles[output_name] = ctx.declare_event_output<float>(handle_name);
    fOutputHandleNames.insert(handle_name);
  }
}

void DNNApplication::set_outputs(Event & event) {
  lwt::ValueMap inputs_map; // std::map<std::string, double>
  // for(uint i = 0; i < fInputNames.size(); i++) {
  for(const string & input_name : fInputNames) {
    if(!event.is_valid(fInputHandles.at(input_name))) throw runtime_error("DNNApplication::set_outputs(): Input handle '"+input_name+"' not valid.");
    inputs_map[input_name] = (double)event.get(fInputHandles.at(input_name));
  }

  const lwt::ValueMap dnn_output_values = fNeuralNetwork->compute(inputs_map);
  // for(uint i = 0; i < fOutputNames.size(); i++) {
  for(const string & output_name : fOutputNames) {
    event.set(fOutputHandles.at(output_name), max(0.f, min(0.99999f, (float)dnn_output_values.at(output_name)))); // avoid outputs of ==1 to not have events filled into overflow bins
  }
}

void DNNApplication::set_dummy_outputs(Event & event) {
  // for(uint i = 0; i < fOutputNames.size(); i++) {
  for(const string & output_name : fOutputNames) {
    event.set(fOutputHandles.at(output_name), fDummyOutputValue);
  }
}

bool DNNApplication::process(Event & event) {
  if(!bApplyDNN) return true;
  const ERegion_heavyTags & region_heavyTags = event.get(fHandle_Region_heavyTags);
  if(region_heavyTags == fRegion_heavyTags) {
    set_outputs(event);
  }
  else {
    set_dummy_outputs(event);
  }
  return true;
}

}}
