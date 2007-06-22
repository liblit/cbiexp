open Arg

class type c = 
  object 
    method usage_msg : unit -> string 
    method argActions : unit -> (key * spec * doc) list
    method setName : string -> unit
    method format : int -> string
    method cleanup : unit -> unit
  end

module InputReport :
  sig
    val factory : RunsDirectory.c -> c 
  end

module LogReport :
  sig
    val factory : RunsDirectory.c -> c
  end

module OutputReport :
  sig
    val factory : RunsDirectory.c -> c
  end
