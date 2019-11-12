#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/Utils.h"


/** \brief Module to apply MET XY corrections
 *
 * Must be used on Type-I corrected MET
 * No up/down variations available yet! (Nov. 12, 2019)
 * For documentation, see:
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETRun2Corrections
 */


class METXYCorrections: public uhh2::AnalysisModule {
 public:
  METXYCorrections(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;
  virtual ~METXYCorrections();

 private:
  Year year;
  bool init_done;
  bool usemetv2;
  bool isMC;
  
  enum TheRunEra{y2016B,
		 y2016C,
		 y2016D,
		 y2016E,
		 y2016F,
		 y2016G,
		 y2016H,
		 y2017B,
		 y2017C,
		 y2017D,
		 y2017E,
		 y2017F,
		 y2018A,
		 y2018B,
		 y2018C,
		 y2018D,
		 y2016MC,
		 y2017MC,
		 y2018MC};
};
