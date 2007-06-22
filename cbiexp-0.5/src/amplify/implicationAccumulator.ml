type predicate = Predicate.p 

class c =
  object
    val implications : (predicate * predicate) list ref = ref [] 

    method addImplication l r = implications := (l,r)::!implications

    method getImplications () = !implications

  end 

