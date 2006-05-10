open Arg

class c : RunsDirectory.c ->
  object 
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list
    method setName : string -> unit
    method format : int -> string
    method cleanup : unit -> unit
  end
