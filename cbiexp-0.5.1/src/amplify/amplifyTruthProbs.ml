open Flag
open AnalysisReport
open PerRunReport
open MultiReports

type predicate = Predicate.p
module PredicateTable = Predicate.Table
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
let ir = ImplicationsReports.factory ()

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
  (lr :> CommandLine.argParser) ;
  (ir :> CommandLine.argParser) ]

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

let readImplications () =
  let impls = new ImplicationAccumulator.c in

  let readOne impls filename =
    let inchannel = open_in filename in
    ImplLexer.readImplications inchannel (impls :> ImplLexer.implications);
    close_in inchannel;
    impls
  in

  List.fold_left readOne impls (ir#getNames()) 

let readPreds trans =
  let preds = 
    new PredicateAccumulator.c (trans :> PredicateAccumulator.translator) in

  let inchannel = open_in (pr#getName()) in
  PredsLexer.read inchannel (preds :> PredsLexer.preds);
  close_in inchannel;
  preds

let readRunResults trans run =
  let results =
    new RunResultsAccumulator.synth (trans :> RunResultsAccumulator.translator) in
  
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
    if c < 1 then [] else (readAux l c)  

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

  let impls = readImplications () in
  let impls = impls#getImplications () in
  let implications = new Implications.impliesSynthRelationImpl in
  List.iter (fun (l,r) -> implications#add l r) impls; 
  
  let preds = readPreds sites in
  let preds = preds#getPredicates () in
  let notPreds = List.map (Predicate.complement) preds in
  let predCount = List.length preds in

  let logchannel = open_out (lr#getName()) in
  let logger = (Logger.factory (dl#shouldDo()) logchannel) in

  let calcTruthProb results pred prob =
    if prob = 1.0 then 1.0 else
    try 
      let prob = PredicateTable.find results pred in
      begin
        if(dl#shouldDo()) then
         logger#logPredicate pred else ();
        prob 
      end
    with
      Not_found -> 0.0 
  in

  let analyze preds probs outstream results =
    let probs = List.rev 
      (List.fold_left2 
        (fun l a b -> (calcTruthProb results a b)::l) 
        []
        preds 
        probs) in 
    writeTruthProbs probs outstream;
    logger#advance ()
  in

  let addProbs probs preds results =
    List.iter2 (fun a b -> (results#addPredicateProb a b)) preds probs
  in

  let amplify impls results =
    let results = results#getPositiveProbEntries () in
    let vals = PredicateTable.create (List.length results * 3) in
    let worklist = ref [] in
    List.iter (fun (p,v) -> PredicateTable.add vals p v) results; 
    List.iter (fun (p,_) -> worklist := p::!worklist) results;

    let assign_implicand implier_prob implicand = 
      try 
        let prob = PredicateTable.find vals implicand in
        if (prob < implier_prob) then 
          begin
            PredicateTable.replace vals implicand implier_prob;
            worklist := implicand::!worklist  
          end
        else () 
      with
        Not_found -> 
          begin
            PredicateTable.replace vals implicand implier_prob;
            worklist := implicand::!worklist  
          end
    in
      
    while (List.length !worklist <> 0) do
      let item = List.hd !worklist in 
      worklist := List.tl !worklist; 
      let implicands = impls#implicands item in
      let prob = PredicateTable.find vals item in 
      PredicateSet.iter (fun x -> assign_implicand prob x) implicands
    done;
    vals
  in

  let inX = open_in (tpi#getName()) in
  let inNotX = open_in (ntpi#getName()) in
  let outX = open_out (tpo#getName()) in
  let outNotX = open_out (ntpo#getName()) in
  
  for i = nr#getBeginRuns () to nr#getEndRuns () - 1 do
    let results = readRunResults sites i in

    let xProbs = readTruthProbs (input_line inX) predCount in
    addProbs xProbs preds results;

    let notXProbs = readTruthProbs (input_line inNotX) predCount in
    addProbs notXProbs notPreds results;

    let amplifiedResults = amplify implications results in

    analyze preds xProbs outX amplifiedResults;
    analyze notPreds notXProbs outNotX amplifiedResults
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
