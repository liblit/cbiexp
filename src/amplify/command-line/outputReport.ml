open Arg
open CommandLine

class c (rd : RunsDirectory.c) =
  object (self)
    val name = new box "reports.amplify"
    method usage_msg () = "[-output-report-name <output report name>]"

    method argActions () =
      [("-output-report-name", String self#setName, "output report name (default \"reports.amplify\")")]

    method setName n = name#set n

    method cleanup () = ()

    method format runId =
      rd#format runId (name#get ()) 
  end 
