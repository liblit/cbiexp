open Arg

type predicate = Predicate.p
module PredicateSet = Predicate.Set 

let implFileName : string ref = ref ""
let reportFileName : string ref = ref ""
let outputFileName : string ref = ref ""

let usage_msg = "Usage: sharpen -i <implication file name> -r <report file name> -o <output file name>" 

let argActions =
  [("-i", Set_string implFileName, "implication file name");
   ("-r", Set_string reportFileName, "report file name");
   ("-o", Set_string outputFileName, "output file name")]

let doAnalysis () = 
  let impls = new ImplicationAccumulator.c in
  let inchannel = open_in !implFileName in
  ImplLexer.readImplications inchannel (impls :> ImplLexer.implications);
  close_in inchannel;

  let preds = new PredicateAccumulator.c in 
  let inchannel = open_in !reportFileName in
  PassOneLexer.readPredicates inchannel (preds :> PassOneLexer.predicates); 
  close_in inchannel;

  let observedTrue =
    List.fold_left (fun s x -> PredicateSet.add x s) PredicateSet.empty (preds#getPredicates true ) in
  let notObservedTrue =
    List.fold_left (fun s x -> PredicateSet.add x s) PredicateSet.empty (preds#getPredicates false ) in

    (* build implication structure *)
  let implications = new Implications.c in
    List.iter (fun (l,r) -> implications#add l r) (impls#getImplications ()); 
 
    (* second pass *)
  let areTrue =
    (Sharpen.doAnalysis (implications :> Sharpen.implications) observedTrue notObservedTrue) 
  in 
  let ps = new Predicates.c areTrue in
  let inchannel = open_in !reportFileName in
  let outchannel = open_out !outputFileName in
  PassTwoLexer.updatePredicates inchannel outchannel (ps :> PassTwoLexer.predicates);
  close_in inchannel;
  close_out outchannel

let main () =
  Arg.parse argActions (fun s -> ()) usage_msg;
  if !implFileName = "" or !reportFileName = "" or !outputFileName = "" 
  then usage argActions usage_msg 
  else doAnalysis () 

let _ =  main ()
