open Arg
open CommandLine

class c = 
  object (self)
    val name = new box ""
    method usage_msg () = "-implications-report <implications report>"

    method argActions () =
      [("-implications-report", String self#setName, "implications report")]

    method setName n = name#set n

    method getName () = name#get ()

    method cleanup () =
      if not (name#isSet ()) then
        raise (Bad "no implications report specified")

  end 
