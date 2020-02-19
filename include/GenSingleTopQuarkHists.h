#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"


class GenSingleTopQuarkHists: public uhh2::Hists {

 public:

  GenSingleTopQuarkHists(uhh2::Context & ctx, const std::string & dirname);
  virtual void fill(const uhh2::Event & event) override;

 protected:

  TH1F *hist_topQuarkPt;
  TH1F *hist_topQuarkPt_ggFusion;
  TH1F *hist_topQuarkPt_qqAnnihilation;
  TH1F *hist_topQuarkPt_gbFusion;
  TH1F *hist_topQuarkPt_else;
};
