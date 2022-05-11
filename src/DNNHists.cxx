#include "UHH2/HighPtSingleTop/include/DNNHists.h"

#include "UHH2/common/include/Utils.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;
using namespace uhh2::btw;

namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
// DNNHists::DNNHists(Context & ctx, const string & dirname, const vector<string> & arg_used_inputs, const vector<DNNInput> & arg_inputs_info, const vector<string> arg_output_names):
DNNHists::DNNHists(Context & ctx, const string & dirname, const unique_ptr<btw::DNNInputs> & module_DNNInputs, const unique_ptr<btw::DNNApplication> & module_DNNApplication, const unsigned int default_nbins):
  Hists(ctx, dirname),
  fApply(module_DNNApplication->apply())
{
  const auto & outputs = module_DNNApplication->get_output_names();

  if(fApply) {
    for(const auto & output : outputs) {
      fOutputHandles[output] = ctx.get_handle<float>(output);
      fOutputHists_all[output] = book<TH1F>((output+"__all").c_str(), (output+"__all").c_str(), default_nbins, 0, 1);
      fOutputHists_max[output] = book<TH1F>((output+"__max").c_str(), (output+"__max").c_str(), default_nbins, 0, 1);
    }
  }

  const auto & prepared_inputs = module_DNNInputs->get_inputs_map();
  const auto & used_inputs = module_DNNApplication->get_input_names();

  for(const auto & input : prepared_inputs) {
    fInputHandles[input.second.name] = ctx.get_handle<float>(input.second.name);
    const bool used = used_inputs.find(input.second.name) != used_inputs.end();
    const string label = (used ? "" : "[UNUSED] ")+input.second.plot_label;
    fInputHists[input.second.name] = book<TH1F>(input.second.name.c_str(), label.c_str(), input.second.n_bins, input.second.x_low, input.second.x_high);
  }
}

void DNNHists::fill(const Event & event) {
  const float w = event.weight;

  if(fApply) {
    map<string, float> output_values;
    for(const auto & hist : fOutputHists_all) {
      const float value = event.get(fOutputHandles.at(hist.first));
      output_values[hist.first] = value;
      hist.second->Fill(value, w);
    }
    float max_value = -999.f;
    string output_with_max_value;
    for(const auto & value : output_values) {
      if(value.second > max_value) {
        max_value = value.second;
        output_with_max_value = value.first;
      }
    }
    fOutputHists_max[output_with_max_value]->Fill(max_value, w);
  }

  for(const auto & hist : fInputHists) {
    hist.second->Fill(event.get(fInputHandles.at(hist.first)), w);
  }
}

}}
