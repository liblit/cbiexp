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
