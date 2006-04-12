open Arg

class c :
  object
    method doLogging : unit -> bool
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list 
    method cleanup : unit -> unit
  end
