open Arg

class type c =
  object
    method shouldDo : unit -> bool
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list
    method cleanup : unit -> unit 
  end

val factory : bool -> string -> string -> string -> c 
