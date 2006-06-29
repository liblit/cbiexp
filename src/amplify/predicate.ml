type p = { compilationUnit : string; scheme : string; site : int; id : int }

let make cU sc si i = {compilationUnit = cU; scheme = sc; site = si; id = i}

let string_of_p p = 
  "{compilationUnit = \""^p.compilationUnit^"\"; "^
  "scheme = \""^p.scheme^"\"; "^
  "site = "^(string_of_int p.site)^"; "^
  "id = "^(string_of_int p.id)^"}"

let complement p =
  let compAux i = 
    match i with
      | 0 -> 1
      | 1 -> 0
      | 2 -> 3
      | 3 -> 2
      | 4 -> 5
      | 5 -> 4
      | _ -> failwith ("unknown relation id: "^(string_of_int i))
  in
    make p.compilationUnit p.scheme p.site (compAux p.id) 

let synth_to_ground_disjunction s =
  let compAux i =
    match i with
      | 0 -> [0]
      | 1 -> [1; 2]
      | 2 -> [1]
      | 3 -> [0; 2]
      | 4 -> [2]
      | 5 -> [0; 1]
      | _ -> failwith ("unknown synthesized relation index: "^(string_of_int i))
  in
    List.map 
      (fun x -> make s.compilationUnit s.scheme s.site x)
      (compAux s.id) 

module Set = Set.Make (
  struct 
    type t = p
    let compare l r = Pervasives.compare l r
  end )

module Table = Hashtbl.Make (
  struct
    type t = p

    let equal k1 k2 = 
      k1 = k2

    let hash k = Hashtbl.hash k 

  end )
