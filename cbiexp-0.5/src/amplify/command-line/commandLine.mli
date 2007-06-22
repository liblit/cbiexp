open Arg

class ['a] box : 'a ->
  object
    val mutable x : 'a
    val mutable wasSet : bool
    method get : unit -> 'a
    method isSet : unit -> bool
    method set : 'a -> unit
  end

class type argParser = 
  object 
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list 
    method cleanup : unit -> unit
  end

class parser : argParser list -> string ->
  object
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list
    method cleanup : unit -> unit
    method parseCommandLine : unit -> unit
  end
