type predicate = Predicate.p
module PredicateSet = Predicate.Set 

let rd = new RunsDirectory.c
let nr = new NumRuns.c rd
let input = new InputReport.c rd
let output = new OutputReport.c rd
let lr = new LogReport.c rd
let ir = new ImplicationsReport.c 
let sr = new SitesReport.c
let dl = new Logging.c

let parsers = [ 
  (ir :> CommandLine.argParser) ;  
  (sr :> CommandLine.argParser) ;
  (rd :> CommandLine.argParser) ; 
  (nr :> CommandLine.argParser) ; 
  (input :> CommandLine.argParser) ; 
  (output :> CommandLine.argParser) ; 
  (lr :> CommandLine.argParser) ;
  (dl :> CommandLine.argParser) ]

let parser = new CommandLine.parser parsers "amplify"

let doAnalysis sites implications inputFileName outputFileName logFileName = 
  let logchannel = open_out logFileName in
  let logger = (Logger.factory (dl#doLogging()) logchannel) in

  let preds = new PredicateAccumulator.c (sites :> PredicateAccumulator.translator) in 
  let inchannel = open_in inputFileName in
  PassOneLexer.readPredicates 
    inchannel 
    (preds :> PassOneLexer.predicates); 
  close_in inchannel;

  let observedTrue = preds#getObservedTrue () in

  let areTrue =
    Reconstruct.doAnalysis 
      (logger :> Reconstruct.logger) 
      (implications :> Reconstruct.implications) 
      observedTrue 
  in 

  let areChanged = PredicateSet.diff areTrue observedTrue in

  PredicateSet.iter (preds#addPredicate) areChanged;

  let outchannel = open_out outputFileName in
  preds#printSortedEntries outchannel;
  close_out outchannel;
  close_out logchannel

let analyzeAll () =
  let failed = false in
  let impls = new ImplicationAccumulator.c in 
  let inchannel = open_in (ir#getName ()) in
  ImplLexer.readImplications inchannel (impls :> ImplLexer.implications); 
  close_in inchannel;

  let sites = new SiteInfoAccumulator.c in
  let inchannel = open_in (sr#getName ()) in
  SitesLexer.readSites inchannel (sites :> SitesLexer.sites);
  close_in inchannel;

  let sites = new PredicateTranslator.c (sites#getSiteInfos ()) in

  let implications = new Implications.c in
    List.iter (fun (l,r) -> implications#add l r) (impls#getImplications ());

  for i = nr#getBeginRuns () to nr#getEndRuns () - 1 do
    doAnalysis sites implications (input#format i) (output#format i) (lr#format i) 
  done

let main () =
  parser#parseCommandLine ();
  analyzeAll () 

let _ =  main ()
