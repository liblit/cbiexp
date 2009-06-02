class type preds =
  object
    method addEntry : int -> int -> int -> unit 
  end

val read : in_channel -> preds -> unit
