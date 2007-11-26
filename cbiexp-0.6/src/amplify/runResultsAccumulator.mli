type entry = { index : int; siteId : int; values : float array }

class type translator =
  object 
    method from_sparse : int -> string * string
    method to_sparse : string -> string -> int
    method getScheme : int -> string
  end

class type accumulator = 
  object
    method addEntry : int -> int -> float array -> unit 
    method isTrue : Predicate.p -> bool
    method truthProb : Predicate.p -> float
    method getObservedTrue : unit -> Predicate.Set.t
    method getPositiveProbEntries : unit -> (Predicate.p * float) list
    method addPredicate : Predicate.p -> unit
    method addPredicateProb : Predicate.p -> float -> unit
    method getScheme : int -> string 
    method getSortedEntries : unit -> entry list
    method printSortedEntries : out_channel -> unit
    method xArray : float array -> float array
    method makeSitesArray : unit -> float array 
  end

class core : translator -> accumulator

class synth : translator -> accumulator


