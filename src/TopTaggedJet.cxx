#include "UHH2/HighPtSingleTop/include/TopTaggedJet.h"

using namespace uhh2;
using namespace std;


TopTaggedJet::TopTaggedJet(Context & ctx,
			   TopJetId topjetid,
			   const std::string & h_name):
  h_toptaggedjet(ctx.get_handle<TopJet>(h_name)),
  m_topjetid(topjetid)
{}


bool TopTaggedJet::process(uhh2::Event & event) {
  assert(event.topjets);

  TopJet toptaggedjet;

  for(auto t : *event.topjets) {
    if(m_topjetid(t, event)) toptaggedjet = t;
  }

  event.set(h_toptaggedjet, std::move(toptaggedjet));
  return true;
}


TopTaggedJet::~TopTaggedJet() {}
