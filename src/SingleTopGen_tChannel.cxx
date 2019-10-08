#include "UHH2/HighPtSingleTop/include/SingleTopGen_tChannel.h"

using namespace std;
using namespace uhh2;

SingleTopGen_tChannel::SingleTopGen_tChannel(const vector<GenParticle> & genparticles, bool throw_on_failure): m_type(e_notfound) {    

  int n_top = 0;
  GenParticle top, initial1, initial2, bSpectator;

  for(unsigned int i = 0; i < genparticles.size(); ++i) {
    const GenParticle & genp = genparticles[i];
    if (genp.index() == 0){ // find the initial state particles
      initial1 = genp;
      m_initial1 = initial1;
    }
    else if (genp.index() == 1){
      initial2 = genp;
      m_initial2 = initial2;
    }
    else if (abs(genp.pdgId()) == 6){ // We don't distinguish between top and antitop. May be changed later but for now it's okay
      top = genp;
      auto w = genp.daughter(&genparticles, 1);
      auto b = genp.daughter(&genparticles, 2);
      if(!w || !b) {
        if(throw_on_failure) throw runtime_error("SingleTopGen_tChannel: top has not ==2 daughters");
        return;
      }
      if (abs(w->pdgId()) != 24) {
        std::swap(w, b);
      }
      /* It rarely happens that the list of genparticles contains 4 or more (instead of 2) particles which reckon the same top
         as their mother although each particle including the tops can just have two daughters. E.g. if the top emits a photon
         before decaying into b and W, this photon may split up into two leptons which reckon the top as their mother, too.
         Therefore, it may happen that those leptons are considered as the top daughters whereas b and W are "ignored" and cannot
         be found. This workaround fixes that issue: */
      if(abs(w->pdgId()) != 24) {
        for(unsigned int j = 0; j < genparticles.size(); ++j) {
          const GenParticle & gp = genparticles[j];
          auto m1 = gp.mother(&genparticles, 1);
          auto m2 = gp.mother(&genparticles, 2);
          bool has_top_mother = ((m1 && m1->index() == genp.index()) || (m2 && m2->index() == genp.index()));
          if(has_top_mother && (abs(gp.pdgId()) == 24)) {
            w = &gp;
            break;
          }
        }
      }
      if (abs(w->pdgId()) != 24) {
        if(throw_on_failure) throw runtime_error("SingleTopGen_tChannel: top has no W daughter");
        return;
      }

      /* Do a similar workaround as above if the expected b daughter has not been found yet */
      if(abs(b->pdgId()) != 5 && abs(b->pdgId()) != 3 && abs(b->pdgId()) != 1) {
        for(unsigned int j = 0; j < genparticles.size(); ++j) {
          const GenParticle & gp = genparticles[j];
          auto m1 = gp.mother(&genparticles, 1);
          auto m2 = gp.mother(&genparticles, 2);
          bool has_top_mother = ((m1 && m1->index() == genp.index()) || (m2 && m2->index() == genp.index()));
          if(has_top_mother && (abs(gp.pdgId()) == 5 || abs(gp.pdgId()) == 3 || abs(gp.pdgId()) == 1)) {
            b = &gp;
            break;
          }
        }
      }
      if (abs(b->pdgId()) != 5 && abs(b->pdgId()) != 3   && abs(b->pdgId()) != 1) {
        if(throw_on_failure) throw runtime_error("SingleTopGen_tChannel: top has no b daughter");
        return;
      }
      // now get WTop daughters:

      int n_WTopDau = 0;

      auto wd1 = w->daughter(&genparticles, 1);
      auto wd2 = w->daughter(&genparticles, 2);

      while (n_WTopDau != 2){

        if(wd1 && !wd2){
          w = wd1;
          wd1 = w->daughter(&genparticles, 1);
          wd2 = w->daughter(&genparticles, 2);
        }
        else if(wd1 && wd2){
          n_WTopDau = 2;
        }

        else{
          if(throw_on_failure) throw runtime_error("SingleTopGen_tChannel: WTop has no daughters");
          return;
        }

      }
      if(!wd1 || !wd2){
        if(throw_on_failure) throw runtime_error("SingleTopGen_tChannel: WTop has not ==2 daughters");
        return;
      }
            
      // now that we collected everything, fill the member variables. 
      // Unlike in TTbarGen.cxx, where we use different member variables according to top quark charge, we do not distinguish them here because there is just one top
      m_Top = top;
      m_WTop = *w;
      m_bTop = *b;
      m_WTopDecay1 = *wd1;
      m_WTopDecay2 = *wd2;
      ++n_top;

    }

    // now identify the associated b quark ('spectator') coming from the gluon splitting due to 4-fermion scheme being used
    else if(genp.index() == 3) { // using the index is highly dependent on the MC sample but it seems that it is o.k. for now
      bSpectator = genp;
      if(abs(bSpectator.pdgId()) == 3){
	std::cout << "INFO from SingleTopGen_tChannel:  found t-channel event with no spectator b quark but spectator s quark" << std::endl;
      }
      else if(abs(bSpectator.pdgId()) == 1){
	std::cout << "INFO from SingleTopGen_tChannel:  found t-channel event with no spectator b quark but spectator d quark" << std::endl;
      }
      m_bSpectator = bSpectator;
    }

    else if(genp.index() == 4) { // the scattered (light) quark that exchanged the W boson
      m_scatteredQuark = genp;
    }

    else if(genp.index() == 5) { // radiated gluon or additional quark that radiates a gluon splitting into the bb pair
      m_radiatedGluonOrQuark = genp;
    }

  }


  if(n_top != 1){
    if(throw_on_failure) throw runtime_error("SingleTopGen_tChannel: did not find exactly one (anti)top in the event");
    return;
  }
  
  
  // calculate decay channel by counting the number of charged leptons in the WTop daughters
  int n_e = 0, n_m = 0, n_t = 0;
  for(const auto & wd : {m_WTopDecay1, m_WTopDecay2}){
    int id_wd = abs(wd.pdgId());
    if(id_wd == 11) ++n_e;
    else if(id_wd == 13) ++n_m;
    else if(id_wd == 15) ++n_t;
  }
  if(n_e == 1){
    m_type = e_topele;
  }
  else if(n_m == 1){
    m_type = e_topmuo;
  }
  else if(n_t == 1){
    m_type = e_toptau;
  }
  else if(n_e + n_m + n_t == 0){
    m_type = e_tophad;
  }
  else{ // this should not happen except when the W has more than one charged lepton under its daughters (maybe because of W --> W gamma --> 3 lep + 1 nu)
    m_type = e_notfound;
  }

}


