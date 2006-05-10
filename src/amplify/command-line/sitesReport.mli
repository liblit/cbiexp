open Arg

class c :  
  object 
    method usage_msg : unit -> string 
    method argActions : unit -> (key * spec * doc) list
    method setName : string -> unit
    method getName : unit -> string
    method cleanup : unit -> unit
  end
