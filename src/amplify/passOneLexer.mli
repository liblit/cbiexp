class type predicates =
  object
    method addEntry : int -> int -> float array -> unit
    method getScheme : int -> string
  end

val readPredicates : in_channel -> predicates -> unit
