open Arg
open CommandLine

let factory rd = PerRunReport.factory 
  rd
  "reports.sparse"
  "-input-report-name <input report name>"
  "-input-report-name"
  "input report name (default \"reports.sparse\")"
