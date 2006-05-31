open Arg

class type c = 
  object 
    method usage_msg : unit -> string 
    method argActions : unit -> (key * spec * doc) list
    method setName : string -> unit
    method getNames : unit -> string list
    method cleanup : unit -> unit
  end

val factory : string -> string -> string -> c 
