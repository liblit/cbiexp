class type sites =
  object
    method addSiteInfo : string -> string -> unit
  end
      
val readSites : in_channel -> sites -> unit
