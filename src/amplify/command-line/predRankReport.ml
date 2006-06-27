open Arg
open CommandLine

let factory () = AnalysisReport.factory
  "preds_amplified.txt"
  "-pred-rank-report-name <predicate ranking report name>"
  "-pred-rank-report-name"
  "preds amplification statistics report name (default \"preds_amplified.txt\")"
