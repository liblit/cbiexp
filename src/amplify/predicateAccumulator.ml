open Predicate

type entry = { index : int; siteId : int; values : bool array } 

let makeEntry id si v = { index = id; siteId = si; values = v }  

module IntHash = Hashtbl.Make (
  struct
    type t = int
    let equal = (=) 
    let hash = Hashtbl.hash 
  end
)

module PredicateSet = Predicate.Set

class type translator =
  object 
    method from_sparse : int -> string * string 
    method to_sparse : string -> string -> int
    method getScheme : int -> string 
  end

class c t =
  object
    val preds : p list ref = ref [] 
    val trans : translator = t 

    method getScheme index =
      trans#getScheme index 

    method addEntry index siteId pred =
      let (cU,scheme) = trans#from_sparse index in
      preds := (Predicate.make cU scheme siteId pred)::!preds

    method getPredicates () =
      List.rev (!preds)

    method addPredicate pred =
      preds := pred::!preds

  end 
