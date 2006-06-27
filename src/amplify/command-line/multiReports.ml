open Arg
open CommandLine

class type c = 
  object
    method usage_msg : unit -> string 
    method argActions : unit -> (key * spec * doc) list
    method setName : string -> unit
    method getNames : unit -> string list
    method cleanup : unit -> unit
  end

class c_impl (usage : string) (flag : string) description = 
  object (self)
    val names : string list ref = ref [] 

    method usage_msg () = usage 

    method argActions () =
      [(flag, String self#setName, description)]

    method setName n = names := n::!names 

    method getNames () = List.rev (!names) 

    method cleanup () =
      if (List.length !names = 0) then 
        raise (Bad ("no "^description^" specified"))

  end 

let factory u f d = 
  let obj = new c_impl u f d in (obj :> c)  
