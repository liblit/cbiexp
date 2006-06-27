open Arg
open CommandLine

let factory rd = PerRunReport.factory
  rd
  "amplify.log"
  "-amplify-log-name <amplify log name>"
  "-amplify-log-name"
  "amplify log name (default \"amplify.log\")"
