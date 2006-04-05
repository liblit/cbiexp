open Arg

type predicate = Predicate.p
module PredicateSet = Predicate.Set 

let reportFileName : string ref = ref ""
let outputFileName : string ref = ref ""
let logFileName : string ref = ref "" 

let usage_msg = "Usage: amplify -r <report file
name> -o <output file name> [-l <log file name>]" 

let argActions =
  [("-r", Set_string reportFileName, "report file name");
   ("-o", Set_string outputFileName, "output file name");
   ("-l", Set_string logFileName, "log file name");]

let log numChanged =
    let doLogging = !logFileName <> "" in
    let outchannel = open_out !logFileName in
    output_string outchannel ((string_of_int numChanged)^"\n"); 
    close_out outchannel

let doAnalysis () = 
  let preds = new PredicateAccumulator.c in 
  let inchannel = open_in !reportFileName in
  PassOneLexer.readPredicates inchannel (preds :> PassOneLexer.predicates); 
  close_in inchannel;

  let observedTrue =
    List.fold_left (fun s x -> PredicateSet.add x s) PredicateSet.empty (preds#getPredicates true ) in
  let notObservedTrue =
    List.fold_left (fun s x -> PredicateSet.add x s) PredicateSet.empty (preds#getPredicates false ) in

    (* second pass *)
  let areTrue =
    (Reconstruct.doAnalysis (new Impls.c :> Reconstruct.implications) observedTrue notObservedTrue) 
  in 
  let areChanged = PredicateSet.diff areTrue observedTrue in
  log (PredicateSet.cardinal areChanged);
  let ps = new Predicates.c areChanged in
  let inchannel = open_in !reportFileName in
  let outchannel = open_out !outputFileName in
  PassTwoLexer.updatePredicates inchannel outchannel (ps :> PassTwoLexer.predicates);
  close_in inchannel;
  close_out outchannel

let main () =
  Arg.parse argActions (fun s -> ()) usage_msg;
  if !reportFileName = "" or !outputFileName = "" 
  then begin 
    usage argActions usage_msg; 
    invalid_arg "Missing mandatory arguments" 
  end
  else doAnalysis () 

let _ =  main ()
