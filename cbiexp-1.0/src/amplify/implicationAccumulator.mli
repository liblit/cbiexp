type predicate = Predicate.p 

class c : 
  object
    method addImplication : predicate -> predicate -> unit

    method getImplications : unit -> (predicate * predicate) list
  end

