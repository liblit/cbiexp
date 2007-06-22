open Arg
open CommandLine

class type c =
  object
    method shouldDo : unit -> bool
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list
    method cleanup : unit -> unit 
  end


class c_impl (default : bool) (usage : string) (flag : string) (description : string) =
  object (self)
    val on = ref default 

    method shouldDo () = !on

    method usage_msg () = usage 

    method argActions () =
      [(flag, Set on, description)]

    method cleanup () = ()

  end

let factory default usage flag description =
  let obj = new c_impl default usage flag description in (obj :> c)

module CountAmplificationsPerRun =
  struct
    let factory () = factory
      false
      "-do-counts (default is no counts)"
      "-do-counts"
      "if set number of amplifications per run is recorded"
  end

module Log =
  struct
    let factory () = factory
      false
      "-do-logging (default is no logging)"
      "-do-logging"
      "if set logging is done"
  end

module RankPredsByRunsAmplified =
  struct
    let factory () = factory
      false
      "-rank-preds (default is no)"
      "-rank-preds"
      "if set predicates are ranked according to the number of times amplified in this data set" 
  end  
