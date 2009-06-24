// -*- C++ -*-
//
// Package:    HLTEventNumberFilter
// Class:      HLTEventNumberFilter
// 
/**\class HLTEventNumberFilter HLTEventNumberFilter.cc filter/HLTEventNumberFilter/src/HLTEventNumberFilter.cc

Description: 

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Martin Grunewald
//         Created:  Tue Jan 22 13:55:00 CET 2008
// $Id: HLTEventNumberFilter.cc,v 1.4 2009/05/07 16:04:43 gruen Exp $
//
//


// system include files
#include <string>
#include <iostream>
#include <memory>

// user include files
#include "HLTrigger/special/interface/HLTEventNumberFilter.h"

//
// constructors and destructor
//
HLTEventNumberFilter::HLTEventNumberFilter(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed

  period_ = iConfig.getParameter<unsigned int>("period") ;
  invert_ = iConfig.getParameter<bool>("invert") ;
}


HLTEventNumberFilter::~HLTEventNumberFilter()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
HLTEventNumberFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  bool accept(false);

  if (period_!=0) accept = ( ( (iEvent.id().event())%period_ ) == 0 );

  if (invert_) accept = !accept;

  return accept;

}
