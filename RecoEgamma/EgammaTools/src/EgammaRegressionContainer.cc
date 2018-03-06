#include "RecoEgamma/EgammaTools/interface/EgammaRegressionContainer.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CondFormats/DataRecord/interface/GBRDWrapperRcd.h"
#include "CondFormats/EgammaObjects/interface/GBRForestD.h"

EgammaRegressionContainer::EgammaRegressionContainer(const edm::ParameterSet& iConfig):
  outputTransformer_(iConfig.getParameter<double>("rangeMin"),iConfig.getParameter<double>("rangeMax")),
  lowEtHighEtBoundary_(iConfig.getParameter<double>("lowEtHighEtBoundary")),
  ebLowEtForestName_(iConfig.getParameter<std::string>("ebLowEtForestName")),
  ebHighEtForestName_(iConfig.getParameter<std::string>("ebHighEtForestName")),
  eeLowEtForestName_(iConfig.getParameter<std::string>("eeLowEtForestName")),
  eeHighEtForestName_(iConfig.getParameter<std::string>("eeHighEtForestName")),
  ebLowEtForest_(nullptr),ebHighEtForest_(nullptr),
  eeLowEtForest_(nullptr),eeHighEtForest_(nullptr)
{

}

namespace{
  const GBRForestD* getForest(const edm::EventSetup& iSetup,const std::string&name){
    edm::ESHandle<GBRForestD> handle;
    iSetup.get<GBRDWrapperRcd>().get(name,handle);
    return handle.product();
  }
}

void EgammaRegressionContainer::setEventContent(const edm::EventSetup& iSetup)
{
  ebLowEtForest_  = getForest(iSetup,ebLowEtForestName_);
  ebHighEtForest_ = getForest(iSetup,ebHighEtForestName_);
  eeLowEtForest_  = getForest(iSetup,eeLowEtForestName_);
  eeHighEtForest_ = getForest(iSetup,eeHighEtForestName_); 
}

float EgammaRegressionContainer::operator()(float et,bool isEB,const float* data)const
{
  if(isEB){
    if(et<lowEtHighEtBoundary_) return outputTransformer_(ebLowEtForest_->GetResponse(data));
    else return outputTransformer_(ebHighEtForest_->GetResponse(data));
  }else{
    if(et<lowEtHighEtBoundary_) return outputTransformer_(eeLowEtForest_->GetResponse(data));
    else return outputTransformer_(eeHighEtForest_->GetResponse(data));
  }
}
