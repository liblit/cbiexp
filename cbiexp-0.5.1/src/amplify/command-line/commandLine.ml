open Arg

class ['a] box default =
  object(self)
    val mutable x : 'a = default 
    val mutable wasSet = false

    method set value =
      wasSet <- true;
      x <- value

    method get () =
      x

    method isSet () =
      wasSet

  end

class type argParser = 
  object 
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list 
    method cleanup : unit -> unit
  end

class parser p c =
  object (self)
    val parsers : argParser list = p
    val cmd : string = c

    method usage_msg () =
      "Usage: "^cmd^
      List.fold_right (fun x s -> " "^(x#usage_msg ())^s) parsers ""

    method argActions () =
      List.fold_right (fun x l -> (x#argActions ()) @ l) parsers []

    method cleanup () =
      List.iter (fun x -> x#cleanup ()) parsers

    method parseCommandLine () =
      Arg.parse (self#argActions ()) (fun x -> ()) (self#usage_msg ());
      self#cleanup ()

  end
