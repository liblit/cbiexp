open Arg

class c : RunsDirectory.c -> 
  object 
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list 
    method setBeginRuns : int -> unit
    method getBeginRuns : unit -> int
    method setEndRuns : int -> unit
    method getEndRuns : unit -> int
    method cleanup : unit -> unit
  end
