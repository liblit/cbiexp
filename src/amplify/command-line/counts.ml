open Arg
open CommandLine

let factory () = Flag.factory
  false
  "-do-counts (default is no counts)"
  "-do-counts"
  "if set number of amplifications per run is recorded"
