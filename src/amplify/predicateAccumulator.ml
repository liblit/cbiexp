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
  object (self)
    val table : bool array IntHash.t IntHash.t = IntHash.create 17 
    val trans : translator = t 

    method getScheme index =
      trans#getScheme index 

    method addEntry index siteId values =

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

    method getObservedTrue () =
      let observedTrue = ref [] in

      let processSites index sites =
        let (cU, scheme) = trans#from_sparse index in

        let processSite siteId values =

          let processValue i v =
            if v 
              then observedTrue := (Predicate.make cU scheme siteId i) :: !observedTrue
              else ()
          in
            Array.iteri processValue values 

        in
          IntHash.iter processSite  sites 

      in
        IntHash.iter processSites table;

      List.fold_left (fun s x -> PredicateSet.add x s) PredicateSet.empty !observedTrue

    method addPredicate pred =
      let rec addPredicateAux pred =
        let index = trans#to_sparse pred.compilationUnit pred.scheme in
        try
          let sites = IntHash.find table index in
          try
            let site = IntHash.find sites pred.site in
            Array.set site pred.id true
          with
            Not_found -> 
              let site = Array.make (Schemes.getNumValues pred.scheme) false in
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
          (fun x -> output_string outchannel (if x = true then "\t1" else "\t0")) entry.values;
        output_string outchannel "\n"
      in
      
      let printSorted outchannel sites =
        List.iter (printEntry outchannel ) sites
      in

      let sorted = self#sortAux () in
      List.iter (fun (l,r) -> printSorted outchannel r) sorted
      
      

  end 
