open Flag
open AnalysisReport
open PerRunReport
open MultiReports

type predicate = Predicate.p
module PredicateSet = Predicate.Set 

let rd = new RunsDirectory.c
let nr = new NumRuns.c rd
let input = InputReport.factory rd
let sr = SitesReports.factory ()
let tpi = TruthProbabilitiesInput.factory ()
let ntpi = NotTruthProbabilitiesInput.factory ()
let tpo = TruthProbabilitiesOutput.factory ()
let ntpo = NotTruthProbabilitiesOutput.factory ()
let pr = PredsReport.factory () 
let dl = Log.factory ()
let lr = AnalysisReport.LogReport.factory ()

let parsers = [ 
  (sr :> CommandLine.argParser) ;
  (rd :> CommandLine.argParser) ; 
  (nr :> CommandLine.argParser) ; 
  (input :> CommandLine.argParser) ; 
  (tpi :> CommandLine.argParser) ;
  (ntpi :> CommandLine.argParser) ;
  (tpo :> CommandLine.argParser) ;
  (ntpo :> CommandLine.argParser) ;
  (pr :> CommandLine.argParser) ;
  (dl :> CommandLine.argParser) ;
  (lr :> CommandLine.argParser) ]

let parser = new CommandLine.parser parsers "amplify_truth_probs"

let readSites () =
  let sites = new SiteInfoAccumulator.c in

  let readOne sites filename = 
    let inchannel = open_in filename in 
    SitesLexer.readSites inchannel (sites :> SitesLexer.sites);
    close_in inchannel;
    sites
  in 

  List.fold_left (readOne) sites (sr#getNames()) 

let readPreds trans =
  let preds = 
    new PredicateAccumulator.c (trans :> PredicateAccumulator.translator) in

  let inchannel = open_in (pr#getName()) in
  PredsLexer.read inchannel (preds :> PredsLexer.preds);
  close_in inchannel;
  preds

let readRunResults trans run =
  let results =
    new RunResultsAccumulator.c (trans :> RunResultsAccumulator.translator) in
  
  let inchannel = open_in (input#format run) in
  PassOneLexer.readPredicates
    inchannel
    (results :> PassOneLexer.predicates);
  close_in inchannel;
  results

let readTruthProbs l c =
  let rec readAux l c =
    if c = 1 
      then Scanf.sscanf 
             l 
             "%f%s" 
             (fun x s -> if s = "" 
               then [x] 
               else failwith ("malformed truth probability file")) 
      else Scanf.sscanf l "%f %s@\n" (fun x s -> x::readAux s (c - 1))
  in
    if c < 1 then [] else readAux l c  

let rec writeTruthProbs l outstream =
  if (List.length l = 0) 
    then Printf.fprintf outstream "\n"
    else 
      begin
        Printf.fprintf outstream "%18.12e " (List.hd l); 
        writeTruthProbs (List.tl l) outstream
      end

let analyzeAll () =
  let sites = readSites () in

  let sites = new PredicateTranslator.c (sites#getSiteInfos ()) in
  
  let preds = readPreds sites in
  let preds = preds#getPredicates () in
  let notPreds = List.map (Predicate.complement) preds in
  let predCount = List.length preds in

  let logchannel = open_out (lr#getName()) in
  let logger = (Logger.factory (dl#shouldDo()) logchannel) in

  let calcTruthProb results pred prob =
    if prob = 1.0 then 1.0 else
    let preds = Predicate.synth_to_ground_disjunction pred in
    if (List.exists (results#isTrue) preds) then 
      begin
        if(dl#shouldDo()) then
         logger#logPredicate pred else ();
        1.0 
      end  
      else prob
  in

  let analyze preds instream outstream results =
    let probs = readTruthProbs (input_line instream) predCount in
    let probs = List.rev
      (List.fold_left2 
        (fun l a b -> (calcTruthProb results a b)::l) 
        []
        preds 
        probs) in 
    writeTruthProbs probs outstream;
    logger#advance ()
  in

  let inX = open_in (tpi#getName()) in
  let inNotX = open_in (ntpi#getName()) in
  let outX = open_out (tpo#getName()) in
  let outNotX = open_out (ntpo#getName()) in
  
  for i = nr#getBeginRuns () to nr#getEndRuns () - 1 do
    let results = readRunResults sites i in
    analyze preds inX outX results;
    analyze notPreds inNotX outNotX results
  done;

  close_in inX;
  close_in inNotX;
  close_out outX;
  close_out outNotX;
  close_out logchannel

let main () =
  parser#parseCommandLine ();
  analyzeAll () 

let _ =  main ()
