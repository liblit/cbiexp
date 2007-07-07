open Arg

class type c = 
  object 
    method usage_msg : unit -> string 
    method argActions : unit -> (key * spec * doc) list
    method setName : string -> unit
    method getNames : unit -> string list
    method cleanup : unit -> unit
  end

module ImplicationsReports :
  sig
    val factory : unit -> c
  end

module SitesReports :
  sig
    val factory : unit -> c
  end
