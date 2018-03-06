#ifndef RecoEgamma_EgammaTools_EgammaRegressionContainer_h
#define RecoEgamma_EgammaTools_EgammaRegressionContainer_h

#include "RecoEgamma/EgammaTools/interface/EgammaBDTOutputTransformer.h"

#include <string>

namespace edm{
  class ParameterSet;
  class EventSetup;
}
class GBRForestD;

class EgammaRegressionContainer {
public:
  
  EgammaRegressionContainer(const edm::ParameterSet& iConfig);
  ~EgammaRegressionContainer(){}

  void setEventContent(const edm::EventSetup& iSetup);  

  float operator()(float et,bool isEB,const float* data)const;
  

private:
  const EgammaBDTOutputTransformer outputTransformer_;

  const float lowEtHighEtBoundary_;
  const std::string ebLowEtForestName_;
  const std::string ebHighEtForestName_;
  const std::string eeLowEtForestName_;
  const std::string eeHighEtForestName_;

  const GBRForestD* ebLowEtForest_; //not owned
  const GBRForestD* ebHighEtForest_; //not owned
  const GBRForestD* eeLowEtForest_; //not owned
  const GBRForestD* eeHighEtForest_; //not owned

};


#endif
