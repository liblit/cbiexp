open Arg
open CommandLine

class c =
  object (self) 
    val name = new box ""
    method usage_msg () = "-sites-report <sites report>"

    method argActions () =
      [("-sites-report", String self#setName, "sites report")]
     
    method setName n = name#set n

    method getName () = name#get ()

    method cleanup () =
      if not (name#isSet ()) then
        raise (Bad "no sites report specified")

  end
