{
  type predicate = Predicate.p
  exception ParseFailure of string
  exception Eof
}

let compilationUnit = ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']

let nonZeroCount = ['1' - '9'] ['0' - '9']* 

rule initial preds = shortest 
  | "<report id=\"samples\">\n"		{ report preds lexbuf }  
  | "<report id=\"" _* "\">\n"      	{ report_skip preds lexbuf }
  | eof					{ raise Eof }
  | _* '\n'  				{ raise (ParseFailure "initial") }
and report preds = shortest
  | "</report>\n"			{ initial preds lexbuf } 
  | "<samples unit=\"" 
      (compilationUnit as cU) 
      "\" scheme=\"scalar-pairs\">\n"	{ scalar_pairs preds cU 0 lexbuf }
  | "<samples unit=\""
      compilationUnit
      "\" scheme=\""
      ['a'-'z' '-']+ 
      "\">\n"				{ sites_skip preds lexbuf }
  | _* '\n'				{ raise (ParseFailure "report") }
and report_skip preds = shortest 
  | "</report>\n"			{ initial preds lexbuf }
  | _* '\n'				{ report_skip preds lexbuf }
and scalar_pairs preds cU siteId = shortest
  | "</samples>\n"			{ report preds lexbuf }
  | '0' '\t' '0' '\t' '0' '\n'          { 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      preds#addPredicate false (mkP 0);
      preds#addPredicate false (mkP 1);
      preds#addPredicate false (mkP 2);
      scalar_pairs preds cU (siteId + 1) lexbuf } 
  | '0' '\t' '0' '\t' nonZeroCount '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      preds#addPredicate false (mkP 0);
      preds#addPredicate false (mkP 1);
      preds#addPredicate true (mkP 2);
      scalar_pairs preds cU (siteId + 1) lexbuf }
  | '0' '\t' nonZeroCount '\t' '0' '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      preds#addPredicate false (mkP 0);
      preds#addPredicate true (mkP 1);
      preds#addPredicate false (mkP 2);
      scalar_pairs preds cU (siteId + 1) lexbuf } 
  | '0' '\t' nonZeroCount '\t' nonZeroCount '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      preds#addPredicate false (mkP 0);
      preds#addPredicate true (mkP 1);
      preds#addPredicate true (mkP 2);
      scalar_pairs preds cU (siteId + 1) lexbuf } 
  | nonZeroCount '\t' '0' '\t' '0' '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      preds#addPredicate true (mkP 0);
      preds#addPredicate false (mkP 1);
      preds#addPredicate false (mkP 2);
      scalar_pairs preds cU (siteId + 1) lexbuf } 
  | nonZeroCount '\t' '0' '\t' nonZeroCount '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      preds#addPredicate true (mkP 0);
      preds#addPredicate false (mkP 1);
      preds#addPredicate true (mkP 2);
      scalar_pairs preds cU (siteId + 1) lexbuf } 
  | nonZeroCount '\t' nonZeroCount '\t' '0' '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      preds#addPredicate true (mkP 0);
      preds#addPredicate true (mkP 1);
      preds#addPredicate false (mkP 2);
      scalar_pairs preds cU (siteId + 1) lexbuf } 
  | nonZeroCount '\t' nonZeroCount '\t' nonZeroCount '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      preds#addPredicate true (mkP 0);
      preds#addPredicate true (mkP 1);
      preds#addPredicate true (mkP 2);
      scalar_pairs preds cU (siteId + 1) lexbuf } 
and sites_skip preds = shortest
  | "</samples>\n"			{ report preds lexbuf }
  | _* '\n'				{ sites_skip preds lexbuf } 

{
  class type predicates =
    object
      method addPredicate : bool -> predicate -> unit
    end

  let readPredicates inchannel preds =
    let lexbuf = Lexing.from_channel inchannel in 
    
    let readlines () =
      try 
        while true do 
          initial preds lexbuf 
        done 
      with
        Eof -> () 

    in readlines ()
}
