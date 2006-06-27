open Arg
open CommandLine

let factory () = AnalysisReport.factory
  "amps.txt"
  "-stats-report-name <stats report name>"
  "-stats-report-name"
  "stats report name (default \"amps.txt\")"
