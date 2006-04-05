type predicate = Predicate.p 

class c :
  object
    method add : predicate -> predicate -> unit
    method holds : predicate -> predicate -> bool
    method toList : unit -> (predicate * predicate list) list
  end
