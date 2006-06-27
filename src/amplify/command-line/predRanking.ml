open Arg
open CommandLine

let factory () = Flag.factory
  false
  "-rank-preds (default is no)"
  "-rank-preds"
  "if set predicates are ranked according to the number of times amplified in this data set" 
