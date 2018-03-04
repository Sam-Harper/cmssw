#ifndef RecoEgamma_EgammaTools_EnergyScaleCorrection_h
#define RecoEgamma_EgammaTools_EnergyScaleCorrection_h

/// Read and get energy scale and smearings from .dat files
/**\class EnergyScaleCorrection_class EnergyScaleCorrection_class.cc Calibration/ZFitter/src/EnergyScaleCorrection_class.cc
 *  \author Shervin Nourbakhsh
 *
 */

/** Description
	This module is taken from the ECALELF package, used to derive the energy scales and smearings.

	There is one class that reads the text files with the corrections and returns the scale/smearings given the electron/photon properties
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <bitset> 


class EnergyScaleCorrection
{	
public:  
  enum FileFormat{
    UNKNOWN=0,
    GLOBE,
    ECALELF_TOY,
    ECALELF
  };

  enum ParamSmear{
    kNone = 0,
    kRho,
    kPhi,
    kNParamSmear
  };
  
  enum ScaleNuisances{
    kErrNone = 0,
    kErrStat = 1,
    kErrSyst = 2,
    kErrGain = 4,
    kErrStatSyst = 3,
    kErrStatGain = 5,
    kErrSystGain = 6,
    kErrStatSystGain = 7,
    kErrNrBits=3
  };
  

  
  struct ScaleCorrection
  {
  public:  
    ScaleCorrection():
      scale(1.),scaleErrStat(0.),scaleErrSyst(0.),scaleErrGain(0.){}
    ScaleCorrection(float iScale,float iScaleErrStat,float iScaleErrSyst,float iScaleErrGain):
      scale(iScale),scaleErrStat(iScaleErrStat),scaleErrSyst(iScaleErrSyst),scaleErrGain(iScaleErrGain){}
    
    friend std::ostream& operator<<(std::ostream& os, const ScaleCorrection& a){return a.print(os);}
    std::ostream& print(std::ostream& os)const;
    
    float scale, scaleErrStat, scaleErrSyst, scaleErrGain;
  };
  
  struct SmearCorrection
  {
  public:  
    SmearCorrection():
      rho(0.),rhoErr(0.),phi(0.),phiErr(0.),
      eMean(0.),eMeanErr(0.){}
    SmearCorrection(float iRho,float iRhoErr,float iPhi,float iPhiErr,float iEMean,float iEMeanErr):
      rho(iRho),rhoErr(iRhoErr),phi(iPhi),phiErr(iPhiErr),
      eMean(iEMean),eMeanErr(iEMeanErr){}
    
    friend std::ostream& operator<<(std::ostream& os, const SmearCorrection& a){return a.print(os);}
    std::ostream& print(std::ostream& os)const;
    
    float rho, rhoErr;
    float phi, phiErr;
    float eMean, eMeanErr;
  };
  
  class CorrectionCategory
  {  
  public:
    CorrectionCategory(const std::string& category,int runnrMin=0,int runnrMax=999999);
    CorrectionCategory(const unsigned int runnr, const float etaEle, const float r9Ele, 
		       const float etEle, const unsigned int gainSeed):
      runMin_(runnr),runMax_(runnr),etaMin_(std::abs(etaEle)),etaMax_(std::abs(etaEle)),
      r9Min_(r9Ele),r9Max_(r9Ele),etMin_(etEle),etMax_(etEle),gain_(gainSeed){}
    
    bool operator<(const CorrectionCategory& b) const;
    
    
    friend std::ostream& operator << (std::ostream& os, const CorrectionCategory& a){return a.print(os);}
    std::ostream& print(std::ostream &os)const;
    
  private:
    unsigned int runMin_;
    unsigned int runMax_;
    float etaMin_; ///< min eta value for the bin
    float etaMax_; ///< max eta value for the bin
    float r9Min_;  ///< min R9 vaule for the bin
    float r9Max_;  ///< max R9 value for the bin
    float etMin_;  ///< min Et value for the bin
    float etMax_;  ///< max Et value for the bin
    unsigned int gain_; ///< 12, 6, 1, 61 (double gain switch)
  };

    
public:
  EnergyScaleCorrection(const std::string& correctionFileName, unsigned int genSeed=0);
  EnergyScaleCorrection(){};
  ~EnergyScaleCorrection(){}
  
 
  float scaleCorr(unsigned int runnr, bool isEBEle, double r9Ele, double etaSCEle,
			double etEle, unsigned int gainSeed=12, 
			std::bitset<kErrNrBits> uncBitMask=kErrNone) const; 
  
  float scaleCorrUncert(unsigned int runnr, bool isEBEle,double r9Ele, 
			double etaSCEle,double etEle, unsigned int gainSeed, 
			std::bitset<kErrNrBits> uncBitMask=kErrNone) const;
  
  float smearingSigma(int runnr, bool isEBEle, float r9Ele, float etaSCEle, float etEle, unsigned int gainSeed, ParamSmear par, float nSigma = 0.) const;
  float smearingSigma(int runnr, bool isEBEle, float r9Ele, float etaSCEle, float etEle, unsigned int gainSeed, float nSigmaRho, float nSigmaPhi) const;
  
  void setSmearingType(FileFormat value);
  void setDoScale(bool val){doScale_=val;}
  void setDoSmearings(bool val){doSmearings_=val;}

 private:

  const ScaleCorrection* getScaleCorr(unsigned int runnr, bool isEBEle, double R9Ele, double etaSCEle, double EtEle, unsigned int gainSeed) const; 
  const SmearCorrection* getSmearCorr(unsigned int runnr, bool isEBEle, double R9Ele, double etaSCEle, double EtEle, unsigned int gainSeed) const; 
  
  void addScale(const std::string& category, int runMin, int runMax, 
		double deltaP, double errDeltaP, double errSystDeltaP, double errDeltaPGain);
  void addSmearing(const std::string& category, int runMin, int runMax,
		   double rho, double errRho, double phi, double errPhi, 
		   double eMean, double errEMean);
  
  void readScalesFromFile(const std::string& filename); 
  void readSmearingsFromFile(const std::string& filename); 

  //static data members
  static constexpr float kDefaultScaleVal_ = 1.0;
  static constexpr float kDefaultSmearVal_ = 0.0;

  //data members
  FileFormat smearingType_;  
  std::vector<std::pair<CorrectionCategory,ScaleCorrection> >scales_;
  std::vector<std::pair<CorrectionCategory,SmearCorrection> >smearings_;
  bool doScale_, doSmearings_;
  
  template<typename T1,typename T2>
  class Sorter{
  public:
    bool operator()(const std::pair<T1,T2>& lhs,const T1& rhs)const{return lhs.first<rhs;}
    bool operator()(const std::pair<T1,T2>& lhs,const std::pair<T1,T2>& rhs)const{return lhs.first<rhs.first;}
    bool operator()(const T1& lhs,const std::pair<T1,T2>& rhs)const{return lhs<rhs.first;}
    bool operator()(const T1& lhs,const T1& rhs)const{return lhs<rhs;}
  };
  
};


#endif
