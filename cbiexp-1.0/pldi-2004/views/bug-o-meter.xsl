<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  >


  <!-- a single Bug-O-Meter -->
  <xsl:template match="bug-o-meter">
    <xsl:variable name="width" select="@log10-true"/>
    <xsl:text>\bugometer{</xsl:text>
    <xsl:value-of select="$width"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="@context"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="@lower-bound"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="@fail - (@context + @lower-bound)"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="1 - @fail"/>
    <xsl:text>}</xsl:text>
  </xsl:template>


</xsl:stylesheet>
