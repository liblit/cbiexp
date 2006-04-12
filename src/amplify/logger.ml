open Predicate

type predicate = Predicate.p

module PredicateSet = Predicate.Set

class type c = 
  object
    method logNumChanged : int -> unit
    method logImplications : predicate -> PredicateSet.t -> unit
  end

class c_impl out =
  object (self)
    val outchannel = out 

    method logNumChanged num = 
      output_string outchannel ((string_of_int num)^"\n")

    method logImplications (left : predicate)  (rights : PredicateSet.t) =
      PredicateSet.iter 
        (fun x -> 
          Printf.fprintf 
            outchannel 
            "%s %s\n" 
            (string_of_p left) 
            (string_of_p x))
        rights
  end

class c_null out =
  object (self)
    val outchannel : out_channel = out

    method logNumChanged (num : int) = ()

    method logImplications (left : predicate) (rights : PredicateSet.t)  = ()
  end

let factory loggingOn out = 
  if loggingOn then ((new c_impl out) :> c) else ((new c_null out) :> c) 
