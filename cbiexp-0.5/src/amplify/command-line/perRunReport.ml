open Arg
open CommandLine

class type c =
  object
    method usage_msg : unit -> string
    method argActions : unit -> (key * spec * doc) list
    method setName : string -> unit
    method format : int -> string
    method cleanup : unit -> unit
  end

class c_impl (rd : RunsDirectory.c) (default : string) (usage : string) (flag : string) (description : string) =
  object (self)
    val name = new box default 

    method usage_msg () = usage 

    method argActions () =
      [(flag, String self#setName, description)]

    method setName n = name#set n

    method cleanup () = ()

    method format runId =
      rd#format runId (name#get ()) 
  end 

let factory rd default usage flag description =
  let obj = new c_impl rd default usage flag description in (obj :> c)

module InputReport =
  struct
    let factory rd = factory 
      rd
      "reports.sparse"
      "-input-report-name <input report name>"
      "-input-report-name"
      "input report name (default \"reports.sparse\")"
  end

module LogReport =
  struct
    let factory rd = factory
      rd
      "amplify.log"
      "-amplify-log-name <amplify log name>"
      "-amplify-log-name"
      "amplify log name (default \"amplify.log\")"
  end

module OutputReport =
  struct
    let factory rd = factory
      rd
      "reports.sparse.amplify"
      "-output-report-name <output report name>"
      "-output-report-name"
      "output report name (default \"reports.sparse.amplify\")"
  end
