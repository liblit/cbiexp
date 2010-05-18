<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:template match="/" mode="head-extra">
    <script type="text/javascript" src="bug-o-meter.js">
      <xsl:comment><xsl:text> </xsl:text></xsl:comment>
    </script>
  </xsl:template>

  <xsl:template match="bug-o-meter">
	  	  
      <table class="meter" width="{@log10-true * 60 + 1}px">
	<tr>
	  <td class="f1" style="width: {@context * 100}%"/>
	  <td class="f2" style="width: {@lower-bound * 100}%"/>
	  <td class="f3" style="width: {(@fail - (@context + @lower-bound)) * 100}%"/>
	  <td class="f4" style="width: {(1 - @fail) * 100}%"/>
	</tr>
      </table>
      <div class="details">
	<p>Increase: <xsl:value-of select="round(@increase * 100)"/>%, Lower Bound: <xsl:value-of select="round(@lower-bound * 100)"/>%<br/>Fail: <xsl:value-of select="round(@fail * 100)"/>%, Context: <xsl:value-of select="round(@context * 100)"/>%</p>
	<hr/>
	<p>true in <xsl:value-of select="@true-success"/> successes and <xsl:value-of select="@true-failure"/> failures<br/>observed in <xsl:value-of select="@seen-success"/> successes and <xsl:value-of select="@seen-failure"/> failures</p>
      </div>  
  </xsl:template>

</xsl:stylesheet>
