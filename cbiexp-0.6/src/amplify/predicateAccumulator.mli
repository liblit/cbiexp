type entry = { index : int; siteId : int; values : bool array }

class type translator =
  object 
    method from_sparse : int -> string * string
    method to_sparse : string -> string -> int
    method getScheme : int -> string
  end

class c : translator -> 
  object
    method addEntry : int -> int -> int -> unit 
    method addPredicate : Predicate.p -> unit
    method getPredicates : unit -> Predicate.p list
    method getScheme : int -> string 
  end

