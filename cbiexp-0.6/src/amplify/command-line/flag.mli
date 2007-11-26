open Arg

class type c =
  object
    method shouldDo : unit -> bool
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list
    method cleanup : unit -> unit 
  end

module CountAmplificationsPerRun : 
  sig
    val factory : unit -> c
  end

module Log :
  sig
    val factory : unit -> c
  end

module RankPredsByRunsAmplified :
  sig
    val factory : unit -> c
  end
