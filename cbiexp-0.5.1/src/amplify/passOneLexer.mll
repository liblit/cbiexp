{
  exception ParseFailure of string
  exception Eof
}

let nonZeroFormat = ['1' - '9'] ['0' - '9']* 
let indexFormat = "0" | nonZeroFormat 

rule initial preds = parse 
  | (indexFormat as id) '\t' (indexFormat as siteId) { 
      let id = int_of_string id in
      let siteId = int_of_string siteId in
      let scheme = preds#getScheme id in
      if scheme = "scalar-pairs" 
      then scalar_pairs id siteId preds lexbuf 
      else raise (ParseFailure "non scalar-pair")}  
  | eof { raise Eof }
and scalar_pairs id siteId preds = parse 
  | '\t' '0' '\t' '0' '\t' nonZeroFormat '\n' {
      preds#addEntry id siteId [| 0.0; 0.0; 1.0 |];
      initial preds lexbuf }
  | '\t' '0' '\t' nonZeroFormat '\t' '0' '\n' {
      preds#addEntry id siteId [| 0.0; 1.0; 0.0 |];
      initial preds lexbuf }
  | '\t' '0' '\t' nonZeroFormat '\t' nonZeroFormat '\n' {
      preds#addEntry id siteId [| 0.0; 1.0; 1.0 |];
      initial preds lexbuf }
  | '\t' nonZeroFormat '\t' '0' '\t' '0' '\n' {
      preds#addEntry id siteId [| 1.0; 0.0; 0.0 |];
      initial preds lexbuf }
  | '\t' nonZeroFormat '\t' '0' '\t' nonZeroFormat '\n' {
      preds#addEntry id siteId [| 1.0; 0.0; 1.0 |];
      initial preds lexbuf }
  | '\t' nonZeroFormat '\t' nonZeroFormat '\t' '0' '\n' {
      preds#addEntry id siteId [| 1.0; 1.0; 0.0 |];
      initial preds lexbuf }
  | '\t' nonZeroFormat '\t' nonZeroFormat '\t' nonZeroFormat '\n' {
      preds#addEntry id siteId [| 1.0; 1.0; 1.0 |];
      initial preds lexbuf }

{
  class type predicates =
    object
      method addEntry : int -> int -> float array -> unit
      method getScheme : int -> string
    end

  let readPredicates inchannel (preds : predicates)  =
    let lexbuf = Lexing.from_channel inchannel in 
    
    let readlines () =
      try 
        initial preds lexbuf 
      with
        Eof -> () 

    in readlines ()
}
