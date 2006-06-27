open Arg
open CommandLine

let factory () = Flag.factory
  false
  "-do-logging (default is no logging)"
  "-do-logging"
  "if set logging is done"
