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

class c_impl (default : string) (usage : string) (flag : string) (description :
string) =
  object (self)
    val name = new box default 
    method usage_msg () = usage 

    method argActions () =
      [(flag, String self#setName, description)]

    method setName n = name#set n

    method getName () = name#get ()

    method cleanup () = ()

  end 

let factory def u f des =
  let obj = new c_impl def u f des in (obj :> c) 
