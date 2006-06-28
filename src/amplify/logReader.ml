open Flag
open AnalysisReport
open PerRunReport

module PredicateTable = Predicate.Table

let rd = new RunsDirectory.c
let nr = new NumRuns.c rd
let lr = LogReport.factory rd
let sr = StatsReport.factory ()
let prr = PredRankReport.factory ()
let pr = RankPredsByRunsAmplified.factory ()
let c = CountAmplificationsPerRun.factory ()

let parsers = [ 
  (rd :> CommandLine.argParser) ; 
  (nr :> CommandLine.argParser) ; 
  (lr :> CommandLine.argParser) ;
  (sr :> CommandLine.argParser) ;
  (prr :> CommandLine.argParser) ;
  (pr :> CommandLine.argParser) ;
  (c :> CommandLine.argParser) ]

let parser = new CommandLine.parser parsers "logReader"

let countImplications logFileName =
  let inchannel = open_in logFileName in
  let count = LineCountLexer.read inchannel in
  close_in inchannel; count

let doCounts () =
  let count = ref 0 in 

  let outchannel = open_out (sr#getName ()) in

  for i = nr#getBeginRuns () to nr#getEndRuns () - 1 do
    let result = countImplications (lr#format i) in
    output_string outchannel ((string_of_int result)^"\n");
    count := !count + result
  done;

  output_string outchannel ("Total: "^(string_of_int !count)^"\n");
  close_out outchannel

let addPredicate table pred =
  try 
    let count = PredicateTable.find table pred
    in
      PredicateTable.replace table pred (count + 1)
  with 
    Not_found ->
      PredicateTable.add table pred 1

let doRanking () =
  let table = PredicateTable.create 37 in

  for i = nr#getBeginRuns () to nr#getEndRuns () - 1 do
    let impls = new ImplicationAccumulator.c in
    let inchannel = open_in (lr#format i) in 
    LogImplLexer.readImplications inchannel impls;
    List.iter 
      (fun (_,r) -> addPredicate table r) 
      (impls#getImplications ()); 
    close_in inchannel
  done;

  let preds = PredicateTable.fold (fun k v l -> (k,v)::l) table [] in

  let compare (k1, v1) (k2, v2) =
    let c = Pervasives.compare v1 v2 in
    if c = 0 then Pervasives.compare k1 k2 else c
  in

  let preds = List.sort compare preds in
  let outchannel = open_out (prr#getName ()) in
  List.iter (fun (p,c) -> 
              Printf.fprintf 
                outchannel 
                "%u %s\n" 
                c 
                (Predicate.string_of_p p)) preds;
  close_out outchannel

let doAll () =
 if (pr#shouldDo ()) 
   then
     if (c#shouldDo ()) then begin doCounts (); doRanking () end else doRanking ()
   else doCounts ()  


let main () =
  parser#parseCommandLine ();
  doAll () 

let _ =  main ()
