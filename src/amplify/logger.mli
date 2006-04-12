class type c = 
  object
    method logNumChanged : int -> unit 
    method logImplications : Predicate.p -> Predicate.Set.t -> unit 
  end

val factory : bool -> out_channel -> c
