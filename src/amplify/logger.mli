class type c = 
  object
    method logImplications : Predicate.p -> Predicate.Set.t -> unit 
    method logImplication : Predicate.p -> Predicate.p -> unit
    method advance : unit -> unit
  end

val factory : bool -> out_channel -> c
