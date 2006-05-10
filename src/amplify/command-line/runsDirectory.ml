open Arg
open CommandLine

class c =
  object(self)
    val runsPerSubdirectory = 10000
    val root = new box "." 
    method usage_msg () = "[-runs-directory <runs directory>]"

    method argActions () =
      [("-runs-directory", String self#setRoot, "look for scripted runs in this directory (default \".\")")] 

    method setRoot r =
      root#set r

    method getRoot () = 
      root#get ()

    method format runId filename =
      let subdirectory = runId/runsPerSubdirectory in
      (root#get ())^"/"^
      (string_of_int subdirectory)^"/"^
      (string_of_int runId)^"/"^
      filename

    method cleanup () = ()
    
  end
