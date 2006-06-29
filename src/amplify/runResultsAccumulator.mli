type entry = { index : int; siteId : int; values : bool array }

class type translator =
  object 
    method from_sparse : int -> string * string
    method to_sparse : string -> string -> int
    method getScheme : int -> string
  end

class c : translator -> 
  object
    method addEntry : int -> int -> bool array -> unit 
    method isTrue : Predicate.p -> bool
    method getObservedTrue : unit -> Predicate.Set.t
    method addPredicate : Predicate.p -> unit
    method getScheme : int -> string 
    method getSortedEntries : unit -> entry list
    method printSortedEntries : out_channel -> unit
  end

