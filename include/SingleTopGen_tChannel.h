// based on TTbarGen.h

#pragma once

#include "UHH2/core/include/GenParticle.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include <vector>


/** \brief ttbar generator truth information
 * 
 * Interpret a vector of GenParticle as ttbar event, providing easier access to
 * the various components in the ttbar decay chain.
 * 
 * The class can either be used directly by passing the genparticles vector;
 * another option is to use the SingleTopGen_tChannelProducer (see below), which writes the
 * result to the event container, where it can be accessed later.
 */
class SingleTopGen_tChannel {
public:
  
  /** construct from genparticles.
   * 
   * The event should be an actual ttbar event, i.e.:
   *   - there are exactly two tops: one top and one anti-top
   *   - each top must have exactl two daughters
   *   - one of the top daughters must be a W
   *   - the Ws must have exactly two daughters
   * 
   * Note that it is allowed that more particles than those belonging to the ttbar system
   * are in genparts; those are ignored.
   * 
   * In case one of these conditions is not fulfilled, the behavior
   * depends on the throw_on_failure parameter: if it is true (the default), a runtime_error
   * is thrown with an explanation what failed. If it is false, no exception is thrown, but
   * not all contents is valid; most will return a 0 vector. The one thing guaranteed is that the
   * decaychannel will be e_notfound. If using throw_on_failure = false, it is thus a good idea
   * to check the decaychannel.
   */
  explicit SingleTopGen_tChannel(const std::vector<GenParticle> & genparts, bool throw_on_failure = true);

  enum E_DecayChannel{
    e_tophad,
    e_topele,
    e_topmuo,
    e_toptau,
    e_notfound
  };

  GenParticle Top() const;
  GenParticle WTop() const;
  GenParticle bTop() const;
  GenParticle WTopDecay1() const;
  GenParticle WTopDecay2() const;
  GenParticle Initial1() const;
  GenParticle Initial2() const;
  GenParticle bSpectator() const;
  GenParticle scatteredQuark() const;
  GenParticle radiatedGluonOrQuark() const;
  E_DecayChannel DecayChannel() const;

  bool IsTopHadronicDecay() const;
  bool IsTopLeptonicDecay() const;
  bool IsTopToElectronDecay() const;
  bool IsTopToMuonDecay() const;
  bool IsTopToTauonDecay() const;

 private:

  GenParticle m_Top; 
  GenParticle m_WTop; 
  GenParticle m_bTop; 
  GenParticle m_WTopDecay1; 
  GenParticle m_WTopDecay2; 
  GenParticle m_initial1;
  GenParticle m_initial2;
  GenParticle m_bSpectator;
  GenParticle m_scatteredQuark;
  GenParticle m_radiatedGluonOrQuark;

  E_DecayChannel m_type;
};


class SingleTopGen_tChannelProducer: public uhh2::AnalysisModule {
public:
    explicit SingleTopGen_tChannelProducer(uhh2::Context & ctx, const std::string & name = "singletopgen_tChannel", bool throw_on_failure = true);
    virtual bool process(uhh2::Event & event) override;
    
private:
    uhh2::Event::Handle<SingleTopGen_tChannel> h_singletopgen_tChannel;
    bool throw_on_failure;
};
