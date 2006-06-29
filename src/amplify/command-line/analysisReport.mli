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

val factory : string -> string -> string -> string -> c 

module StatsReport :
  sig
    val factory : unit -> c
  end

module PredRankReport :
  sig
    val factory : unit -> c
  end

module TruthProbabilitiesInput :
  sig
    val factory : unit -> c
  end

module NotTruthProbabilitiesInput :
  sig
    val factory : unit -> c
  end

module TruthProbabilitiesOutput :
  sig
    val factory : unit -> c
  end

module NotTruthProbabilitiesOutput :
  sig
    val factory : unit -> c
  end

module PredsReport :
  sig
    val factory : unit -> c
  end
