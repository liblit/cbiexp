open Predicate

type entry = { index : int; siteId : int; values : float array } 

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

class type accumulator =
  object
    method addEntry : int -> int -> float array -> unit 
    method isTrue : Predicate.p -> bool
    method truthProb : Predicate.p -> float
    method getObservedTrue : unit -> Predicate.Set.t
    method getPositiveProbEntries : unit -> (p * float) list
    method addPredicate : Predicate.p -> unit
    method addPredicateProb : Predicate.p -> float -> unit 
    method getScheme : int -> string 
    method getSortedEntries : unit -> entry list
    method printSortedEntries : out_channel -> unit
    method xArray : float array -> float array
    method makeSitesArray : unit -> float array
  end


class virtual base t =
  object (self)
    val table : float array IntHash.t IntHash.t = IntHash.create 17 
    val trans : translator = t 

    method virtual xArray : float array -> float array 
    method virtual makeSitesArray : unit -> float array

    method getScheme index =
      trans#getScheme index 

    method addEntry index siteId values =

      let values = self#xArray values in

      let rec addValueAux index siteId values =
        try
          let sites = IntHash.find table index in
          IntHash.add sites siteId values
        with
          Not_found ->
            IntHash.add table index (IntHash.create 17);
            addValueAux index siteId values 
      in
        addValueAux index siteId values 

    method isTrue pred = self#truthProb pred = 1.0

    method truthProb pred =
      let index = trans#to_sparse pred.compilationUnit pred.scheme in
      try
        let sites = IntHash.find table index in
        let site = IntHash.find sites pred.site in
        Array.get site pred.id
      with
        Not_found -> 0.0 

    method getObservedTrue () =
      List.fold_left 
        (fun s (p,v) -> if v = 1.0 then PredicateSet.add p s else s)
        PredicateSet.empty
        (self#getPositiveProbEntries ())

    method getPositiveProbEntries () =
      let observedProb = ref [] in

      let processSites index sites =
        let (cU, scheme) = trans#from_sparse index in

        let processSite siteId values =

          let processValue i v =
            if v > 0.0 
              then observedProb := 
                ((Predicate.make cU scheme siteId i), v) :: !observedProb
              else ()
          in
            Array.iteri processValue values 

        in
          IntHash.iter processSite  sites 

      in
        IntHash.iter processSites table;
      !observedProb

    method addPredicate pred = self#addPredicateProb pred 1.0
      
    method addPredicateProb pred prob =
      let rec addPredicateAux pred =
        let index = trans#to_sparse pred.compilationUnit pred.scheme in
        try
          let sites = IntHash.find table index in
          try
            let site = IntHash.find sites pred.site in
            Array.set site pred.id prob
          with
            Not_found -> 
              let site = (self#makeSitesArray ()) in
              IntHash.add sites pred.site site;
              addPredicateAux pred 
        with
          Not_found -> 
            IntHash.add table index (IntHash.create 17); 
            addPredicateAux pred
      in
        addPredicateAux pred

    method private sortAux () =
      let getSorted id sites =
        let entries = IntHash.fold (fun k v l -> makeEntry id k v :: l) sites [] in
        List.sort (fun x y -> compare x.siteId y.siteId) entries
      in 

      let indexed = IntHash.fold (fun k v l -> (k, getSorted k v)::l) table [] in
      List.sort (fun x y ->  compare (fst x) (fst y) ) indexed 
      
    method getSortedEntries () =
      let sorted = self#sortAux () in
      List.fold_left (fun l (_,r) -> r @ l) [] sorted 

    method printSortedEntries outchannel =

      let printEntry outchannel entry =
        output_string outchannel (string_of_int entry.index);
        output_string outchannel "\t";
        output_string outchannel (string_of_int entry.siteId);
        Array.iter 
          (fun x -> output_string outchannel (if x = 1.0 then "\t1" else "\t0")) entry.values;
        output_string outchannel "\n"
      in
      
      let printSorted outchannel sites =
        List.iter (printEntry outchannel ) sites
      in

      let sorted = self#sortAux () in
      List.iter (fun (l,r) -> printSorted outchannel r) sorted

  end 

class core t =
  object (self)
    inherit base t
    method xArray arr = arr
    method makeSitesArray () = Array.make 3 0.0 
  end 

class synth t =
  object (self)
    inherit base t
    method xArray arr = Predicate.core_to_synth_array arr 
    method makeSitesArray () = Array.make 6 0.0
  end 
