open Arg
open CommandLine

class c (rd : RunsDirectory.c) =
  object (self)
    val name = new box "amplify.log"
    method usage_msg () = "[-amplify-log-name <amplify log name>]"

    method argActions () =
      [("-amplify-log-name", String self#setName, "amplify log name (default \"amplify.log\")")]

    method setName n = name#set n

    method cleanup () = ()

    method format runId =
      rd#format runId (name#get ()) 
  end 
