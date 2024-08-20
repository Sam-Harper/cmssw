#include "DataFormats/Provenance/interface/ModuleDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/Registry.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/ServiceRegistry/interface/ConsumesInfo.h"
#include "FWCore/ServiceRegistry/interface/PathsAndConsumesOfModulesBase.h"
#include "FWCore/ServiceRegistry/interface/ProcessContext.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>

namespace {
    bool isOutputMod(const edm::ParameterSet& pset) {
        if (!pset.existsAs<std::string>("@module_edm_type")) {
            return false;
        }
        const std::string modType = pset.getParameter<std::string>("@module_edm_type");
        return modType == "OutputModule";
    }
}



class ProductsConsumedDumper {
public:
    ProductsConsumedDumper(const edm::ParameterSet& iConfig, edm::ActivityRegistry& iAR);
    static void fillDescripts(edm::ConfigurationDescriptions& descriptions);

    void preSourceConstruction(edm::ModuleDescription const &);
    void preBeginJob(edm::PathsAndConsumesOfModulesBase const &, edm::ProcessContext const &);
    void postBeginJob();
private:
    std::string filename_;
    bool initialised_;
};




ProductsConsumedDumper::ProductsConsumedDumper(edm::ParameterSet const &config, edm::ActivityRegistry &registry)
    : filename_(config.getUntrackedParameter<std::string>("filename")),
      initialised_(false) {
  registry.watchPreSourceConstruction(this, &ProductsConsumedDumper::preSourceConstruction);
  registry.watchPreBeginJob(this, &ProductsConsumedDumper::preBeginJob);
  registry.watchPostBeginJob(this, &ProductsConsumedDumper::postBeginJob);
}

void ProductsConsumedDumper::preSourceConstruction(edm::ModuleDescription const &desc) {
 
}

void ProductsConsumedDumper::preBeginJob(edm::PathsAndConsumesOfModulesBase const &pathsAndConsumes,
                                         edm::ProcessContext const &context) {
    auto const &registry = *edm::pset::Registry::instance();
    

    std::vector<std::string> moduleLabels;       
    auto comparator =  [](const auto& lhs,const auto &rhs) {return lhs.encode()<rhs.encode();};
    std::set<edm::InputTag,decltype(comparator)> consumedProducts(comparator);                                  
    for(const edm::ModuleDescription* desc : pathsAndConsumes.allModules()) {
        auto const &pset = *registry.getMapped(desc->parameterSetID());
        if (isOutputMod(pset)) {
            continue;
        }
        moduleLabels.push_back(desc->moduleLabel());
        
        for(const edm::ConsumesInfo& info : pathsAndConsumes.consumesInfo(desc->id())) {
            if (info.skipCurrentProcess()) {
                continue;
            }
            consumedProducts.insert(edm::InputTag(info.label().data(),info.instance().data(),info.process().data()));
            //std::cout << "Module " << desc->moduleLabel() << " consumes " << info.label()<<" "<<info.instance()<<" "<<info.process()<<" skip "<<info.skipCurrentProcess()<<" always "<<info.alwaysGets()<<std::endl;
        }
    }

    std::sort(moduleLabels.begin(), moduleLabels.end());
    for (const edm::InputTag& consumedProduct : consumedProducts) {
        if(!std::binary_search(moduleLabels.begin(), moduleLabels.end(), consumedProduct.label()) || consumedProduct.process() != "") {
            if (consumedProduct.process()== "HLT" || consumedProduct.process()== "HLTX") {
                continue;
            }
            const std::string process = consumedProduct.process() == "" ? "*" : consumedProduct.process();
            
            
            std::cout << "\"keep *_"+consumedProduct.label()+"_"+consumedProduct.instance()+"_"+process<<"\"," << std::endl;

        }
    }

}

void ProductsConsumedDumper::postBeginJob()
{

}




#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
DEFINE_FWK_SERVICE(ProductsConsumedDumper);