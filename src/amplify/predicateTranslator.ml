module Index = Hashtbl.Make (
  struct
    type t = string * string

    let equal x y = 
      fst x = fst y && snd x = snd y 

    let hash = Hashtbl.hash 
  end
)

let makeIndex a =
  let table = Index.create (Array.length a) in

  let insert i (cU, scheme) =
    Index.add table (cU, scheme) i
  in
    Array.iteri insert a; table

class c (siteInfos : (string * string) array)=
  object (self)
    val table = siteInfos
    val index = makeIndex siteInfos
    method from_sparse num =
      siteInfos.(num)

    method to_sparse cU scheme =
      Index.find index (cU, scheme)

    method getCompilationUnit index =
      fst (self#from_sparse index)

    method getScheme index =
      snd (self#from_sparse index)
      
  end
