open Predicate

type predicate = Predicate.p

module PredicateSet = Predicate.Set

class type c = 
  object
    method logImplications : predicate -> PredicateSet.t -> unit
    method logImplication : predicate -> predicate -> unit
    method logPredicate : predicate -> unit
    method advance : unit -> unit
  end

class c_impl out =
  object (self)
    val outchannel = out 

    method logImplications (left : predicate)  (rights : PredicateSet.t) =
      PredicateSet.iter 
        (fun x -> 
          Printf.fprintf 
            outchannel 
            "%s %s\n" 
            (string_of_p left) 
            (string_of_p x))
        rights

    method logImplication (left : predicate) (right : predicate) =
      Printf.fprintf outchannel "%s %s\t" (string_of_p left) (string_of_p right)

    method logPredicate (p : predicate) =
      Printf.fprintf outchannel "%s\t" (string_of_p p)

    method advance () = Printf.fprintf outchannel "\n"

  end

class c_null out =
  object (self)
    val outchannel : out_channel = out

    method logImplications (left : predicate) (rights : PredicateSet.t)  = ()
    method logImplication (left : predicate) (right : predicate)  = ()
    method logPredicate (p : predicate) = ()
    method advance () = () 
  end

let factory loggingOn out = 
  if loggingOn then ((new c_impl out) :> c) else ((new c_null out) :> c) 
