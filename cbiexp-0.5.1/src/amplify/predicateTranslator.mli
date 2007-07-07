class c :
  (string * string) array ->
  object
    method from_sparse : int -> string * string
    method to_sparse : string -> string -> int 
    method getCompilationUnit : int -> string
    method getScheme : int -> string
  end
