<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:template match="bug-o-meter">
    <td>
      <table class="scores" width="{@log10-true * 60 + 1}px" title="Ctxt: {round(@context * 100)}%, LB: {round(@lower-bound * 100)}%, Incr: {round(@increase * 100)}%, Fail: {round(@fail * 100)}%&#10;tru in {@true-success} S and {@true-failure} F&#10;obs in {@seen-success} S and {@seen-failure} F">
	<tr>
	  <td class="f1" style="width: {@context * 100}%"/>
	  <td class="f2" style="width: {@lower-bound * 100}%"/>
	  <td class="f3" style="width: {(@fail - (@context + @lower-bound)) * 100}%"/>
	  <td class="f4" style="width: {(1 - @fail) * 100}%"/>
	</tr>
      </table>
    </td>
  </xsl:template>

</xsl:stylesheet>
