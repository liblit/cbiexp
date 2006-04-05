open Arg
open Printf

type predicate = Predicate.p
module PredicateSet = Predicate.Set

let implFileName : string ref = ref ""
let mlFileName : string ref = ref ""

let usage_msg = "Usage: makeImplications -i <implication file name> -m <ml file
name>"

let argActions =
  [("-i", Set_string implFileName, "implications file name");
   ("-m", Set_string mlFileName, ".ml file name")]

let fpf outchannel l = 

  let fpf_rights o l =
    fprintf o "[%a]"
      (fun o x -> 
        match x with
          | [] -> ()
          | hd::tl ->
              List.iter (fun x -> fprintf o "%s; " (Predicate.string_of_p x)) tl;
              fprintf o "%s" (Predicate.string_of_p hd))
    l    
  in

  let fpf_impls o (k,v) =
    fprintf o "(%s,%a)" 
      (Predicate.string_of_p k)
      (fun o x -> fpf_rights o x)
      v
  in

  fprintf outchannel "let implications = [\n%a]\n" 
    (fun o x -> 
      match x with
        | [] -> ()
        | hd::tl ->
            List.iter (fun x -> fprintf o "%a;\n" fpf_impls x) tl;
            fprintf o "%a" fpf_impls hd )
    l

let dump () =
  let impls = new ImplicationAccumulator.c in
  let inchannel = open_in !implFileName in
  ImplLexer.readImplications inchannel (impls :> ImplLexer.implications);
  close_in inchannel;

  let implications = new Implications.c in
    List.iter (fun (l,r) -> implications#add l r) (impls#getImplications ());
  
  let implications = implications#toList () in
  let outchannel = open_out !mlFileName in 
    fpf outchannel implications; 
    close_out outchannel 

let main () =
  Arg.parse argActions (fun s -> ()) usage_msg;
  if !implFileName = "" or !mlFileName = "" 
  then begin
    usage argActions usage_msg;
    invalid_arg "Missing mandatory arguments"
  end
  else dump ()

let _ = main ()
