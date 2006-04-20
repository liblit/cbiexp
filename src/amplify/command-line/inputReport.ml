open Arg
open CommandLine

class c (rd : RunsDirectory.c) =
  object (self)
    val name = new box "reports.sparse"
    method usage_msg () = "[-input-report-name <input report name>]" 

    method argActions () =
      [("-input-report-name", String self#setName, "input report name (default \"reports.sparse\")")]

    method setName n = name#set n

    method cleanup () = ()

    method format runId =
      rd#format runId (name#get ()) 
  end 
