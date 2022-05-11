#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/DNNInputs.h"
#include "UHH2/HighPtSingleTop/include/DNNApplication.h"

namespace uhh2 { namespace btw {

//____________________________________________________________________________________________________
class DNNHists: public uhh2::Hists {
public:
  DNNHists(uhh2::Context & ctx, const std::string & dirname, const std::unique_ptr<btw::DNNInputs> & module_DNNInputs, const std::unique_ptr<btw::DNNApplication> & module_DNNApplication, const unsigned int default_nbins = 100);
  virtual void fill(const uhh2::Event & event) override;

private:
  const bool fApply;
  std::map<std::string, uhh2::Event::Handle<float>> fOutputHandles;
  std::map<std::string, uhh2::Event::Handle<float>> fInputHandles;

protected:
  std::map<std::string, TH1F*> fOutputHists_all;
  std::map<std::string, TH1F*> fOutputHists_max;
  std::map<std::string, TH1F*> fInputHists;
};

}}
