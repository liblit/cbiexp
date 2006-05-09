open Arg
open CommandLine

class c =
  object (self)
    val name = new box "amps.txt"
    method usage_msg () = "[-stats-report-name <stats report name>]"

    method argActions () =
      [("-stats-report-name", String self#setName, "stats report name (default \"amps.txt\")")]

    method setName n = name#set n

    method getName () = name#get ()

    method cleanup () = ()

  end 
