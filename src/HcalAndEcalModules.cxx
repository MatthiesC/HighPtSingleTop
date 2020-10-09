#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/HighPtSingleTop/include/HcalAndEcalModules.h"

using namespace std;
using namespace uhh2;


PrefiringWeights::PrefiringWeights(Context & ctx) {

  if(ctx.get("SystDirection_Prefiring") == "up") {
    syst_direction = 1;
  }
  else if(ctx.get("SystDirection_Prefiring") == "down") {
    syst_direction = -1;
  }
  else {
    syst_direction = 0;
  }
}

bool PrefiringWeights::process(Event & event) {

  if(syst_direction == 1) {
    event.weight *= event.prefiringWeightUp;
  }
  else if(syst_direction == -1) {
    event.weight *= event.prefiringWeightDown;
  }
  else {
    event.weight *= event.prefiringWeight;
  }

  return true;
}
