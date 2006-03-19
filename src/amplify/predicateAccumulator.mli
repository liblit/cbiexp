type predicate = Predicate.p 
 
class c :
  object
    method addPredicate : bool -> predicate -> unit
    method getPredicates : bool -> predicate list
  end

