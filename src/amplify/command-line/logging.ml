open Arg
open CommandLine

class c =
  object (self)
    val log = ref false

    method doLogging () = !log

    method usage_msg () = "[-do-logging (default is no logging)]"

    method argActions () =
      [("-do-logging", Set log, "if set logging is done")]

    method cleanup () = ()

  end