GenParticle SingleTopGen_tChannel::Top() const{
  return m_Top;
}

GenParticle SingleTopGen_tChannel::WTop() const{
  return m_WTop;
}

GenParticle SingleTopGen_tChannel::bTop() const{
  return m_bTop;
}

GenParticle SingleTopGen_tChannel::WTopDecay1() const{
  return m_WTopDecay1;
} 

GenParticle SingleTopGen_tChannel::WTopDecay2() const{
  return m_WTopDecay2;
} 

GenParticle SingleTopGen_tChannel::Initial1() const{
  return m_initial1;
}

GenParticle SingleTopGen_tChannel::Initial2() const{
  return m_initial2;
}

GenParticle SingleTopGen_tChannel::bSpectator() const{
  return m_bSpectator;
}

GenParticle SingleTopGen_tChannel::scatteredQuark() const{
  return m_scatteredQuark;
}

GenParticle SingleTopGen_tChannel::radiatedGluonOrQuark() const{
  return m_radiatedGluonOrQuark;
}


SingleTopGen_tChannel::E_DecayChannel SingleTopGen_tChannel::DecayChannel() const{  
  return m_type;
}


bool SingleTopGen_tChannel::IsTopHadronicDecay() const{
  return m_type == e_tophad;
}

bool SingleTopGen_tChannel::IsTopLeptonicDecay() const{
  return m_type == e_topele || m_type == e_topmuo || m_type == e_toptau;
}

bool SingleTopGen_tChannel::IsTopToElectronDecay() const{
  return m_type == e_topele;
}

bool SingleTopGen_tChannel::IsTopToMuonDecay() const{
  return m_type == e_topmuo;
}

bool SingleTopGen_tChannel::IsTopToTauonDecay() const{
  return m_type == e_toptau;
}



SingleTopGen_tChannelProducer::SingleTopGen_tChannelProducer(uhh2::Context & ctx, const std::string & name, bool throw_on_failure_): throw_on_failure(throw_on_failure_){
  h_singletopgen_tChannel = ctx.get_handle<SingleTopGen_tChannel>(name);
}

bool SingleTopGen_tChannelProducer::process(Event & event){
  event.set(h_singletopgen_tChannel, SingleTopGen_tChannel(*event.genparticles, throw_on_failure));
  return true;
}
