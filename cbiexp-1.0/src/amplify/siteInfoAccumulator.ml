class c =
  object
    val infos : (string * string) list ref = ref []
    method addSiteInfo cU scheme = infos := (cU, scheme)::!infos
    method getSiteInfos () = Array.of_list (List.rev !infos)
  end
    
