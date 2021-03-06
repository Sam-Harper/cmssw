// -*- C++ -*-
//
// Package:     Core
// Class  :     FWSimpleProxyHelper
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:  Chris Jones
//         Created:  Tue Dec  2 15:13:22 EST 2008
// $Id: FWSimpleProxyHelper.cc,v 1.5 2012/08/03 18:20:28 wmtan Exp $
//

// system include files
#include <sstream>
#include <cassert>

#include "FWCore/Utilities/interface/ObjectWithDict.h"
#include "FWCore/Utilities/interface/TypeWithDict.h"
#include "TClass.h"

// user include files
#include "Fireworks/Core/interface/FWSimpleProxyHelper.h"
#include "Fireworks/Core/interface/FWEventItem.h"


//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
FWSimpleProxyHelper::FWSimpleProxyHelper(const std::type_info& iType) :
   m_itemType(&iType),
   m_objectOffset(0)
{
}

// FWSimpleProxyHelper::FWSimpleProxyHelper(const FWSimpleProxyHelper& rhs)
// {
//    // do actual copying here;
// }

//FWSimpleProxyHelper::~FWSimpleProxyHelper()
//{
//}

//
// assignment operators
//
// const FWSimpleProxyHelper& FWSimpleProxyHelper::operator=(const FWSimpleProxyHelper& rhs)
// {
//   //An exception safe implementation is
//   FWSimpleProxyHelper temp(rhs);
//   swap(rhs);
//
//   return *this;
// }

//
// member functions
//
void
FWSimpleProxyHelper::itemChanged(const FWEventItem* iItem)
{
   if(0!=iItem) {
      edm::TypeWithDict myType(*m_itemType);
      edm::ObjectWithDict dummy(edm::TypeWithDict(*(iItem->modelType()->GetTypeInfo())),
                   reinterpret_cast<void*>(0xFFFF));
      edm::ObjectWithDict castTo(dummy.castObject(myType));
      assert(0!=castTo.address());
      m_objectOffset=static_cast<char*>(dummy.address())-static_cast<char*>(castTo.address());
   }
}

//
// static member functions
//
