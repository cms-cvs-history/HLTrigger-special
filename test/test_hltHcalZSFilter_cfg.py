import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( 
    input = cms.untracked.int32(10000) 
)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

process.source = cms.Source("EmptySource")

process.load("HLTrigger.special.hltHcalZSFilter_cfi")

process.hltTrigReport = cms.EDAnalyzer( "HLTrigReport",
    HLTriggerResults = cms.InputTag( 'TriggerResults' )
)

# select only NZS events (# 4096, # 8192, ...)
process.path = cms.Path( ~ process.hltHcalZSFilter ) 
process.end  = cms.EndPath( process.hltTrigReport )
