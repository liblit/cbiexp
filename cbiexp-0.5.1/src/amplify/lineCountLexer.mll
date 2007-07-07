{
  exception ParseFailure of string
  exception Eof
}

rule initial count = shortest
  | _* '\n' { initial (count + 1) lexbuf } 
  | eof { count }

{

  let read inchannel =
    let lexbuf = Lexing.from_channel inchannel in 
    
    let readlines () =
      initial 0 lexbuf 

    in readlines ()
}
