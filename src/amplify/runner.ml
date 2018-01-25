open Flag
open PerRunReport
open MultiReports

module PredicateSet = Predicate.Set

let rd = new RunsDirectory.c
let nr = new NumRuns.c rd
let input = InputReport.factory rd
let output = OutputReport.factory rd
let lr = LogReport.factory rd
let ir = ImplicationsReports.factory ()
let sr = SitesReports.factory ()
let dl = Log.factory ()

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

  let preds = new RunResultsAccumulator.core (sites :> RunResultsAccumulator.translator) in 
  let inchannel = open_in inputFileName in
  PassOneLexer.readPredicates 
    inchannel 
    (preds :> PassOneLexer.predicates); 
  close_in inchannel;

  let observedTrue = preds#getObservedTrue () in

  let areTrue =
    Reconstruct.doAnalysis 
      (implications :> Reconstruct.implications) 
      observedTrue 
  in 

  let areChanged = PredicateSet.diff areTrue observedTrue in
  PredicateSet.iter (preds#addPredicate) areChanged;

  let outchannel = open_out outputFileName in
  preds#printSortedEntries outchannel;
  close_out outchannel;

  let ampPreds = new RunResultsAccumulator.core (sites :> RunResultsAccumulator.translator) in 
  PredicateSet.iter (ampPreds#addPredicate) areChanged;
  ampPreds#printSortedEntries logchannel;
  close_out logchannel

let readImplications () =
  let impls = new ImplicationAccumulator.c in

  let readOne impls filename =
    let inchannel = open_in filename in
    ImplLexer.readImplications inchannel (impls :> ImplLexer.implications);
    close_in inchannel;
    impls
  in

  List.fold_left (readOne) impls (ir#getNames())

let readSites () =
  let sites = new SiteInfoAccumulator.c in

  let readOne sites filename = 
    let inchannel = open_in filename in 
    SitesLexer.readSites inchannel (sites :> SitesLexer.sites);
    close_in inchannel;
    sites
  in 

  List.fold_left (readOne) sites (sr#getNames()) 

let analyzeAll () =
  let impls = readImplications () in
  let sites = readSites () in

  let sites = new PredicateTranslator.c (sites#getSiteInfos ()) in

  let implications = new Implications.impliesRelationImpl in
    List.iter (fun (l,r) -> implications#add l r) (impls#getImplications ());

  for i = nr#getBeginRuns () to nr#getEndRuns () - 1 do
    doAnalysis sites implications (input#format i) (output#format i) (lr#format i) 
  done

let main () =
  parser#parseCommandLine ();
  analyzeAll () 

let _ =  main ()
