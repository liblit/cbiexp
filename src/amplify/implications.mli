type predicate = Predicate.p 

class c :
  object
    method add : predicate -> predicate -> unit
    method holds : predicate -> predicate -> bool
  end
