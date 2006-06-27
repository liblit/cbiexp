open Arg
open CommandLine

let factory rd = PerRunReport.factory
  rd
  "reports.sparse.amplify"
  "-output-report-name <output report name>"
  "-output-report-name"
  "output report name (default \"reports.sparse.amplify\")"

