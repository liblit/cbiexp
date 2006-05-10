open Arg

class c :
  object
    method usage_msg : unit -> string 
    method argActions : unit -> (key * spec * doc) list 
    method setRoot : string -> unit
    method getRoot : unit -> string
    method format : int -> string -> string
    method cleanup : unit -> unit
  end
