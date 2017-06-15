#include "RecoEgamma/EgammaElectronAlgos/interface/EgammaPixelMatchParamObjects.h"

edm::ParameterSetDescription egPM::makeParamBinsDesc()
{
  edm::ParameterSetDescription binParamDesc;
  auto binDescCases = 
    "AbsEtaClus" >>
    (edm::ParameterDescription<double>("xMin",0,true) and
     edm::ParameterDescription<double>("xMax",3.0,true) and
     edm::ParameterDescription<int>("yMin",0,true) and
     edm::ParameterDescription<int>("yMax",9999,true) and
     edm::ParameterDescription<std::string>("funcType","TF1:pol0",true) and
     edm::ParameterDescription<std::vector<double> >("funcParams",{0.},true)) or
    "AbsEtaCharge" >>
    (edm::ParameterDescription<double>("xMin",0,true) and
     edm::ParameterDescription<double>("xMax",3.0,true) and
     edm::ParameterDescription<int>("yMin",-1,true) and
     edm::ParameterDescription<int>("yMax",1,true) and
     edm::ParameterDescription<std::string>("funcType","TF1:pol0",true) and
     edm::ParameterDescription<std::vector<double> >("funcParams",{0.},true)) or
    "AbsEtaClusPhi" >>
    (edm::ParameterDescription<double>("xMin",0,true) and
     edm::ParameterDescription<double>("xMax",3.0,true) and
     edm::ParameterDescription<int>("yMin",0,true) and
     edm::ParameterDescription<int>("yMax",9999,true) and
     edm::ParameterDescription<double>("zMin",-4,true) and
     edm::ParameterDescription<double>("zMax",4,true) and
     edm::ParameterDescription<std::string>("funcType","TF1:pol0",true) and
     edm::ParameterDescription<std::vector<double> >("funcParams",{0.},true)) or 
    "AbsEtaClusEt" >>
    (edm::ParameterDescription<double>("xMin",0,true) and
     edm::ParameterDescription<double>("xMax",3.0,true) and
     edm::ParameterDescription<int>("yMin",0,true) and
     edm::ParameterDescription<int>("yMax",9999,true) and
     edm::ParameterDescription<double>("zMin",0,true) and
     edm::ParameterDescription<double>("zMax",-1,true) and
     edm::ParameterDescription<std::string>("funcType","TF1:pol0",true) and
     edm::ParameterDescription<std::vector<double> >("funcParams",{0.},true)) or 
    "EtAbsEta" >>
    (edm::ParameterDescription<double>("xMin",0,true) and
     edm::ParameterDescription<double>("xMax",-1,true) and
     edm::ParameterDescription<double>("yMin",0,true) and
     edm::ParameterDescription<double>("yMax",3.0,true) and
     edm::ParameterDescription<std::string>("funcType","TF1:pol0",true) and
     edm::ParameterDescription<std::vector<double> >("funcParams",{0.},true)) or
    "EtAbsEtaCharge" >>
    (edm::ParameterDescription<double>("xMin",0,true) and
     edm::ParameterDescription<double>("xMax",-1,true) and
     edm::ParameterDescription<double>("yMin",0,true) and
     edm::ParameterDescription<double>("yMax",3.0,true) and
     edm::ParameterDescription<int>("zMin",-1,true) and
     edm::ParameterDescription<int>("zMax",1,true) and
     edm::ParameterDescription<std::string>("funcType","TF1:pol0",true) and
     edm::ParameterDescription<std::vector<double> >("funcParams",{0.},true)) or
    "EtAbsEtaPhi" >>
    (edm::ParameterDescription<double>("xMin",0,true) and
     edm::ParameterDescription<double>("xMax",-1,true) and
     edm::ParameterDescription<double>("yMin",0,true) and
     edm::ParameterDescription<double>("yMax",3.0,true) and
     edm::ParameterDescription<double>("zMin",-4,true) and
     edm::ParameterDescription<double>("zMax",4,true) and
     edm::ParameterDescription<std::string>("funcType","TF1:pol0",true) and
     edm::ParameterDescription<std::vector<double> >("funcParams",{0.},true)) or
    "Const" >>
    (edm::ParameterDescription<double>("val",0,true));
  binParamDesc.ifValue(edm::ParameterDescription<std::string>("binType","EtAbsEtaCharge",true),std::move(binDescCases));
  
  return binParamDesc;
}

