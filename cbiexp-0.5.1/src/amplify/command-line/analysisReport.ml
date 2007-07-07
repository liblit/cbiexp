open Arg
open CommandLine

class type c =
  object
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list
    method setName : string -> unit
    method getName : unit -> string
    method cleanup : unit -> unit 
  end

class c_impl (default : string) (usage : string) (flag : string) (description :
string) =
  object (self)
    val name = new box default 
    method usage_msg () = usage 

    method argActions () =
      [(flag, String self#setName, description)]

    method setName n = name#set n

    method getName () = name#get ()

    method cleanup () = ()

  end 

let factory def u f des =
  let obj = new c_impl def u f des in (obj :> c) 

module StatsReport =
  struct
    let factory () = factory
      "amps.txt"
      "-stats-report-name <stats report name>"
      "-stats-report-name"
      "stats report name (default \"amps.txt\")"
  end

module PredRankReport =
  struct
    let factory () = factory
      "preds_amplified.txt"
      "-pred-rank-report-name <predicate ranking report name>"
      "-pred-rank-report-name"
      "preds amplification statistics report name (default \"preds_amplified.txt\")"
  end

module TruthProbabilitiesInput =
  struct
    let factory () = factory
      "X.dat"
      "-tp-report <truth probabilities report>"
      "-tp-report"
      "truth probabilities report name (default \"X.dat\")"
  end

module NotTruthProbabilitiesInput =
  struct  
    let factory () = factory
      "notX.dat"
      "-ntp-report <truth probabilities for predicate complements report>"
      "-ntp-report"
      "truth probabilities for predicate complements report name (default \"notX.dat\")"
  end 

module TruthProbabilitiesOutput =
  struct
    let factory () = factory
      "X.amplify.dat"
      "-tp-amplify <amplified truth probabilities report>"
      "-tp-amplify"
      "amplified truth probabilities report name (default \"X.amplify.dat\")"
  end

module NotTruthProbabilitiesOutput =
  struct
    let factory () = factory
      "notX.amplify.dat"
      "-ntp-amplify <amplified truth probabilities for predicate complements report>"
      "-ntp-amplify"
      "amplified truth probabilities for predicate complements report name (default \"notX.amplify.dat\")"
  end

module PredsReport =
  struct
    let factory () = factory 
      "preds.txt"
      "-preds-report <file containing interesting predicates>"
      "-preds-report"
      "file containing interesting predicates (default \"preds.txt\")"
  end

module LogReport =
  struct
    let factory () = factory 
      "amps.log"
      "-log-report <log of amplifications>"
      "-log-report"
      "log predicates amplified to this file (defulat \"amps.log\")"
  end
